# NFCDoorAccess_CMake

### Authors
- Petar Kaselj (@ emovis tehnologije d.o.o.)

### Contact info:
- petar.kaselj.00@gmail.com
- petar.kaselj@emovis.hr
---

### Motivation

This project was developed during early stages of my internship at _emovis tehnologije d.o.o._ in coordination with
_FESB (Faculty of Electrical Engineering, Mechanical Engineering and Naval Architecture)_ at _University of Split_ as
part of my bachelor's thesis.

---

### Description

Access Control System used to control electronic lock via _Raspberry Pi_'s GPIO pins, based on authentication using USB keypad (to enter passcode) or RFID/NFC reader (for contactless authorization). The solution uses 16x2 LCD and piezo buzzer to provide audio-visual feedback to the user.

The system was written in C, C++ and C++ Qt framework using CMake as its build system. It runs on _Raspberry Pi 3B+_.

---

### Supported Features:
- PIN/Contactless(NFC/RFID) user authorization
- Multiple authorization Levels:
    - Non-Employee
        - Cannot gain access to the object
    - Guest
        - Either all guests can or cannot access the object at the given time (global switch)
    - Employee (Up to 127 hierarchial authorization sub-levels)
        - Can access the object unless specifically demoted to a _Non-Employee_
        - Multiple sub-levels can execute different commands (ADD, REMOVE, SET other users)
- Commands that can be executed using numeric keypad _on-site_
    - ADD/REMOVE USER/CARD/PIN
- Each user can have multiple assigned PIN codes and/or contactless cards
    - Pins start with asterisk '\*' e.g. '*2204'
    - Commands start with forward-slash '/' e.g. '/2204'
        - The 16x2 LCD and pizeo buzzer provide user with audio-visual feedback (during multi-step commands)
- Can be _Fail Safe_ or _Fail Secure_ depending on configuration settings and wiring

---

### Performance
-   0 memory leaks after 4+ hrs of continued use
-   0 latency after 4+ hrs of use
-   0 crashes during 4+ hrs of use

---

### External Links
- [Documentation](https://github.com/pkaselj/NFCDoorAccessCMakeDocumentation)
- [Doxygen Documentation](https://nfcdooraccess-docs.netlify.app)

Written as a part of my bachelor's thesis.

---

### License

```
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    NFCDoorAccess is written by Petar Kaselj as an employee of
    Emovis tehnologije d.o.o. which allowed its release under
    this license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
  
 ```
  
---

### Dependencies
- Raspberry Pi 3B+
- CMake
- PiGPIO library
- Qt C++ framework [LGPL](https://www.qt.io/faq/tag/qt-open-source-licensing)
- libnfc [LGPL](http://nfc-tools.org/index.php/Libnfc:License)
