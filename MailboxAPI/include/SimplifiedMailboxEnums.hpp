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


