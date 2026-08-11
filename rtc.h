/*
 * rtc.h - LPC2148 on-chip real-time clock driver
 */
#ifndef RTC_H
#define RTC_H
#include "types.h"

void Init_RTC(void);
void SetRTCTime(s32 hour, s32 min, s32 sec);
void SetRTCDate(s32 date, s32 month, s32 year);
void GetRTCTime(s32 *hour, s32 *min, s32 *sec);
void GetRTCDate(s32 *date, s32 *month, s32 *year);
void RTC_LogTimestamp(void);   /* sends "DD/MM/YYYY HH:MM:SS\r\n" over UART0 */

#endif /* RTC_H */
