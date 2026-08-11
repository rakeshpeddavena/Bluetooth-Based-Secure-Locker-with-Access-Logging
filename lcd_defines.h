/*
 * lcd_defines.h
 * -----------------------------------------------------------------------
 * Pin mapping and HD44780 command bytes for the 16x2 character LCD
 * wired in 8-bit mode:
 *   DATA (D0-D7) : P0.8 - P0.15
 *   RS           : P0.16
 *   RW           : P0.18
 *   EN           : P0.17
 * -----------------------------------------------------------------------
 */

#define LCD_DATA     16     /* data bus starts at bit 8 -> P0.8 (WRITEBYTE start bit) */
#define LCD_RS       4      /* offset within the byte written by WRITEBYTE: P0.16     */
#define LCD_RW       10     /* P0.18 */
#define LCD_EN       5      /* P0.17 */

/* HD44780 command bytes */
#define CLEAR_LCD          0x01
#define RET_CUR_HOME       0x02
#define SHIFT_CUR_RIGHT    0x06
#define DSP_OFF            0x08
#define DSP_ON_CUR_OFF     0x0C
#define DSP_ON_CUR_ON      0x0E
#define DSP_ON_CUR_BLINK   0x0F
#define MODE_8BIT_2LINE    0x38
#define GOTO_LINE1_POS0    0x80
#define GOTO_LINE2_POS0    0xC0
#define GOTO_CGRAM_START   0x40
