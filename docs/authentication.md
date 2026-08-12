

\## Authentication Flow



!\[Authentication Flow](images/architecture/authentication-flow.png)



The authentication flow accepts credentials through the supported user-input paths, compares them with the stored credential, and then follows either the access-granted or access-denied path.





\# Authentication Architecture



\## 1. Overview



The locker implements two credential-based access paths:



\- \*\*Level-1 authentication:\*\* Bluetooth password through the HC-05 module and UART1.

\- \*\*Level-2 authentication:\*\* Local keypad password through the 4×4 matrix keypad.



The credentials are stored in the AT24C256 EEPROM and are read by the firmware during authentication.



```mermaid

flowchart TB



&#x20;   USER\["User"]



&#x20;   BT\["Bluetooth Device"]

&#x20;   HC05\["HC-05"]

&#x20;   UART1\["UART1"]



&#x20;   KP\["4×4 Keypad"]

&#x20;   KPM\["Keypad Driver"]



&#x20;   AUTH\["Authentication Logic"]



&#x20;   EEPROM\["AT24C256<br/>Credential Storage"]



&#x20;   VERIFY{"Credential<br/>Match?"}



&#x20;   GRANT\["Access Granted"]

&#x20;   DENY\["Access Denied"]



&#x20;   MOTOR\["L293D + DC Motor"]

&#x20;   BUZZER\["Buzzer"]

&#x20;   LOG\["RTC + UART0<br/>Activity Log"]



&#x20;   USER --> BT

&#x20;   BT --> HC05

&#x20;   HC05 --> UART1

&#x20;   UART1 --> AUTH



&#x20;   USER --> KP

&#x20;   KP --> KPM

&#x20;   KPM --> AUTH



&#x20;   EEPROM --> AUTH

&#x20;   AUTH --> VERIFY



&#x20;   VERIFY -->|YES| GRANT

&#x20;   VERIFY -->|NO| DENY



&#x20;   GRANT --> MOTOR

&#x20;   GRANT --> BUZZER

&#x20;   GRANT --> LOG



&#x20;   DENY --> BUZZER

&#x20;   DENY --> LOG

