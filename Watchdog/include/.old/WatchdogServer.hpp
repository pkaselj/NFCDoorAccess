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

#ifndef WATCHDOG_SERVER_HPP
#define WATCHDOG_SERVER_HPP

#include "IWatchdogSlot.hpp"
#include "DataMailbox.hpp"
#include "SharedMemory.hpp"

#include<vector>
#include <string>

/**
 * @brief Watchdog server side class.
 * 
 */

/*
class WatchdogServer
{
    public:
        WatchdogServer(const std::string& name, unsigned int initialNumberOfSlots, ILogger* pLogger = NulLogger::getInstance());

    private:
        ILogger* m_pLogger;

    DataMailbox m_mailbox;
    std::vector<WatchdogSlot_ServerSide> m_slots;
    SharedMemory<SlotStatus> m_slotStatus;
};

class WatchdogSlot_ServerSide : public IWatchdogSlot
{
public:
    WatchdogSlot_ServerSide(const std::string& name, const SlotSettings& settings);
    ~WatchdogSlot_ServerSide();

    void registerSlot();
    void deregisterSlot();

private:


    friend class WatchdogServer;
};

*/
#endif