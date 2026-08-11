/*
 * uart1.c
 * -----------------------------------------------------------------------
 * Interrupt-driven UART1 receive driver for the HC-05 Bluetooth module
 * (TXD1 = P0.8, RXD1 = P0.9, 9600-8N1). Incoming bytes are pushed into a
 * ring buffer by the UART1 ISR (VIC channel 7); UART1_RxStr() then pulls
 * a full line (the Bluetooth password) out of that buffer with an
 * inactivity timeout, so a dropped connection can't hang the locker.
 * -----------------------------------------------------------------------
 */

#include <LPC21xx.h>
#include "types.h"
#include "uart1.h"
#include "delay.h"

#define BUF_SZ   256
#define DIVISOR1 97        /* PCLK/(16*9600) = 15,000,000/153,600 ~= 97 */
#define DLAB_BIT 7
#define _8_BIT   3

/* Ring buffer filled by the ISR, drained by UART1_RX()/UART1_RxStr(). */
static volatile u8 rx_buf[BUF_SZ];
static volatile u8 wr = 0, rd = 0;

/* Route P0.8/P0.9 to TXD1/RXD1, program 9600-8N1, enable the RX FIFO
 * interrupt and register the ISR on VIC channel 7. */
void UART1_Init(void)
{
	PINSEL0 |= 0x00050000;         /* PINSEL0[19:16] -> P0.8=TXD1, P0.9=RXD1 */
	U1LCR = (1 << DLAB_BIT) | _8_BIT;   /* DLAB=1, 8N1 */
	U1DLL = DIVISOR1;
	U1DLM = (DIVISOR1 >> 8);
	U1LCR = _8_BIT;                /* DLAB=0 */
	U1FCR = 0x07;                  /* enable + reset the RX/TX FIFOs */
	U1IER = 0x01;                  /* enable "receive data available" interrupt */

	VICVectAddr0  = (u32)UART1_ISR;
	VICVectCntl0  = 0x20 | 7;      /* VIC slot 0, channel 7 = UART1 */
	VICIntEnable |= (1 << 7);
}

/* ISR: pull the received byte out of U1RBR and push it into the ring
 * buffer. If the buffer is full the byte is silently dropped (nxt==rd)
 * rather than overwriting unread data. */
void UART1_ISR(void) __irq
{
	u8 ch  = U1RBR;
	u8 nxt = (wr + 1) % BUF_SZ;
	if(nxt != rd) { rx_buf[wr] = ch; wr = nxt; }
	VICVectAddr = 0;               /* acknowledge the VIC */
}

/* True if the ring buffer has at least one unread byte. */
u8 UART1_Available(void)
{
	return (wr != rd) ? 1 : 0;
}

/* Pop and return the oldest byte from the ring buffer.
 * Caller must check UART1_Available() first. */
u8 UART1_RX(void)
{
	u8 ch = rx_buf[rd];
	rd = (rd + 1) % BUF_SZ;
	return ch;
}

/*
 * UART1_RxStr - collect one line (the Bluetooth password) into buf.
 *
 * Reads bytes until a '\r' or '\n' terminator, stripping a leading
 * stray '\n' (some terminals send \r\n and the \r is consumed first).
 * If more than (max-1) characters arrive before a terminator, the
 * input is treated as invalid: remaining bytes up to the next line
 * terminator (or a 2-second idle gap) are drained and discarded, and
 * the function returns 0 with an empty buffer. If no data arrives at
 * all for 2 seconds while characters are already pending, the partial
 * line is accepted as-is.
 *
 * Returns 1 with buf holding the null-terminated line, or 0 if the
 * input was rejected as oversized.
 */
u8 UART1_RxStr(u8 *buf, u8 max)
{
	u8 i = 0, ch;
	u32 idle = 0;

	while(1)
	{
		if(UART1_Available())
		{
			ch = UART1_RX();
			idle = 0;

			if(ch == '\n' && i == 0)         /* ignore a leading stray LF */
				continue;

			if(ch == '\r' || ch == '\n')     /* end of line reached */
				break;

			if(i < (max - 1))
			{
				buf[i++] = ch;                /* accumulate into caller's buffer */
			}
			else
			{
				/* Buffer would overflow: discard this and all further bytes
				 * until the line terminator shows up, or the sender goes
				 * quiet for 2 seconds. */
				while(1)
				{
					if(UART1_Available())
					{
						ch = UART1_RX();
						if(ch == '\r' || ch == '\n')
							break;
						idle = 0;   /* NOTE: unreachable after break above, kept as-is from the original logic */
					}
					else
					{
						delay_ms(10);
						idle += 10;
						if(idle >= 2000)
							break;
					}
				}

				buf[0] = '\0';
				return 0;                     /* input rejected: too long */
			}
		}
		else
		{
			/* No byte waiting right now. */
			if(i > 0)
			{
				/* We already have some characters: treat a 2 s gap as
				 * "sender finished without sending a terminator". */
				delay_ms(10);
				idle += 10;
				if(idle >= 2000)
					break;
			}
			else
			{
				/* Nothing received yet at all: keep waiting (the caller
				 * enforces its own overall timeout around this call). */
				delay_ms(10);
			}
		}
	}

	buf[i] = '\0';
	return 1;
}
