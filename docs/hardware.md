\# Hardware Architecture



\## 1. Overview



The Bluetooth-Based Secure Locker is built around an LPC2148 ARM7TDMI-S microcontroller.



The controller interfaces with multiple peripherals to provide:



\- Bluetooth-based authentication

\- Keypad-based authentication

\- Password storage

\- LCD user interaction

\- Motorized locker control

\- Audible feedback

\- Administrator access

\- Real-time timestamping

\- PC-based activity logging



The firmware uses GPIO, UART, I2C, the on-chip RTC, and an external interrupt.



\---



\## 2. Main Hardware Components



| Component | Purpose |

|---|---|

| LPC2148 | Main embedded controller |

| HC-05 Bluetooth module | Wireless Level-1 authentication interface |

| 16×2 LCD | User interface and status display |

| 4×4 matrix keypad | Local password entry and administrator input |

| AT24C256 EEPROM | Persistent password storage |

| L293D | DC motor driver / H-bridge |

| DC motor | Locker locking/unlocking mechanism |

| Buzzer | Authentication feedback |

| Push button | Administrator-mode trigger |

| PC serial terminal | Activity-log monitoring |

| 32.768 kHz RTC crystal | Real-time clock source |



\---



\## 3. LPC2148 Pin Mapping



The following mapping is based on the GPIO and peripheral definitions used by the current firmware.



\### UART0 — PC Activity Logging



| Signal | LPC2148 Pin | Purpose |

|---|---|---|

| TXD0 | P0.0 | Serial transmit |

| RXD0 | P0.1 | Serial receive |



UART0 operates at 9600 baud and is used by the firmware to send human-readable activity logs to a PC terminal.



\---



\### I2C0 — AT24C256 EEPROM



| Signal | LPC2148 Pin | Purpose |

|---|---|---|

| SCL0 | P0.2 | I2C clock |

| SDA0 | P0.3 | I2C data |



The firmware configures I2C0 for 100 kHz operation.



The EEPROM is used to persist the Level-1 and Level-2 passwords across power cycles.



\---



\### LCD — 16×2 HD44780-Compatible Display



The LCD operates in 8-bit parallel mode.



| LCD Signal | LPC2148 Pin |

|---|---|

| D0 | P0.16 |

| D1 | P0.17 |

| D2 | P0.18 |

| D3 | P0.19 |

| D4 | P0.20 |

| D5 | P0.21 |

| D6 | P0.22 |

| D7 | P0.23 |

| RS | P0.4 |

| EN | P0.5 |

| RW | P0.10 |



The LCD is used for prompts, system status, authentication feedback, and masked password entry.



> \*\*Implementation note:\*\* The LCD data/control mapping above reflects the active definitions used by `lcd.c` and `lcd\_defines.h`. Some older comments in the source describe different pin numbers; the firmware definitions are treated as the current implementation reference.



\---



\### HC-05 Bluetooth — UART1



| Signal | LPC2148 Pin |

|---|---|

| TXD1 | P0.8 |

| RXD1 | P0.9 |



UART1 is configured for:



```text

9600 baud

8 data bits

No parity

1 stop bit

