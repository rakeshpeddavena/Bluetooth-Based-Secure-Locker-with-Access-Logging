/*
 * types.h
 * -----------------------------------------------------------------------
 * Short, fixed-width type aliases used throughout the project instead of
 * the verbose standard names (unsigned char, unsigned int, etc.).
 * Included by almost every module: delay, i2c, eeprom, rtc, uart0/1, kpm.
 * -----------------------------------------------------------------------
 */

typedef unsigned int   u32;   /* 32-bit unsigned  - addresses, timers, ms counts */
typedef int            s32;   /* 32-bit signed    - RTC hour/min/sec/date fields */
typedef unsigned short  u16;   /* 16-bit unsigned  - not heavily used, kept for completeness */
typedef unsigned char   u8;    /* 8-bit unsigned   - bytes, ASCII chars, register values */
typedef char             s8;    /* 8-bit signed     - kept for completeness */
typedef const char       cs8;   /* read-only string literal pointer type */
typedef const short int  cu8;   /* kept for completeness (unused in this project) */
typedef float             f32;   /* kept for completeness (unused in this project) */
