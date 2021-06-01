/*
*	 Copyright (C) Petar Kaselj 2021
*
*	 This file is part of NFCDoorAccess.
*
*	 NFCDoorAccess is written by Petar Kaselj as an employee of
*	 Emovis tehnologije d.o.o. which allowed its release under
*	 this license.
*
*    NFCDoorAccess is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NFCDoorAccess is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NFCDoorAccess.  If not, see <https://www.gnu.org/licenses/>.
*
*/

#ifndef SIMPLIFIED_MAILBOX_ENUMS_HPP
#define SIMPLIFIED_MAILBOX_ENUMS_HPP

#include <string>

typedef enum : char
{
    EMPTY,
    RTS,
    CTS,
    HOLD,
    MESSAGE,
    TIMED_OUT,
    ACK,
    MESSAGE_CONNECTIONLESS,
    ERROR,
    SYSCALL_INTERRUPTED
} enuMessageType;

std::string toString(enuMessageType type)
{
    switch (type)
    {
    case EMPTY:
        return "EMPTY";

    case RTS:
        return"RTS";

    case CTS:
        return"CTS";

    case HOLD:
        return"HOLD";

    case MESSAGE:
        return"MESSAGE";

    case TIMED_OUT:
        return"TIMED_OUT";

    case ACK:
        return"ACK";

    case MESSAGE_CONNECTIONLESS:
        return "MESSAGE_CONNECTIONLESS";

    case SYSCALL_INTERRUPTED:
        return "SYSCALL_INTERRUPTED";

    default:
        return "INVALID TYPE";
    }
}

typedef enum : unsigned char
{
    NORMAL = 1, // BLOCKING
    TIMED = 2,
    NONBLOCKING = 4,
    CONNECTIONLESS = 8, // UNUSED SINCE v2
    IGNORE_QUEUE = 16,
    OVERRIDE_NORMAL = (unsigned char) 1, // Bit mask to set last (LSB) bit to 1 (logical OR)
    DONT_OVERRIDE = (unsigned char) 0 // All ones for logical OR
} enuReceiveOptions;

typedef enuReceiveOptions enuSendOptions; // alias

inline enuReceiveOptions operator&(enuReceiveOptions opt1, enuReceiveOptions opt2)
{
    return static_cast<enuReceiveOptions>((unsigned char)opt1 & (unsigned char)opt2);
}

inline enuReceiveOptions operator|(enuReceiveOptions opt1, enuReceiveOptions opt2)
{
    return static_cast<enuReceiveOptions>((unsigned char)opt1 | (unsigned char)opt2);
}

inline bool operator%(enuReceiveOptions opt1, enuReceiveOptions opt2)
{
    return (opt1 & opt2) != 0;
}

#endif


