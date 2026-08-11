/*
 * uart0.h - UART0, used as the PC-side activity/debug log (9600-8N1)
 */
#ifndef UART0_H
#define UART0_H
#include "types.h"

void UART0_Init(void);
void UART0_TX(u8 ch);
u8   UART0_RX(void);
void UART0_Str(u8 *str);
void UART0_Log(u8 *msg);       /* string + CRLF, one line of the access log */
void UART0_TxNum(u32 num);     /* decimal number, no leading zeros */

#endif /* UART0_H */
