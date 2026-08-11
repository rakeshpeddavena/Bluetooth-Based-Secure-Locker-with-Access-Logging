/*
 * i2c.c
 * -----------------------------------------------------------------------
 * Polled (non-interrupt) master-mode driver for the LPC2148's I2C0
 * peripheral. Used by eeprom.c to talk to the AT24C256 EEPROM that stores
 * the two locker passwords.
 *
 * Each function below maps directly onto one step of the I2C protocol:
 * start / restart / write byte / stop / read byte (ack or nack).
 * -----------------------------------------------------------------------
 */

#include <LPC21xx.h>
#include "types.h"
#include "i2c_defines.h"
#include "i2c.h"

/* Configure P0.2/P0.3 as SCL0/SDA0, set the 100 kHz clock divider and
 * enable the I2C0 interface. */
void Init_i2c(void)
{
	PINSEL0 |= SCL_PIN_FUN | SDA_PIN_FUN;
	I2SCLL   = DIVIDER;
	I2SCLH   = DIVIDER;
	I2CONSET = 1 << I2EN_BIT;
}

/* Generate a START condition and wait for the interrupt (SI) flag,
 * which signals the bus is ready for the next byte. */
void i2c_start(void)
{
	I2CONSET = 1 << STA_BIT;
	while(((I2CONSET >> SI_BIT) & 1) == 0);
	I2CONCLR = 1 << STAC_BIT;                /* clear STA, keep bus active */
}

/* Generate a repeated START (used between the write-address phase and
 * the read phase of an EEPROM random read). */
void i2c_restart(void)
{
	I2CONSET = 1 << STA_BIT;
	I2CONCLR = 1 << SIC_BIT;
	while(((I2CONSET >> SI_BIT) & 1) == 0);
	I2CONCLR = 1 << STAC_BIT;
}

/* Send one byte (address or data) on the bus and wait for it to
 * complete. */
void i2c_write(u8 data)
{
	I2DAT    = data;
	I2CONCLR = 1 << SIC_BIT;                 /* clear SI to start the transfer */
	while(((I2CONSET >> SI_BIT) & 1) == 0);
}

/* Generate a STOP condition, releasing the bus. */
void i2c_stop(void)
{
	I2CONSET = 1 << STO_BIT;
	I2CONCLR = 1 << SIC_BIT;
}

/* Read one byte and ACK it (tells the slave more bytes are wanted). */
u8 i2c_read_ack(void)
{
	I2CONSET = 1 << AA_BIT;
	I2CONCLR = 1 << SIC_BIT;
	while(((I2CONSET >> SI_BIT) & 1) == 0);
	return I2DAT;
}

/* Read one byte and NACK it (tells the slave this is the last byte). */
u8 i2c_read_nack(void)
{
	I2CONCLR = 1 << AAC_BIT;
	I2CONCLR = 1 << SIC_BIT;
	while(((I2CONSET >> SI_BIT) & 1) == 0);
	return I2DAT;
}
