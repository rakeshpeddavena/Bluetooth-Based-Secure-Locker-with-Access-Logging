\# Security Architecture



\## 1. Security Overview



The Bluetooth-Based Secure Locker implements multiple layers of access control around the physical locker mechanism.



The current design combines:



\- Bluetooth-based authentication

\- Local keypad authentication

\- Non-volatile credential storage

\- Administrator access

\- Input validation

\- Bluetooth receive buffering

\- Input timeout handling

\- Password masking on the LCD

\- Activity logging

\- RTC timestamps

\- Controlled motor actuation



```mermaid

flowchart TB



&#x20;   USER\["User"]



&#x20;   BT\["Bluetooth Authentication"]

&#x20;   KP\["Keypad Authentication"]



&#x20;   VALIDATE\["Input Validation"]



&#x20;   EEPROM\["AT24C256<br/>Credential Storage"]



&#x20;   VERIFY\["Credential Verification"]



&#x20;   ACCESS{"Authentication<br/>Result"}



&#x20;   MOTOR\["Locker Motor"]



&#x20;   BUZZER\["Buzzer"]



&#x20;   RTC\["RTC Timestamp"]



&#x20;   LOG\["UART0 Activity Log"]



&#x20;   ADMIN\["Administrator Interface"]



&#x20;   USER --> BT

&#x20;   USER --> KP



&#x20;   BT --> VALIDATE

&#x20;   KP --> VALIDATE



&#x20;   EEPROM --> VERIFY

&#x20;   VALIDATE --> VERIFY



&#x20;   VERIFY --> ACCESS



&#x20;   ACCESS -->|Valid| MOTOR

&#x20;   ACCESS -->|Valid| BUZZER



&#x20;   ACCESS -->|Invalid| BUZZER



&#x20;   ACCESS --> RTC

&#x20;   RTC --> LOG



&#x20;   ADMIN --> VERIFY

