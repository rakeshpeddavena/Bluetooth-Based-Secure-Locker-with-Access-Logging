/*
 * config.h
 * -----------------------------------------------------------------------
 * Single place to tune the locker's behaviour without touching main.c:
 *   - where each password lives in the AT24C256 EEPROM
 *   - default (first-boot) passwords
 *   - how long the locker motor stays open
 *   - buzzer beep pattern
 * -----------------------------------------------------------------------
 */

#ifndef CONFIG_H
#define CONFIG_H

/* ---- EEPROM address map -------------------------------------------- */
#define L1_ADDR               0x0000   /* Level-1 (Bluetooth) password  */
#define L2_ADDR               0x0010   /* Level-2 (keypad) password     */

/* ---- Password rules --------------------------------------------------
 * PASSWORD_LENGTH includes the null terminator, so a 4-digit PIN needs 5. */
#define PASSWORD_LENGTH       5
#define DEFAULT_BT_PASSWORD   "1234"
#define DEFAULT_KP_PASSWORD   "5678"

/* ---- Locker motor timing (milliseconds) ------------------------------ */
#define LOCKER_OPEN_TIME_MS   5000     /* how long the locker stays unlocked */
#define MOTOR_RUN_TIME_MS     3000     /* reserved for future motor tuning   */

/* ---- Buzzer feedback --------------------------------------------------- */
#define BUZZER_BEEP_COUNT     5
#define BUZZER_ON_TIME        200
#define BUZZER_OFF_TIME       200

#endif /* CONFIG_H */
