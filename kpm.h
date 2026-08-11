/*
 * kpm.h - 4x4 matrix keypad driver
 */
#ifndef KPM_H
#define KPM_H
#include "types.h"

void InitKPM(void);

u8 Colscan(void);                          /* 0 = a key is currently pressed, 1 = none */
u8 KeyScan(void);                          /* blocking: wait for a key, return its ASCII value */
u8 KeyScanTimeout(u32 timeout_ms);         /* like KeyScan but gives up after timeout_ms, returns 0 */

void GetPwd(u8 *buf, u8 max);              /* collect digits until '#'; '*' = backspace */
u8   GetDigit(void);
u8   GetPwdTimeout(u8 *buf, u8 max, u32 timeout_ms);  /* GetPwd with an overall timeout */
void GetNumber(u8 *buf, u8 max);           /* like GetPwd but digits only, echoed on LCD */

#endif /* KPM_H */
