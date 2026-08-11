# Bluetooth-Based Secure Locker with Dual-Factor Authentication

A locker/door-lock system on the **NXP LPC2148** (ARM7TDMI-S) that requires
two independent factors before it will open:

1. **Level 1 - Bluetooth**: a password sent from a phone (any HC-05 serial
   terminal / Bluetooth Serial app) over an HC-05 module.
2. **Level 2 - Keypad**: a second password typed on a 4x4 matrix keypad.

Every attempt - successful or not - is timestamped with the on-chip RTC and
streamed as a human-readable log over UART0 to a PC terminal. An admin
push-button opens a menu to update the RTC or change either password,
protected by the existing password before any change is accepted.

## Features

- Two-factor authentication (Bluetooth password + keypad password)
- Passwords stored in an external **AT24C256 EEPROM** (I2C0), so they
  survive power loss and MCU resets
- Live activity log over UART0 (9600-8N1) - every access attempt, timeout,
  and admin action is timestamped and logged
- On-chip **RTC** for timestamps, editable from the admin menu
- Admin menu (external interrupt button) to change either password or set
  the date/time, with old-password verification and confirm-on-new-password
- Timeouts everywhere a human input is expected, so the system always
  falls back to a safe standby state instead of hanging
- 16x2 LCD status display and buzzer feedback for granted/denied access

## Hardware

| Peripheral        | Pins                              | Notes                              |
|--------------------|------------------------------------|-------------------------------------|
| LCD (8-bit mode)   | DATA=P0.8-P0.15, RS=P0.16, RW=P0.18, EN=P0.17 | 16x2 HD44780-compatible |
| Keypad (4x4)       | Rows=P1.16-P1.19, Cols=P1.20-P1.23 | `1 2 3 A / 4 5 6 B / 7 8 9 C / * 0 # D` |
| UART0              | TXD=P0.0, RXD=P0.1                | PC activity log, 9600 baud          |
| UART1              | TXD=P0.8, RXD=P0.9                | HC-05 Bluetooth module, 9600 baud   |
| I2C0 / EEPROM      | SCL=P0.2, SDA=P0.3                | AT24C256 on an HW-669 breakout      |
| Motor driver L293D | IN1=P0.12, IN2=P0.13              | Drives the locker's DC motor        |
| Buzzer             | P0.11                             | Active HIGH                         |
| Admin button        | P0.14 (EINT1)                     | Falling edge, external 10K pull-up  |

EEPROM address map: `0x0000` = Level-1 (Bluetooth) password, `0x0010` =
Level-2 (keypad) password. Both default to `"1234"` / `"5678"` respectively
on first use (see `config.h`).

## Repository layout

```
firmware/         All firmware source - flat layout so the Keil project
                   (blutooth.uvproj) opens and builds without touching any
                   include paths.
  main.c           Application logic: authentication flow, admin menu
  lcd.c/.h         16x2 LCD driver (8-bit mode)
  kpm.c/.h         4x4 matrix keypad driver
  uart0.c/.h       UART0 - PC activity log
  uart1.c/.h       UART1 - interrupt-driven HC-05 Bluetooth RX
  i2c.c/.h         I2C0 master driver
  eeprom.c/.h      AT24C256 EEPROM read/write (built on i2c.c)
  rtc.c/.h         On-chip RTC wrapper + timestamp logging
  motor.c/.h       Locker motor (L293D) and buzzer
  eint.c/.h        Admin push-button external interrupt
  delay.c/.h       Software delays
  define.h         Bit/nibble/byte GPIO macros
  config.h         Tunable settings: EEPROM map, password rules, timings
  types.h          Project-wide short type aliases (u8, u32, ...)
  LPC21xx.h        Vendor (Keil) LPC21xx register header
  Startup.s        Vendor (Keil) startup/vector-table assembly
  blutooth.uvproj / blutooth.uvopt   Keil uVision project files
```

## Building

1. Open `firmware/blutooth.uvproj` in Keil uVision (MDK-ARM, ARM7 support).
2. Build (F7). Output `.hex` is written into the `firmware/` folder.
3. Flash with **Flash Magic** (or uVision's own ISP if configured) over the
   board's serial bootloader.

## Usage

1. Power on - the LCD shows a startup screen, then **"Send BT Password"**.
2. Pair the HC-05 with a phone and send the Level-1 password (default
   `1234`) followed by a newline, within 30 seconds.
3. On success, enter the Level-2 password (default `5678`) on the keypad
   within 10 seconds, then press `#`.
4. Both correct -> the locker unlocks for 5 seconds, then re-locks
   automatically.
5. Press the admin button at any time to open the admin menu:
   - **1) RTC** - set the current date/time
   - **2) PWDS** - change the Level-1 or Level-2 password (requires the
     current password)
   - **3) EXIT**

All of the above is mirrored, with timestamps, on the UART0 serial log.

## Notes on this version

This is a documentation/cleanup pass over a working build: every source
file has fresh, function-by-function comments explaining *why* the code
does what it does (not just a restatement of the line), stray debug
comments and dead code were removed, and the two duplicate type-alias
headers (`type.h` / `types.h`) were consolidated into one. No functional
logic was changed - the authentication flow, timeouts, and register
programming are byte-for-byte the same as the working build.
