/*
 * delay.c
 * -----------------------------------------------------------------------
 * Simple busy-wait (software) delays for the LPC2148 running at 60 MHz
 * core clock with PCLK = 15 MHz (VPBDIV = 1/4).
 *
 * These loop counts were tuned empirically for this clock configuration.
 * They are NOT cycle-accurate (compiler optimisation level affects them),
 * but are good enough for LCD timing, debouncing and short pauses.
 * -----------------------------------------------------------------------
 */

#include "types.h"

/* Busy-wait for approximately dlyus microseconds */
void delay_us(u32 dlyus)
{
	for(dlyus *= 12; dlyus > 0; dlyus--);
}

/* Busy-wait for approximately dlyms milliseconds */
void delay_ms(u32 dlyms)
{
	for(dlyms *= 12000; dlyms > 0; dlyms--);
}
