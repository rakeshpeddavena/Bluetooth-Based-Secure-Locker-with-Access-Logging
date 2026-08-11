/*
 * kpm.c
 * -----------------------------------------------------------------------
 * 4x4 matrix keypad driver, PORT1 rows/cols (see kpm_defines.h).
 * Scanning strategy: drive each row low in turn and see which column
 * reads low, i.e. classic row-scan with active-low columns.
 *
 * Layout:
 *      1 2 3 A
 *      4 5 6 B
 *      7 8 9 C
 *      * 0 # D
 *  '#' = ENTER,  '*' = BACKSPACE
 * -----------------------------------------------------------------------
 */

#include <LPC21xx.h>
#include "types.h"
#include "define.h"
#include "kpm_defines.h"
#include "lcd_defines.h"
#include "kpm.h"
#include "lcd.h"
#include "delay.h"

u8 kpmLUT[4][4] = {
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'}
};

/* Rows are outputs (driven low one at a time), columns default to
 * inputs and don't need explicit configuration. */
void InitKPM(void)
{
	WRITENIBBLE(IODIR1, ROW0, 15);
}

/* Quick check: is any column currently pulled low (with all rows
 * driven low)? Returns 0 if a key is pressed, 1 if the keypad is idle. */
u8 Colscan(void)
{
	return (READNIBBLE(IOPIN1, COL0) < 15) ? 0 : 1;
}

/* Drive rows low one at a time until Colscan() reports a pressed key,
 * to find out which row the key is in. Leaves all rows low afterwards
 * (needed so Colcheck() can then read the actual column). */
u8 Rowcheck(void)
{
	u8 r;
	for(r = 0; r < 4; r++)
	{
		WRITENIBBLE(IOPIN1, ROW0, ~(1 << r));   /* drive only row r low */
		if(!Colscan()) break;                    /* key found in this row */
	}
	WRITENIBBLE(IOPIN1, ROW0, 0);                 /* drive all rows low again */
	return r;
}

/* With all rows held low, find which column reads low. */
u8 Colcheck(void)
{
	u8 c;
	for(c = 0; c < 4; c++)
		if(READBIT(IOPIN1, COL0 + c) == 0) break;
	return c;
}

/* Block until a key is pressed, debounce, look it up in the keymap,
 * then block until it's released. Returns the key's ASCII value. */
u8 KeyScan(void)
{
	u8 r, c, key;
	while(Colscan());           /* wait for press */
	delay_ms(50);                /* debounce */
	r   = Rowcheck();
	c   = Colcheck();
	key = kpmLUT[r][c];
	while(!Colscan());          /* wait for release */
	delay_ms(20);
	return key;
}

/* Same as KeyScan(), but gives up and returns 0 if no key is pressed
 * within timeout_ms. Polls in 10 ms steps while idle. */
u8 KeyScanTimeout(u32 timeout_ms)
{
	u32 t = 0;
	while(Colscan())
	{
		delay_ms(10);
		t += 10;
		if(t >= timeout_ms)
			return 0;
	}
	return KeyScan();
}

/* Collect digits/keys into buf until '#' (ENTER) is pressed, with '*'
 * acting as backspace (erases the previous LCD character too). No
 * timeout - used where a timeout isn't required. */
void GetPwd(u8 *buf, u8 max)
{
	u8 i = 0, k;
	while(i < max - 1)
	{
		k = KeyScan();
		if(k == '#') break;

		if(k == '*' && i > 0)
		{
			i--;
			CmdLCD(GOTO_LINE2_POS0 + i);
			CharLCD(' ');
			CmdLCD(GOTO_LINE2_POS0 + i);
			continue;
		}

		buf[i++] = k;
		CharLCD('*');            /* mask the entered character on the LCD */
	}
	buf[i] = '\0';
}

/* Same as GetPwd(), but aborts and returns 0 if the keypad stays idle
 * for timeout_ms while waiting for the next keystroke. Returns 1 if
 * the password was entered ('#' pressed) before timing out. */
u8 GetPwdTimeout(u8 *buf, u8 max, u32 timeout_ms)
{
	u8 i = 0, k;
	u32 t = 0;

	while(i < max - 1)
	{
		while(Colscan())
		{
			delay_ms(10);
			t += 10;

			if(t >= timeout_ms)
			{
				buf[0] = '\0';
				return 0;          /* timed out */
			}
		}

		k = KeyScan();

		if(k == '#')
			break;

		if(k == '*' && i > 0)
		{
			i--;
			CmdLCD(GOTO_LINE2_POS0 + i);
			CharLCD(' ');
			CmdLCD(GOTO_LINE2_POS0 + i);
			continue;
		}

		buf[i++] = k;
		CharLCD('*');
	}

	buf[i] = '\0';
	return 1;                      /* entered successfully */
}

/*
 * GetNumber - like GetPwd(), but only accepts digits 0-9 (used by the
 * admin RTC-edit menu for hour/minute/second/day/month/year entry) and
 * echoes the actual digit on the LCD instead of masking it with '*'.
 * '*' = backspace, '#' = confirm.
 */
void GetNumber(u8 *buf, u8 max)
{
	u8 i = 0;
	u8 k;

	while(i < max-1)
	{
		k = KeyScan();

		if(k == '#')
			break;

		if(k == '*' && i > 0)
		{
			i--;

			CmdLCD(GOTO_LINE2_POS0 + i);
			CharLCD(' ');
			CmdLCD(GOTO_LINE2_POS0 + i);

			continue;
		}

		if(k < '0' || k > '9')
			continue;               /* ignore letter keys (A-D) and '*' at i==0 */

		buf[i++] = k;

		CharLCD(k);
	}

	buf[i] = '\0';
}
