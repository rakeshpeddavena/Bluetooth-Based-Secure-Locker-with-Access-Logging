\# Communication Architecture



\## 1. Overview



The locker uses multiple communication interfaces, each assigned to a specific subsystem.



```mermaid

flowchart TB



&#x20;   MCU\["LPC2148"]



&#x20;   UART0\["UART0<br/>P0.0 / P0.1"]

&#x20;   UART1\["UART1<br/>P0.8 / P0.9"]

&#x20;   I2C\["I²C0<br/>P0.2 / P0.3"]



&#x20;   PC\["PC Serial Terminal"]

&#x20;   HC05\["HC-05 Bluetooth Module"]

&#x20;   EEPROM\["AT24C256 EEPROM"]



&#x20;   MCU --> UART0

&#x20;   MCU --> UART1

&#x20;   MCU --> I2C



&#x20;   UART0 <--> PC

&#x20;   UART1 <--> HC05

&#x20;   I2C <--> EEPROM

