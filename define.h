/*
 * define.h
 * -----------------------------------------------------------------------
 * Generic bit / nibble / byte manipulation macros used to read and write
 * groups of pins on a GPIO port register (IOPIN0/1, IODIR0/1, ...).
 * These keep the driver code (lcd.c, kpm.c, motor.c) free of repeated
 * shift-and-mask expressions.
 * -----------------------------------------------------------------------
 */

#include "types.h"

/* Set a single bit at BITPOS to 1 */
#define SETBIT(WORD,BITPOS)               (WORD |= ((u32)1 << BITPOS))

/* Clear a single bit at BITPOS to 0 */
#define CLRBIT(WORD,BITPOS)               (WORD &= (~((u32)1 << BITPOS)))

/* Read a single bit at BITPOS (returns 0 or 1) */
#define READBIT(WORD,BITPOS)              ((WORD >> BITPOS) & 1)

/* Write an 8-bit value VAL into WORD starting at bit STARTBITPOS
 * (used for the 8-bit LCD data bus on P0.8-P0.15)                     */
#define WRITEBYTE(WORD,STARTBITPOS,VAL)   (WORD = ((WORD & ~(0xFF << STARTBITPOS)) | ((VAL) << STARTBITPOS)))

/* Write a 4-bit nibble VAL into WORD starting at bit STARTBITPOS
 * (used for the keypad row pins P1.16-P1.19)                          */
#define WRITENIBBLE(WORD,STARTBITPOS,VAL) (WORD = ((WORD & ~(0x0F << STARTBITPOS)) | ((VAL & 0x0F) << STARTBITPOS)))

/* Read a 4-bit nibble from WORD starting at bit STARTPOS */
#define READNIBBLE(WORD,STARTPOS)         ((WORD >> STARTPOS) & 0x0F)

/* Read an 8-bit byte from WORD starting at bit STARTPOS */
#define READBYTE(WORD,STARTPOS)           ((WORD >> STARTPOS) & 0xFF)
