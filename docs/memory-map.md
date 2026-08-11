\# Memory \& Peripheral Map



\## 1. Overview



This document records the memory and peripheral resources used by the Bluetooth-Based Secure Locker firmware.



The project uses:



\- LPC2148 GPIO Port 0

\- LPC2148 GPIO Port 1

\- UART0

\- UART1

\- I²C0

\- On-chip RTC

\- EINT1

\- External AT24C256 EEPROM

\- 16×2 LCD

\- 4×4 matrix keypad

\- L293D motor driver

\- Buzzer



> This document reflects the pin mappings and addresses currently defined in the project source.



\---



\# 2. Peripheral Allocation



```mermaid

flowchart TB



&#x20;   MCU\["LPC2148"]



&#x20;   P0\["GPIO

