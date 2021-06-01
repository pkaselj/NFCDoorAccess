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

#ifndef KEYPAD_PROTO_HPP
#define KEYPAD_PROTO_HPP

#include<string>

#include"pipe.hpp"
#include"NulLogger.hpp"
#include"propertiesclass.h"



#define CHARACTER_MAP_OFFSET 69
char characterMap[] = {
    'N', // KEY_NUMLOCK
    'S', // KEY_SCROLLLOCK
    '7', // KEY_KP7
    '8', // KEY_KP8
    '9', // KEY_KP9
    '-', // KEY_KPMINUS
    '4', // KEY_KP4
    '5', // KEY_KP5
    '6', // KEY_KP6
    '+', // KEY_KPPLUS
    '1', // KEY_KP1
    '2', // KEY_KP2
    '3', // KEY_KP3
    '0', // KEY_KP0
    '.'  // KEY_KPDOT
    };




/* FROM DOCUMENTATION
struct input_event {
	struct timeval time;
	unsigned short type;
	unsigned short code;
	unsigned int value;
};

  'time' is the timestamp, it returns the time at which the event happened.
Type is for example EV_REL for relative moment, EV_KEY for a keypress or
release. More types are defined in include/uapi/linux/input-event-codes.h.

  'code' is event code, for example REL_X or KEY_BACKSPACE, again a complete
list is in include/uapi/linux/input-event-codes.h.

  'value' is the value the event carries. Either a relative change for
EV_REL, absolute new value for EV_ABS (joysticks ...), or 0 for EV_KEY for
release, 1 for keypress and 2 for autorepeat.
*/


typedef struct input_event IEVENT;

class Keypad
{
private:
// const static uint        m_buffer_size   = BUFFER_SIZE;
    const static uint        m_buffer_size;
             std::string m_buffer                     ;
             Pipe*       m_pOutputPipe                ;
             ILogger*    m_pLogger       = NULL       ;
             FILE*       keypadFile      = NULL       ;
             int         m_keypadFile_fd = -1         ;
             IEVENT*     input_event     = NULL       ;

    /// Appends `x` to the internal buffer
    inline void appendChar         (char x);

    /// Clears the internal buffer
    inline void clearBuffer        (void);

    /// Flushes (sends) the internal buffer to the output pipe
    inline void flushBuffer        (void);

           /// Decodes char from the last `input_event` and appends it to the internal buffer
           void decodeChar         (void);

           /// If buffer size reaches max specified size, flushes it to the output
           void checkBufferOverflow(void);

           /// Sends ENTER or BACKSPACE strings
           void sendSpecialChar(char character);
public:
    /**
     * @brief Creates an object which represents numeric keypad
     * @param keypadPath Path to the keyapad file
     * @param pOutputPipe Pointer to a pipe to which keypad input is sent after pressing enter
     * @param p_logger Pointer to a logger
    */
    Keypad(const std::string& keypadPath, Pipe* pOutputPipe, ILogger* p_logger = NulLogger::getInstance());
    ~Keypad(void);
    void readChar(void);
};

#endif