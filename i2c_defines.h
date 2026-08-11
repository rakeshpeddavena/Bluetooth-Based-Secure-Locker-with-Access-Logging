/*
 * i2c_defines.h
 * -----------------------------------------------------------------------
 * Pin function selects, clock divider and I2CONSET/I2CONCLR bit positions
 * for the LPC2148 I2C0 peripheral (SCL0 = P0.2, SDA0 = P0.3).
 *
 * Clock math (100 kHz standard mode):
 *   FOSC  = 12 MHz (external crystal)
 *   CCLK  = FOSC * 5   = 60 MHz   (PLL multiplier = 5)
 *   PCLK  = CCLK / 4   = 15 MHz   (VPBDIV = 1/4)
 *   DIVIDER = PCLK / (I2C_SPEED * 2) = 15,000,000 / 200,000 = 75
 * -----------------------------------------------------------------------
 */

#include "types.h"

#define SCL_PIN_FUN  0x00000010    /* PINSEL0[5:4]=01 -> P0.2 = SCL0 */
#define SDA_PIN_FUN  0x00000040    /* PINSEL0[7:6]=01 -> P0.3 = SDA0 */

#define I2C_SPEED    100000        /* 100 kHz standard mode */
#define FOSC_I2C     12000000
#define CCLK_I2C     (FOSC_I2C * 5)
#define PCLK_I2C     (CCLK_I2C / 4)
#define DIVIDER      (PCLK_I2C / I2C_SPEED / 2)

/* I2CONSET bit positions */
#define AA_BIT       2   /* Assert Acknowledge   */
#define SI_BIT       3   /* I2C Interrupt flag   */
#define STO_BIT      4   /* STOP flag            */
#define STA_BIT      5   /* START flag           */
#define I2EN_BIT     6   /* I2C interface enable */

/* I2CONCLR bit positions (mirror of the SET bits, used to clear them) */
#define AAC_BIT      2
#define SIC_BIT      3
#define STAC_BIT     5
#define I2ENC_BIT    6
