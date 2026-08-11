/*
 * rtc.c
 * -----------------------------------------------------------------------
 * Thin wrapper around the LPC2148's on-chip RTC peripheral registers.
 * Used to timestamp every access attempt in the UART0 activity log and
 * to let the admin set the current date/time from the keypad.
 *
 * The LPC2148 RTC runs directly off the 32.768 kHz watch crystal, so no
 * PREINT/PREFRAC pre-scaler configuration is needed (unlike some other
 * LPC2000 parts that derive the RTC clock from the peripheral clock).
 * -----------------------------------------------------------------------
 */

#include <LPC21xx.h>
#include "types.h"
#include "rtc_defines.h"
#include "rtc.h"
#include "uart0.h"

/* Reset the tick counter, then enable the RTC from the 32 kHz crystal. */
void Init_RTC(void)
{
	CCR = 0;
	CCR = RTC_RS;             /* pulse the reset bit    */
	CCR = 0;
	CCR = RTC_EN | RTC_CLKSRC;
}

void SetRTCTime(s32 hour, s32 min, s32 sec)
{
	HOUR = hour;
	MIN  = min;
	SEC  = sec;
}

void SetRTCDate(s32 date, s32 month, s32 year)
{
	DOM   = date;
	MONTH = month;
	YEAR  = year;
}

void GetRTCTime(s32 *hour, s32 *min, s32 *sec)
{
	*hour = HOUR;
	*min  = MIN;
	*sec  = SEC;
}

void GetRTCDate(s32 *date, s32 *month, s32 *year)
{
	*date  = DOM;
	*month = MONTH;
	*year  = YEAR;
}

/* Print a value 0-99 as two zero-padded digits, e.g. 5 -> "05". */
static void tx2(u32 v)
{
	UART0_TX('0' + v / 10);
	UART0_TX('0' + v % 10);
}

/* Send the current date/time to UART0 as "DD/MM/YYYY HH:MM:SS\r\n".
 * Called before every log entry so each line in the access log carries
 * a timestamp. */
void RTC_LogTimestamp(void)
{
	s32 h,m,s,d,mo,y;
	GetRTCTime(&h, &m, &s);
	GetRTCDate(&d, &mo, &y);

	tx2(d);  UART0_TX('/');
	tx2(mo); UART0_TX('/');
	UART0_TX('0' +  y / 1000);
	UART0_TX('0' + (y % 1000) / 100);
	UART0_TX('0' + (y % 100)  / 10);
	UART0_TX('0' +  y % 10);
	UART0_TX(' ');
	tx2(h);  UART0_TX(':');
	tx2(m);  UART0_TX(':');
	tx2(s);
	UART0_TX('\r');
	UART0_TX('\n');
}
