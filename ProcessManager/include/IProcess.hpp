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

#ifndef IPROCESS_HPP
#define IPROCESS_HPP

#include"/home/pi/Shared/WatchdogLib/0.0.0/WatchdogClient.hpp"

class IProcess
{
    private:
    
    WatchdogClient processWatchdog;
    MailboxReference mailboxReference;

    public:
    int status; //
    IProcess(WatchdogClient _wdc, const std::string& name) : processWatchdog(_wdc), mailboxReference(name) {};

    virtual void initialize() = 0;
    virtual void start() = 0;

    void synchronize()
    {
        processWatchdog.Synchronize();
    }

    MailboxReference& getMailboxReference()
    {
        return mailboxReference;
    }
};

#endif