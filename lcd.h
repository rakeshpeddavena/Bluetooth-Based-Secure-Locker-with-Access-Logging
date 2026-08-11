/*
 * lcd.h - 16x2 character LCD driver (8-bit mode)
 */
#ifndef LCD_H
#define LCD_H
#include "types.h"

void InitLCD(void);
void CmdLCD(u8 cmd);
void CharLCD(u8 ch);
void StrLCD(u8 *str);
void U32LCD(u32 num);

#endif /* LCD_H */
