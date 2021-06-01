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

#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include<string>

//General settings
// inline const unsigned int MAX_CHAR_COUNT = 10;
// inline const unsigned int BUFFER_SIZE = MAX_CHAR_COUNT + 1;
// inline const std::string STARTUP_PATH = "/home/pi/NFCDoorAccess_build_eth0/Startup";

// Database settings
// inline const std::string DATABASE_PATH = "/home/pi/NFCDoorAccess_src/DatabaseGateway/res/Database_11032021.db";

// Mailbox names
// inline const std::string ROUTER_MESSAGE_QUEUE = "RouterQueue";
// inline const std::string CONTROL_PANEL_QUEUE = "ControlPanelQueue";
// inline const std::string ROUTER_TO_DBG_QUEUE = "routerToDBG";
// inline const std::string HARDWARED_QUEUE = "Hardwared";
// inline const std::string DATABASE_GATEWAY_QUEUE = "databaseGateway";

// Watchdog names
// inline const std::string WATCHDOG_SERVER_NAME = "server.watchdog";
// inline const std::string KEYPAD_CLIENT_NAME = "keypad.watchdog";
// inline const std::string CARD_READER_CLIENT_NAME = "card_reader.watchdog";
// inline const std::string MAIN_APPLICATION_CLIENT_NAME = "main_application.watchdog";
// inline const std::string DATABASE_GATEWAY_CLIENT_NAME = "database_gateway.watchdog";

// Watchdog timeout settings
// inline const unsigned int WATCHDOG_SERVER_PERIOD_MS = 500;
// inline const unsigned int KEYPAD_TIMEOUT_MS = 100;
// inline const unsigned int CARD_READER_TIMEOUT_MS = 100;
// inline const unsigned int MAIN_APPLICATION_TIMEOUT_MS = 100;
// inline const unsigned int DATABASE_GATEWAY_TIMEOUT_MS = 100;


// Watchdog TTLs
// inline const unsigned int KEYPAD_BASE_TTL = 5;
// inline const unsigned int CARD_READER_BASE_TTL = 5;
// inline const unsigned int MAIN_APPLICATION_BASE_TTL = 5;
// inline const unsigned int DATABASE_GATEWAY_BASE_TTL = 5;

// Mailbox settings
// inline const unsigned int MAX_MESSAGES_IN_QUEUE = 5;
// inline const unsigned int MAX_MESSAGE_LENGTH = 200;
// inline const unsigned int MESSAGE_BUFFER_SIZE = MAX_MESSAGE_LENGTH + 1;
// inline const time_t       DEFAULT_MB_TIMEOUT_MS = 5;

// Startup Settings
// inline const std::string KEYPAD_MODULE_PATH = "keypadModule";
// inline const std::string ROUTER_MODULE_PATH = "routerModule";
// inline const std::string STATUS_MODULE_PATH = "controlPanel";
// inline const std::string DATABASE_GATEWAY_PATH = "databaseGateway";

// Keypad module 1 settings
// inline const std::string FIFO_PATH = "keypadFIFO";
// inline const std::string KEYPAD_ISTREAM_PATH = "/dev/input/event0";

// Control panel settings
// inline const std::string OUTPUT_ERROR_LOG_PATH = "error.log";
// inline const std::string OUTPUT_STATUS_LOG_PATH = "status.log";

// Kernel settings
// inline const std::string MAIN_ERROR_LOG_PATHNAME = "kernel.log";

// struct error_report_struct
// inline const unsigned int REPORT_HEADER_LENGTH = 20;
// inline const unsigned int REPORT_BODY_LENGTH = 50;
// inline const unsigned int ERROR_WRAPPER_SIZE = (REPORT_HEADER_LENGTH + 1 + REPORT_BODY_LENGTH + 1 + 2 * 4); // sizeof(int) = 4 !!!!!!! WATCH OUT FOR PADDING;
// inline const unsigned int REPORT_BUFFER_SIZE = REPORT_BODY_LENGTH + 1;


#endif

