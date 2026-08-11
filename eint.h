/*
 * eint.h - EINT1 admin push-button on P0.14
 */
#ifndef EINT_H
#define EINT_H
#include "types.h"

extern volatile u8 admin_flag;   /* set to 1 by the ISR when admin button is pressed */

void EINT1_Init(void);
void EINT1_ISR(void) __irq;

#endif /* EINT_H */
