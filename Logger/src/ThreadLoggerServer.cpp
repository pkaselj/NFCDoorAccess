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

#include"ThreadLoggerServer.hpp"

ThreadLoggerServer::ThreadLoggerServer(const std::string& log_file_name, ILogger* _logger)
    :   mailbox(log_file_name + ".tserver"), m_name(log_file_name)
{
    p_logger = _logger;
    if(p_logger == nullptr)
        p_logger = NulLogger::getInstance();

    if(m_name == "")
    {
        Kernel::Fatal_Error("Thread Logger Server log file name cannot be empty");
    }

    available = true;
}


ThreadLoggerServer::~ThreadLoggerServer(void)
{

}

void ThreadLoggerServer::startLogging(void)
{
    if(this->isAvailable() != true)
        return;

    mailbox_message message = mailbox.receiveImmediate();

    if(p_logger != nullptr) // ? NEEDED ?
        *p_logger << "TL: " + message.sender + " : " + message.content;
}
