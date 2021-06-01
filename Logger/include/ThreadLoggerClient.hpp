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

#ifndef THREAD_LOGGER_CLIENT_HPP
#define THREAD_LOGGER_CLIENT_HPP

#include"Logger.hpp"
#include"mailbox.hpp"

/**
 * @brief Client side of cross-threaded Logger
 *
 * ThreadLoggerClient acts as a Logger to any class using it, \n
 * but internally it sends all log messages to matching ThreadLoggerServer object, \n
 * via a mailbox. \n
 * ThreadLoggerServer then receives log messages from ThreadLoggerClient and \n
 * forwards them to a Logger which handles them apporpriately. \n
 * \n
 * In essesnce ThreadLoggerClient and ThreadLoggerServer pair \n
 * act as a bridge between a class which logs data \n
 * and a Logger executing concurrently in a separate thread. \n
 * \n
 * When execution speed is critical, then instead of Logger class, \n
 * ThreadLogger class can be implemented. \n
 * \n
 * ThreadLoggerClient acts as a Logger. It takes input the same way Logger does, \n
 * but insted of writing it to a file it sends it to a ThreadLoggerServer via a mailbox. \n
 * \n
 * ThreadLoggerServer is a complementary class of ThreadLoggerClient. It reads input from mailbox \n
 * received from ThreadLoggerClient and writes it to a log file. \n
 * \n
 * \n
 * \n
 * Since ThreadLoggerClient only needs to send log message via mailbox to ThreadLoggerServer which is \n
 * much faster than writing it to a file (which ThreadLoggerServer handles). \n
 */
class ThreadLoggerClient : public ILogger
{
    private:

    /// ThreadLoggerClient mailbox used to send log messages to ThreadLoggerServer
    Mailbox mailbox;

    /// Reference to ThreadLoggerServer mailbox to which log messages are sent
    MailboxReference loggerMailbox;

    std::string m_name;

    public:
    /**
     * @brief Construct a new ThreadLoggerClient object
     * 
     * @param mailboxId String identifier (name) of client side mailbox of ThreadLogger
     * @param loggerMailboxId String identifier (name) of server side mailbox of ThreadLogger
     */
    ThreadLoggerClient(const std::string& log_file_name);

    std::string getName() const;

    virtual ~ThreadLoggerClient();

    /// Method which is called whenever something is logged
    ThreadLoggerClient& logString(std::string const& report);

};

#endif