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

#ifndef ERROR_TEMPLATES_H
#define ERROR_TEMPLATES_H

// GPIO ERRORS
#define T_MAP_ERROR "Cannot map memory", MAP_ERROR
#define T_WRONG_PIN "Invalid pin", WRONG_PIN
#define T_OFFSET_ERROR "Function set register offset error", OFFSET_ERROR
#define T_NULLPTRERROR "Pointer can't be NULL", NULLPTRERROR

// PIPE AND FILE ERRORS
#define T_MKFIFO_ERROR "Cannot create a FIFO", MKFIFO_ERROR
#define T_PIPE_NOT_SET "Trying to access pipe with nonexisting fd ", PIPE_NOT_SET
#define T_ISTREAM_ERROR "Invalid input stream", ISTREAM_ERROR
#define T_MALLOC_ERROR "Cannot allocate memory", MALLOC_ERROR
#define T_IEVENT_MALLOC_ERROR "Cannot allocate memory for ievent", IEVENT_MALLOC_ERROR
#define T_IEVENT_NULLPTR "Ievent pointer cannot be NULL", IEVENT_NULLPTR
#define T_WRITE_ERROR "Cannot write to specified pipe", WRITE_ERROR
#define T_READ_ERROR "Cannot read from a specified pipe", READ_ERROR
#define T_INVALID_FD "Invalid file descriptor", INVALID_FD

// MESSAGE QUEUE ERRORS
#define T_MESSAGE_RECIEVE_ERROR "Error while trying to receive a message", MESSAGE_RECIEVE_ERROR
#define T_INVALID_SV_KEY "Invalid System V key", INVALID_SV_KEY
#define T_INVALID_POINTER "Invalid pointer", INVALID_POINTER // ???
#define T_CANNOT_OPEN_MESSAGE_QUEUE "Cannot open specified message queue", CANNOT_OPEN_MESSAGE_QUEUE
#define T_MESSAGE_SEND_ERROR "Error while trying to send a message", MESSAGE_SEND_ERROR
#define T_INVALID_MESSAGE_QUEUE_ID "Invalid message queue ID", INVALID_MESSAGE_QUEUE_ID
#define T_INVALID_MESSAGE_QUEUE_NAME "Invalid MQueue name", INVALID_MESSAGE_QUEUE_NAME
#define T_MESSAGE_TOO_LONG "Message is too long", MESSAGE_TOO_LONG
#define T_UNKNOWN_TYPE "Unknown message type", UNKNOWN_TYPE
// STARTUP ERRORS
#define T_TOO_FEW_ARGUMENTS "Too few arguments", -30

#endif
