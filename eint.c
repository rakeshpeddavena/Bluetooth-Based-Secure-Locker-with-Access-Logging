/*
 * eint.c
 * -----------------------------------------------------------------------
 * External interrupt handler for the admin push-button on P0.14.
 * The button pulls the pin to GND through a 10K external pull-up, so a
 * button press produces a falling edge, which we catch with EINT1
 * (VIC channel 15).
 *
 * The ISR only sets a flag (admin_flag); main() polls that flag and
 * enters the admin menu from the main loop, keeping the ISR itself
 * as short as possible.
 * -----------------------------------------------------------------------
 */

#include <LPC21xx.h>
#include "types.h"
#include "define.h"
#include "eint.h"

volatile u8 admin_flag = 0;

/* Route P0.14 to EINT1, arm it for a falling edge, and hook the vector
 * into VIC slot 1. */
void EINT1_Init(void)
{
	PINSEL0 |=  (1 << 29);    /* PINSEL0[29:28] = 10 -> EINT1 function on P0.14 */
	PINSEL0 &= ~(1 << 28);

	EXTMODE  |=  (1 << 1);    /* EINT1: edge sensitive (not level) */
	EXTPOLAR &= ~(1 << 1);    /* EINT1: falling edge (active-low button) */
	EXTINT    =  (1 << 1);    /* clear any stale pending flag */

	VICVectAddr1  = (u32)EINT1_ISR;
	VICVectCntl1  = 0x20 | 15;    /* VIC slot 1, channel 15 = EINT1 */
	VICIntEnable |= (1 << 15);
}

/* ISR: flag the button press for the main loop and acknowledge both
 * the EINT1 pending bit and the VIC. */
void EINT1_ISR(void) __irq
{
	admin_flag = 1;
	EXTINT      = (1 << 1);
	VICVectAddr = 0;
}
