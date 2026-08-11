/*
 * uart0.c
 * -----------------------------------------------------------------------
 * Polled UART0 driver used purely to stream a human-readable activity
 * log (access attempts, admin actions, RTC updates) to a PC terminal
 * at 9600-8N1. This channel is separate from UART1, which talks to the
 * HC-05 Bluetooth module.
 * -----------------------------------------------------------------------
 */

#include <LPC21xx.h>
#include "types.h"
#include "uart0.h"
#include "uart0_defines.h"

/* Route P0.0/P0.1 to TXD0/RXD0 and program the baud-rate divisor for
 * 9600-8N1. */
void UART0_Init(void)
{
	PINSEL0 &= ~((3<<0)|(3<<2));
	PINSEL0 |= TXD0_PIN_FUN | RXD0_PIN_FUN;

	U0LCR = (1 << DLAB_BIT) | (_8_BIT << WORD_LEN_SEL);  /* DLAB=1 to access divisor latches, 8N1 */
	U0DLL = DIVISOR;
	U0DLM = (DIVISOR >> 8);
	U0LCR &= ~(1 << DLAB_BIT);                           /* DLAB=0, back to normal operation */
}

/* Blocking transmit of a single byte. */
void UART0_TX(u8 sdat)
{
	U0THR = sdat;
	while(((U0LSR >> TEMT_BIT) & 1) == 0);   /* wait until the transmit shift register is empty */
}

/* Blocking receive of a single byte. */
u8 UART0_RX(void)
{
	while(((U0LSR >> DR_BIT) & 1) == 0);     /* wait until a byte has arrived */
	return U0RBR;
}

/* Transmit a null-terminated string. */
void UART0_Str(u8 *str)
{
	while(*str)
		UART0_TX(*str++);
}

/* Transmit a string followed by CR/LF - used for every log line so
 * each entry lands on its own line in the terminal. */
void UART0_Log(u8 *msg)
{
	UART0_Str(msg);
	UART0_TX('\r');
	UART0_TX('\n');
}

/* Transmit an unsigned integer in decimal, without leading zeros. */
void UART0_TxNum(u32 num)
{
	u8  buf[12];
	s32 i = 10;
	buf[11] = '\0';
	if(num == 0) { UART0_TX('0'); return; }
	while(num > 0 && i >= 0)
	{
		buf[i--] = '0' + (num % 10);
		num /= 10;
	}
	UART0_Str(&buf[i + 1]);
}
