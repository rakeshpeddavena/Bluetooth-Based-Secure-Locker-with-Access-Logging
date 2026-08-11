/*
 * lcd.c
 * -----------------------------------------------------------------------
 * Driver for a standard HD44780-compatible 16x2 character LCD wired in
 * 8-bit parallel mode. Used to show system status, prompts and the
 * masked password entry (*) during authentication.
 * -----------------------------------------------------------------------
 */

#include <LPC21xx.h>
#include "lcd_defines.h"
#include "define.h"
#include "delay.h"
#include "types.h"

/* Latch one byte onto the data bus with the standard EN pulse. */
void WriteLCD(u8 byte)
{
	IOCLR0 = 1 << LCD_RW;              /* RW = 0 (write) */
	WRITEBYTE(IOPIN0, LCD_DATA, byte); /* place byte on the data bus */
	IOSET0 = 1 << LCD_EN;              /* EN high        */
	delay_us(1);
	IOCLR0 = 1 << LCD_EN;              /* EN low - LCD latches the byte on this edge */
	delay_ms(2);                       /* generous margin for the LCD's internal execution time */
}

/* Send a command byte (RS = 0), e.g. clear display, cursor position. */
void CmdLCD(u8 cmd)
{
	IOCLR0 = 1 << LCD_RS;
	WriteLCD(cmd);
}

/* Send a data byte (RS = 1), i.e. a character to display. */
void CharLCD(u8 ch)
{
	IOSET0 = 1 << LCD_RS;
	WriteLCD(ch);
}

/* Configure the GPIO pins as outputs and run the standard HD44780
 * power-on initialisation sequence for 8-bit, 2-line mode. */
void InitLCD(void)
{
	WRITEBYTE(IODIR0, LCD_DATA, 0xFF);
	SETBIT(IODIR0, LCD_RS);
	SETBIT(IODIR0, LCD_RW);
	SETBIT(IODIR0, LCD_EN);

	delay_ms(15);                      /* wait for LCD power-on to stabilise */

	CmdLCD(0x30);  delay_ms(5);        /* function-set wake-up sequence, step 1 */
	CmdLCD(0x30);  delay_us(100);      /* step 2 */
	CmdLCD(0x30);                      /* step 3 */

	CmdLCD(MODE_8BIT_2LINE);           /* 8-bit interface, 2 lines, 5x8 font */
	CmdLCD(DSP_ON_CUR_OFF);            /* display on, cursor off, no blink   */
	CmdLCD(CLEAR_LCD);  delay_ms(2);
	CmdLCD(SHIFT_CUR_RIGHT);           /* entry mode: auto-increment cursor  */
}

/* Print a null-terminated string one character at a time. */
void StrLCD(u8 *str)
{
	while(*str)
		CharLCD(*str++);
}

/* Print an unsigned integer in decimal, without leading zeros. */
void U32LCD(u32 num)
{
	u8  buf[12];
	s32 i = 10;
	buf[11] = '\0';
	if(num == 0) { CharLCD('0'); return; }
	while(num > 0 && i >= 0)
	{
		buf[i--] = '0' + (num % 10);
		num /= 10;
	}
	StrLCD(&buf[i + 1]);
}
