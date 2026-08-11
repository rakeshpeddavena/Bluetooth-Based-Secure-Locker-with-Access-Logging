/*
 * eeprom.c
 * -----------------------------------------------------------------------
 * Byte and string read/write helpers for the AT24C256 EEPROM (HW-669
 * breakout module) used to persist the two locker passwords across
 * power cycles.
 *
 * Module wiring : VCC, GND, SCL -> P0.2, SDA -> P0.3 (pull-ups already
 *                 present on the module PCB, none needed on the board).
 * Device address: A0/A1/A2 tied to GND -> 0xA0 (write), 0xA1 (read).
 * -----------------------------------------------------------------------
 */

#include <LPC21xx.h>
#include "types.h"
#include "i2c.h"
#include "delay.h"
#include "eeprom.h"

#define EE_WR   0xA0    /* device address + write bit */
#define EE_RD   0xA1    /* device address + read bit  */

/* Bring up the underlying I2C0 bus. */
void EE_Init(void)
{
	Init_i2c();
}

/* Write a single byte to a 16-bit EEPROM address (page-write of 1 byte). */
void EE_WriteByte(u32 addr, u8 data)
{
	i2c_start();
	i2c_write(EE_WR);
	i2c_write((addr >> 8) & 0xFF);     /* address high byte */
	i2c_write(addr & 0xFF);            /* address low byte  */
	i2c_write(data);
	i2c_stop();
	delay_ms(6);                       /* AT24C256 internal write cycle, max 5 ms */
}

/* Random-read a single byte: set the address pointer, then repeated
 * START into read mode and clock out one byte with a NACK. */
u8 EE_ReadByte(u32 addr)
{
	u8 val;
	i2c_start();
	i2c_write(EE_WR);
	i2c_write((addr >> 8) & 0xFF);
	i2c_write(addr & 0xFF);
	i2c_restart();
	i2c_write(EE_RD);
	val = i2c_read_nack();             /* single byte read, so NACK immediately */
	i2c_stop();
	return val;
}

/* Write a null-terminated string starting at addr, including the
 * terminator, so EE_ReadStr can later detect the end of the string. */
void EE_WriteStr(u32 addr, u8 *str)
{
	while(*str)
		EE_WriteByte(addr++, *str++);
	EE_WriteByte(addr, '\0');
}

/* Read back a string, stopping at a null terminator, an erased byte
 * (0xFF, meaning "never written"), or the caller-supplied buffer limit. */
void EE_ReadStr(u32 addr, u8 *buf, u8 max)
{
	u8 i = 0, ch;
	while(i < max - 1)
	{
		ch = EE_ReadByte(addr++);
		if(ch == '\0' || ch == 0xFF) break;
		buf[i++] = ch;
	}
	buf[i] = '\0';
}
