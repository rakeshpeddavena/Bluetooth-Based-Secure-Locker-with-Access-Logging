\# Testing \& Validation



\## 1. Testing Overview



The Bluetooth-Based Secure Locker is composed of multiple hardware and firmware subsystems.



Testing is therefore divided into:



\- Unit-level peripheral checks

\- Interface testing

\- Authentication testing

\- Hardware integration testing

\- Negative testing

\- End-to-end system testing



The overall approach is:



```mermaid

flowchart TD



&#x20;   SOURCE\["Firmware Source"]



&#x20;   BUILD\["Build Verification"]



&#x20;   PERIPHERAL\["Peripheral Testing"]



&#x20;   INTERFACE\["Communication Testing"]



&#x20;   AUTH\["Authentication Testing"]



&#x20;   INTEGRATION\["System Integration"]



&#x20;   NEGATIVE\["Negative Testing"]



&#x20;   E2E\["End-to-End Testing"]



&#x20;   RESULT\["Validation Result"]



&#x20;   SOURCE --> BUILD

&#x20;   BUILD --> PERIPHERAL

&#x20;   PERIPHERAL --> INTERFACE

&#x20;   INTERFACE --> AUTH

&#x20;   AUTH --> INTEGRATION

&#x20;   INTEGRATION --> NEGATIVE

&#x20;   NEGATIVE --> E2E

&#x20;   E2E --> RESULT

