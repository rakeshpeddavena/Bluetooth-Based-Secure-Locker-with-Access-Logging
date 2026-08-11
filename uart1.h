/*
 * uart1.h - UART1, interrupt-driven RX for the HC-05 Bluetooth module
 */
#ifndef UART1_H
#define UART1_H
#include "types.h"

void UART1_Init(void);
u8   UART1_Available(void);
u8   UART1_RX(void);
u8   UART1_RxStr(u8 *buf, u8 max);   /* collect one line, terminated by '\r' or '\n' */
void UART1_ISR(void) __irq;

#endif /* UART1_H */
