\# Software Architecture



\## 1. Firmware Overview



The firmware is organized as a modular embedded application for the LPC2148.



The application layer coordinates authentication, administrator operations, locker control, password management, RTC handling, and activity logging.



Hardware-specific functionality is separated into individual peripheral drivers.



```mermaid

flowchart TB



&#x20;   MAIN\["main.c<br/>Application Controller"]



&#x20;   CONFIG\["config.h<br/>Configuration"]



&#x20;   LCD\["LCD Driver<br/>lcd.c / lcd.h"]

&#x20;   KPM\["Keypad Driver<br/>kpm.c / kpm.h"]

&#x20;   UART0\["UART0 Driver<br/>uart0.c / uart0.h"]

&#x20;   UART1\["UART1 Driver<br/>uart1.c / uart1.h"]

&#x20;   EEPROM\["EEPROM Driver<br/>eeprom.c / eeprom.h"]

&#x20;   I2C\["I²C Driver<br/>i2c.c / i2c.h"]

&#x20;   RTC\["RTC Driver<br/>rtc.c / rtc.h"]

&#x20;   MOTOR\["Motor Driver<br/>motor.c / motor.h"]

&#x20;   EINT\["External Interrupt<br/>eint.c / eint.h"]



&#x20;   MAIN --> CONFIG



&#x20;   MAIN --> LCD

&#x20;   MAIN --> KPM

&#x20;   MAIN --> UART0

&#x20;   MAIN --> UART1

&#x20;   MAIN --> EEPROM

&#x20;   MAIN --> RTC

&#x20;   MAIN --> MOTOR

&#x20;   MAIN --> EINT



&#x20;   EEPROM --> I2C









flowchart TD



&#x20;   RESET(\["Power On / Reset"])



&#x20;   INIT\["Peripheral Initialization"]



&#x20;   IDLE\["Main Application Loop"]



&#x20;   BT\["Bluetooth Authentication"]

&#x20;   KP\["Keypad Authentication"]

&#x20;   ADMIN\["Administrator Operations"]



&#x20;   VERIFY\["Credential Verification"]



&#x20;   OPEN\["Locker Open"]

&#x20;   DENIED\["Access Denied"]



&#x20;   LOG\["Activity Logging"]



&#x20;   RESET --> INIT

&#x20;   INIT --> IDLE



&#x20;   IDLE --> BT

&#x20;   IDLE --> KP

&#x20;   IDLE --> ADMIN



&#x20;   BT --> VERIFY

&#x20;   KP --> VERIFY



&#x20;   VERIFY -->|Valid| OPEN

&#x20;   VERIFY -->|Invalid| DENIED



&#x20;   OPEN --> LOG

&#x20;   DENIED --> LOG



&#x20;   ADMIN --> LOG



&#x20;   LOG --> IDLE

















