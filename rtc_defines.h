/*
 * rtc_defines.h - on-chip RTC Clock Control Register (CCR) bits
 */
#define RTC_RS      (1<<1)    /* reset the internal clock tick counter */
#define RTC_EN      (1<<0)    /* enable the RTC                        */
#define RTC_CLKSRC  (1<<4)    /* use the 32.768 kHz crystal directly (LPC2148) */
