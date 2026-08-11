&#x20;  # System Architecture



\## 1. System Overview



The Bluetooth-Based Secure Locker is an LPC2148-based embedded access-control system.



The system provides two authentication interfaces:



\- \*\*Bluetooth authentication\*\* through an HC-05 module

\- \*\*Local keypad authentication\*\* through a 4×4 matrix keypad



The LPC2148 coordinates authentication, credential storage, user feedback, locker motor control, administrator access, RTC timestamping, and activity logging.



\---



\## 2. High-Level System Architecture



```mermaid

flowchart TB



&#x20;   USER\["👤 User"]



&#x20;   BT\["📱 Bluetooth Device"]

&#x20;   HC05\["HC-05 Bluetooth Module"]

&#x20;   UART1\["UART1<br/>9600-8N1"]



&#x20;   KEYPAD\["4×4 Matrix Keypad"]

&#x20;   LCD\["16×2 LCD"]



&#x20;   MCU\["LPC2148<br/>ARM7TDMI-S<br/><br/>Main Application"]



&#x20;   EEPROM\["AT24C256 EEPROM<br/>Credential Storage"]

&#x20;   I2C\["I²C0<br/>100 kHz"]



&#x20;   RTC\["On-Chip RTC<br/>32.768 kHz"]



&#x20;   MOTOR\["L293D<br/>Motor Driver"]

&#x20;   LOCK\["DC Motor<br/>Locker Mechanism"]

&#x20;   BUZZER\["Buzzer"]



&#x20;   ADMIN\["Admin Push Button<br/>EINT1"]



&#x20;   UART0\["UART0<br/>9600-8N1"]

&#x20;   PC\["PC Terminal<br/>Activity Log"]



&#x20;   USER --> BT

&#x20;   BT --> HC05

&#x20;   HC05 --> UART1

&#x20;   UART1 --> MCU



&#x20;   USER --> KEYPAD

&#x20;   KEYPAD --> MCU



&#x20;   MCU --> LCD



&#x20;   MCU --> I2C

&#x20;   I2C --> EEPROM



&#x20;   MCU --> RTC



&#x20;   MCU --> MOTOR

&#x20;   MOTOR --> LOCK



&#x20;   MCU --> BUZZER



&#x20;   ADMIN --> MCU



&#x20;   MCU --> UART0

&#x20;   UART0 --> PC

