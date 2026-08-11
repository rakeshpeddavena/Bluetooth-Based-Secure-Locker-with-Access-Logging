/*
 * uart0_defines.h
 * -----------------------------------------------------------------------
 * Pin functions and baud-rate divisor for UART0, which is used purely
 * as a debug/activity log output to a PC terminal (TXD0 = P0.0, RXD0 = P0.1).
 *
 * Baud math (9600 @ PCLK = 15 MHz):
 *   DIVISOR = PCLK / (16 * BAUD) = 15,000,000 / 153,600 ~= 97
 * -----------------------------------------------------------------------
 */

#define TXD0_PIN_FUN   0x00000001    /* PINSEL0[1:0]=01 -> P0.0 = TXD0 */
#define RXD0_PIN_FUN   0x00000004    /* PINSEL0[3:2]=01 -> P0.1 = RXD0 */

#define BAUD           9600
#define FOSC           12000000
#define CCLK           (FOSC * 5)
#define PCLK           (CCLK / 4)
#define DIVISOR        (PCLK / (16 * BAUD))

#define _8_BIT         3    /* U0LCR word-length select: 8 data bits */
#define WORD_LEN_SEL   0    /* bit offset of the word-length field in U0LCR */
#define DLAB_BIT       7    /* Divisor Latch Access Bit in U0LCR */
#define DR_BIT         0    /* Data Ready bit in U0LSR */
#define TEMT_BIT       6    /* Transmitter Empty bit in U0LSR */
