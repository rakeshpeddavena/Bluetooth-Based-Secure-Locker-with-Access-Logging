/*
 * eeprom.h - AT24C256 EEPROM (HW-669 module) access via I2C0
 */
#ifndef EEPROM_H
#define EEPROM_H
#include "types.h"

void EE_Init(void);
void EE_WriteByte(u32 addr, u8 data);
u8   EE_ReadByte(u32 addr);
void EE_WriteStr(u32 addr, u8 *str);
void EE_ReadStr(u32 addr, u8 *buf, u8 max);

#endif /* EEPROM_H */
