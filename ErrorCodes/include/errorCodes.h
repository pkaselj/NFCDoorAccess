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

//test line
#ifndef ERROR_CODES_H
#define ERROR_CODES_H

// GPIO ERRORS
#define MAP_ERROR -1
#define WRONG_PIN -2
#define OFFSET_ERROR -3
#define NULLPTRERROR -4

// PIPE AND FILE ERRORS
#define MKFIFO_ERROR -10
#define PIPE_NOT_SET -11
#define ISTREAM_ERROR -12
#define MALLOC_ERROR -13
#define IEVENT_MALLOC_ERROR MALLOC_ERROR
#define IEVENT_NULLPTR -14
#define WRITE_ERROR -15
#define READ_ERROR -16
#define INVALID_FD -17

// MESSAGE QUEUE ERRORS
#define MESSAGE_RECIEVE_ERROR -20
#define INVALID_SV_KEY -21
#define INVALID_POINTER -22
#define CANNOT_OPEN_MESSAGE_QUEUE -23
#define MESSAGE_SEND_ERROR -24
#define INVALID_MESSAGE_QUEUE_ID -25
#define INVALID_MESSAGE_QUEUE_NAME -26
#define MESSAGE_TOO_LONG -27
#define UNKNOWN_TYPE -28 

// STARTUP ERRORS
#define TOO_FEW_ARGUMENTS -30

#endif
