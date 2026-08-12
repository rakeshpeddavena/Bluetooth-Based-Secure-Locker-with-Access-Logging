\# 🔐 Bluetooth-Based Secure Locker with Access Logging



A microcontroller-based electronic locker system built around the \*\*NXP LPC2148\*\*, providing two authentication methods — \*\*Bluetooth and keypad\*\* — with persistent credential storage, RTC-based timestamping, administrator access, motorized locking, buzzer feedback, and UART activity logging.



\---



\## 📌 Project Overview



The system combines embedded firmware, hardware interfaces, authentication logic, and physical access control into a single locker platform.



```mermaid

flowchart TB



\&#x20;   USER\\\["User"]



\&#x20;   BT\\\["HC-05<br/>Bluetooth"]

\&#x20;   KP\\\["4×4 Matrix<br/>Keypad"]



\&#x20;   UART1\\\["UART1"]

\&#x20;   KPM\\\["Keypad Driver"]



\&#x20;   AUTH\\\["Authentication<br/>Logic"]



\&#x20;   EEPROM\\\["AT24C256<br/>EEPROM"]



\&#x20;   VERIFY{"Credential<br/>Valid?"}



\&#x20;   MOTOR\\\["L293D<br/>Motor Driver"]

\&#x20;   LOCKER\\\["🔒 Locker"]



\&#x20;   LCD\\\["16×2 LCD"]

\&#x20;   BUZZER\\\["Buzzer"]



\&#x20;   RTC\\\["LPC2148<br/>RTC"]



\&#x20;   UART0\\\["UART0"]

\&#x20;   PC\\\["PC Terminal<br/>Activity Log"]



\&#x20;   ADMIN\\\["Admin Button<br/>EINT1"]



\&#x20;   USER --> BT

\&#x20;   USER --> KP



\&#x20;   BT --> UART1

\&#x20;   UART1 --> AUTH



\&#x20;   KP --> KPM

\&#x20;   KPM --> AUTH



\&#x20;   EEPROM --> AUTH

\&#x20;   AUTH --> VERIFY



\&#x20;   VERIFY -->|YES| MOTOR

\&#x20;   VERIFY -->|NO| BUZZER



\&#x20;   MOTOR --> LOCKER

\&#x20;   VERIFY -->|YES| BUZZER



\&#x20;   AUTH --> RTC

\&#x20;   RTC --> UART0

\&#x20;   UART0 --> PC



\&#x20;   ADMIN --> AUTH

\&#x20;   AUTH --> LCD

```



\---



\# ✨ Features



\- 🔵 Bluetooth-based authentication using HC-05

\- 🔢 4×4 matrix keypad authentication

\- 💾 Persistent credential storage using AT24C256 EEPROM

\- 🕒 LPC2148 hardware RTC for timestamping

\- 📟 16×2 LCD user interface

\- 🔐 Masked keypad password entry

\- 🔔 Buzzer feedback

\- ⚙️ L293D-based DC motor control

\- 👤 Administrator button using EINT1

\- 📝 UART0 activity logging

\- 📡 Interrupt-driven UART1 reception

\- 🔄 UART1 ring buffer

\- ⏱️ Bluetooth inactivity timeout

\- 🛡️ Oversized Bluetooth input rejection

\- 🔌 I²C communication with external EEPROM

\- 🧩 Modular embedded-C driver architecture



\---



\# 🧠 System Architecture

!\[System Architecture](docs/images/architecture/system-architecture.png)



The firmware is organized into independent peripheral drivers and application logic.



```mermaid

flowchart TB



\&#x20;   APP\\\["Application Layer<br/>main.c"]



\&#x20;   AUTH\\\["Authentication"]

\&#x20;   ADMIN\\\["Administrator Functions"]

\&#x20;   ACCESS\\\["Access Control"]



\&#x20;   LCD\\\["LCD Driver"]

\&#x20;   KPM\\\["Keypad Driver"]

\&#x20;   UART0\\\["UART0 Driver"]

\&#x20;   UART1\\\["UART1 Driver"]

\&#x20;   EEPROM\\\["EEPROM Driver"]

\&#x20;   I2C\\\["I²C Driver"]

\&#x20;   RTC\\\["RTC Driver"]

\&#x20;   EINT\\\["EINT1 Driver"]

\&#x20;   MOTOR\\\["Motor / Buzzer Driver"]



\&#x20;   APP --> AUTH

\&#x20;   APP --> ADMIN

\&#x20;   APP --> ACCESS



\&#x20;   AUTH --> LCD

\&#x20;   AUTH --> KPM

\&#x20;   AUTH --> UART1

\&#x20;   AUTH --> EEPROM



\&#x20;   ADMIN --> KPM

\&#x20;   ADMIN --> EEPROM

\&#x20;   ADMIN --> RTC

\&#x20;   ADMIN --> EINT



\&#x20;   ACCESS --> MOTOR



\&#x20;   EEPROM --> I2C



\&#x20;   APP --> UART0

\&#x20;   APP --> RTC

```



\---



\# 🔐 Authentication Flow



!\[Authentication Flow](docs/images/architecture/authentication-flow.png)



Two independent user-input paths are supported.



```mermaid

flowchart TD



\&#x20;   START(\\\["Authentication Request"])



\&#x20;   METHOD{"Authentication<br/>Method"}



\&#x20;   BT\\\["Bluetooth"]

\&#x20;   KEYPAD\\\["Keypad"]



\&#x20;   UART\\\["UART1"]

\&#x20;   KPM\\\["Keypad Scanner"]



\&#x20;   BUFFER\\\["Input Buffer"]



\&#x20;   EEPROM\\\["Read Stored Credential"]



\&#x20;   COMPARE{"Credential<br/>Matches?"}



\&#x20;   GRANT\\\["Access Granted"]

\&#x20;   DENY\\\["Access Denied"]



\&#x20;   MOTOR\\\["Open Locker"]

\&#x20;   BUZZER1\\\["Buzzer Feedback"]

\&#x20;   LOG1\\\["Timestamped Log"]



\&#x20;   START --> METHOD



\&#x20;   METHOD --> BT

\&#x20;   METHOD --> KEYPAD



\&#x20;   BT --> UART

\&#x20;   UART --> BUFFER



\&#x20;   KEYPAD --> KPM

\&#x20;   KPM --> BUFFER



\&#x20;   BUFFER --> EEPROM

\&#x20;   EEPROM --> COMPARE



\&#x20;   COMPARE -->|YES| GRANT

\&#x20;   COMPARE -->|NO| DENY



\&#x20;   GRANT --> MOTOR

\&#x20;   GRANT --> BUZZER1

\&#x20;   GRANT --> LOG1



\&#x20;   DENY --> BUZZER1

\&#x20;   DENY --> LOG1

```



\---



\# 📡 Communication Architecture



The project uses separate communication channels for different purposes.



| Interface | Device | Purpose |

|--- |---|---|

| UART0 | PC Terminal | Activity/debug logging |

| UART1 | HC-05 | Bluetooth authentication |

| I²C0 | AT24C256 | Credential storage |

| GPIO | LCD | User interface |

| GPIO | Keypad | Local authentication |

| EINT1 | Admin Button | Administrator trigger |



```mermaid

flowchart LR



\&#x20;   HC05\\\["HC-05"]

\&#x20;   UART1\\\["UART1"]



\&#x20;   EEPROM\\\["AT24C256"]

\&#x20;   I2C\\\["I²C0"]



\&#x20;   PC\\\["PC"]

\&#x20;   UART0\\\["UART0"]



\&#x20;   HC05 --> UART1

\&#x20;   UART1 --> MCU\\\["LPC2148"]



\&#x20;   MCU --> I2C

\&#x20;   I2C --> EEPROM



\&#x20;   MCU --> UART0

\&#x20;   UART0 --> PC

```



\---



\# 🔌 Hardware Architecture



!\[Hardware Block Diagram](docs/images/hardware/hardware-block-diagram.png)





```mermaid

flowchart TB



\&#x20;   MCU\\\["LPC2148"]



\&#x20;   LCD\\\["16×2 LCD"]

\&#x20;   KEYPAD\\\["4×4 Keypad"]

\&#x20;   HC05\\\["HC-05"]

\&#x20;   EEPROM\\\["AT24C256"]

\&#x20;   RTC\\\["Internal RTC"]

\&#x20;   ADMIN\\\["Admin Button"]

\&#x20;   L293D\\\["L293D"]

\&#x20;   MOTOR\\\["DC Motor"]

\&#x20;   BUZZER\\\["Buzzer"]

\&#x20;   PC\\\["PC Terminal"]



\&#x20;   MCU --> LCD

\&#x20;   MCU --> KEYPAD

\&#x20;   MCU --> HC05

\&#x20;   MCU --> EEPROM

\&#x20;   MCU --> RTC

\&#x20;   MCU --> ADMIN

\&#x20;   MCU --> L293D

\&#x20;   L293D --> MOTOR

\&#x20;   MCU --> BUZZER

\&#x20;   MCU --> PC

```



\---



\# 📍 GPIO / Peripheral Mapping



\## Port 0



| Pin | Function | Purpose |

|---|---|---|

| P0.0 | TXD0 | UART0 transmit |

| P0.1 | RXD0 | UART0 receive |

| P0.2 | SCL0 | I²C clock |

| P0.3 | SDA0 | I²C data |

| P0.8 | TXD1 | UART1 / HC-05 |

| P0.9 | RXD1 | UART1 / HC-05 |

| P0.11 | GPIO | Buzzer |

| P0.12 | GPIO | Motor IN1 |

| P0.13 | GPIO | Motor IN2 |

| P0.14 | EINT1 | Administrator button |

| P0.16–P0.23 | GPIO | LCD interface |



\## Port 1



| Pin | Function | Purpose |

|---|---|---|

| P1.16 | GPIO | Keypad Row 0 |

| P1.17 | GPIO | Keypad Row 1 |

| P1.18 | GPIO | Keypad Row 2 |

| P1.19 | GPIO | Keypad Row 3 |

| P1.20 | GPIO | Keypad Column 0 |

| P1.21 | GPIO | Keypad Column 1 |

| P1.22 | GPIO | Keypad Column 2 |

| P1.23 | GPIO | Keypad Column 3 |



> The GPIO mapping above follows the current project source. Hardware wiring should be verified against the actual board before deployment.



\---



\## 🖥️ LCD Interface



The 16×2 LCD is connected to the LPC2148 using an 8-bit parallel data interface.



| LCD Signal | LPC2148 Pin |

|---|---|

| D0–D7 | P0.16–P0.23 |

| RS | P0.4 |

| RW | P0.10 |

| EN | P0.5 |



The LCD driver provides:



\- Command transmission

\- Character transmission

\- String display

\- Decimal number display

\- Cursor positioning

\- Display initialization

\- Password masking



The actual GPIO positions are derived from `lcd\\\_defines.h` and the `WRITEBYTE()` macro in `define.h`.



> Verify the exact LCD control-pin mapping against `lcd\\\_defines.h` before physical wiring. The source comments contain historical mapping descriptions that should not be treated as authoritative over the actual `#define` values.



\---



\# 🔢 Keypad Interface



The 4×4 keypad is scanned using four output rows and four input columns.



```text

\&#x20;      C0 C1 C2 C3

\&#x20;      ↓  ↓  ↓  ↓



R0 →   1  2  3  A

R1 →   4  5  6  B

R2 →   7  8  9  C

R3 →   \\\*  0  #  D

```



Special keys:



```text

\\# → ENTER / CONFIRM

\\\* → BACKSPACE

```



The keypad driver performs:



1\. Press detection

2\. Debouncing

3\. Row detection

4\. Column detection

5\. Key lookup

6\. Release detection



\---



\# 💾 EEPROM Memory Map



The external AT24C256 stores the locker credentials.



```text

AT24C256

│

├── 0x0000

│   └── Level-1 Bluetooth credential

│

├── Reserved space

│

└── 0x0010

\&#x20;   └── Level-2 Keypad credential

```



The firmware uses 16-bit EEPROM addresses.



\---



\# 🔔 Motor \& Buzzer Control



The locker motor is controlled through an L293D H-bridge.



```mermaid

flowchart LR



\&#x20;   LPC\\\["LPC2148"]



\&#x20;   IN1\\\["P0.12<br/>IN1"]

\&#x20;   IN2\\\["P0.13<br/>IN2"]



\&#x20;   DRIVER\\\["L293D"]



\&#x20;   MOTOR\\\["DC Motor"]

\&#x20;   BUZZER\\\["P0.11<br/>Buzzer"]



\&#x20;   LPC --> IN1

\&#x20;   LPC --> IN2



\&#x20;   IN1 --> DRIVER

\&#x20;   IN2 --> DRIVER



\&#x20;   DRIVER --> MOTOR



\&#x20;   LPC --> BUZZER

```



Current motor control:



```text

IN1 = 1, IN2 = 0 → Open

IN1 = 0, IN2 = 1 → Close

IN1 = 0, IN2 = 0 → Stop

```



The actual mechanical direction must be verified on the physical locker.



\---



\# 🕒 Access Logging



The LPC2148 RTC provides date/time information.



UART0 sends human-readable activity information to a PC terminal.



```text

LPC2148

\&#x20;  │

\&#x20;  ├── RTC

\&#x20;  │     │

\&#x20;  │     └── Timestamp

\&#x20;  │

\&#x20;  └── UART0

\&#x20;        │

\&#x20;        ↓

\&#x20;     PC Terminal

```



Timestamp format:



```text

DD/MM/YYYY HH:MM:SS

```



\---



\# ⚙️ Firmware Modules



| Source | Responsibility |

|---|---|

| `main.c` | Application logic |

| `lcd.c` | LCD driver |

| `kpm.c` | Matrix keypad driver |

| `uart0.c` | PC logging UART |

| `uart1.c` | Bluetooth UART |

| `i2c.c` | I²C0 master driver |

| `eeprom.c` | AT24C256 access |

| `rtc.c` | RTC interface |

| `eint.c` | Administrator interrupt |

| `motor.c` | Motor and buzzer control |

| `delay.c` | Timing utilities |



\---



\# 🛡️ Security Model



The current implementation provides several security-related mechanisms:



\- Dual authentication paths

\- Credential separation

\- Password masking on keypad input

\- Bluetooth input size validation

\- Bluetooth inactivity handling

\- Administrator-triggered workflow

\- Timestamped activity logging

\- Controlled motor actuation



However, the current implementation \*\*does not provide cryptographic password hashing or encrypted credential storage\*\*.



Credentials are currently handled as strings and stored in the external EEPROM.



Therefore, this project should be considered an \*\*embedded access-control prototype\*\*, rather than a production-grade cryptographic security system.



See:



\- \[`Security Architecture`](docs/security.md)

\- \[`Authentication`](docs/authentication.md)



for details.



\---



\# 🧪 Testing



The repository includes a structured testing document covering:



\- Build verification

\- LCD testing

\- Keypad testing

\- UART testing

\- Bluetooth testing

\- EEPROM persistence

\- RTC validation

\- Administrator interrupt

\- Motor operation

\- Buzzer operation

\- Authentication

\- Negative testing

\- End-to-end testing



See:



\*\*\[`Testing \\\& Validation`](docs/testing.md)\*\*



\---



\# 🔨 Build \& Flash



The firmware project is designed for \*\*Keil µVision\*\* and the LPC2148.



Basic workflow:



```text

Open Keil Project

\&#x20;      ↓

Build Target

\&#x20;      ↓

Resolve Errors / Warnings

\&#x20;      ↓

Generate Firmware

\&#x20;      ↓

Program LPC2148

\&#x20;      ↓

Power / Reset

\&#x20;      ↓

Verify Hardware

```



Detailed instructions:



\*\*\[`Build \\\& Flash Guide`](docs/build-and-flash.md)\*\*



\---



\# 📚 Documentation



| Document | Description |

|---|---|

| \[`Architecture`](docs/architecture.md) | Overall system architecture |

| \[`Hardware`](docs/hardware.md) | Hardware components and connections |

| \[`Software`](docs/software.md) | Firmware architecture |

| \[`Communication`](docs/communication.md) | UART and I²C communication |

| \[`Authentication`](docs/authentication.md) | Authentication workflows |

| \[`Memory Map`](docs/memory-map.md) | EEPROM and peripheral mapping |

| \[`Security`](docs/security.md) | Security controls and limitations |

| \[`Testing`](docs/testing.md) | Test strategy and validation |

| \[`Build \\\& Flash`](docs/build-and-flash.md) | Firmware build/programming procedure |



\---



\# 📁 Repository Structure



```text

Bluetooth-Based-Secure-Locker-with-Access-Logging/

│

├── main.c

│

├── lcd.c

├── lcd.h

├── lcd\\\_defines.h

│

├── kpm.c

├── kpm.h

├── kpm\\\_defines.h

│

├── uart0.c

├── uart0.h

├── uart0\\\_defines.h

│

├── uart1.c

├── uart1.h

│

├── i2c.c

├── i2c.h

├── i2c\\\_defines.h

│

├── eeprom.c

├── eeprom.h

│

├── rtc.c

├── rtc.h

├── rtc\\\_defines.h

│

├── eint.c

├── eint.h

│

├── motor.c

├── motor.h

├── motor\\\_defines.h

│

├── delay.c

├── delay.h

│

├── config.h

├── define.h

├── types.h

├── LPC21xx.h

│

├── Startup.s

│

├── blutooth.uvproj

├── blutooth.uvopt

│

├── docs/

│   ├── architecture.md

│   ├── authentication.md

│   ├── build-and-flash.md

│   ├── communication.md

│   ├── hardware.md

│   ├── memory-map.md

│   ├── security.md

│   ├── software.md

│   └── testing.md

│

└── README.md

```



\---



\# 🚀 Future Improvements



Possible next-generation improvements include:



\- Password hashing instead of plaintext storage

\- Stronger administrator authentication

\- Bluetooth authentication with cryptographic protection

\- Authentication rate limiting

\- Temporary account lockout

\- Tamper detection

\- Protected audit records

\- Better credential-management workflow

\- Motor position/limit sensing

\- Door-open/door-closed sensing

\- Watchdog-based fault recovery

\- Improved hardware abstraction

\- Automated firmware testing

\- Hardware-in-the-loop testing



\---



\# ⚠️ Important Project Notes



This repository represents an embedded-system project and should be tested against the actual hardware configuration.



Before deployment:



1\. Verify all GPIO connections.

2\. Verify motor direction.

3\. Verify buzzer wiring.

4\. Verify UART levels and interface hardware.

5\. Verify EEPROM addressing.

6\. Verify RTC hardware configuration.

7\. Verify LCD pin mapping.

8\. Test invalid authentication scenarios.

9\. Test behavior after power cycling.

10\. Do not use default credentials in a production deployment.



\---



\# 👨‍💻 Project



\*\*Bluetooth-Based Secure Locker with Access Logging\*\*



Built using:



```text

C

Embedded C

LPC2148

Keil µVision

UART

I²C

GPIO

RTC

HC-05

AT24C256

L293D

```



The project demonstrates the integration of \*\*embedded firmware, hardware interfacing, authentication, persistent storage, communication protocols, interrupt handling, and physical access control\*\*.



\---



\## 📜 License



Add a license appropriate for your project before distributing the repository publicly.

