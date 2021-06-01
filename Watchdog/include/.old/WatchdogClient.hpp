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

#ifndef WATCHDOG_CLIENT_HPP
#define WATCHDOG_CLIENT_HPP

#include "IWatchdogSlot.hpp"

#include"NulLogger.hpp"
#include"Timer.hpp"
#include"DataMailbox.hpp"
#include"SharedMemory.hpp"

/*
class WatchdogSlot_ClientSide;
class WatchdogUnit;


class WatchdogSlot_ClientSide : public IWatchdogSlot
{
public:
    WatchdogSlot_ClientSide(const std::string& name, const SlotSettings& settings, ILogger* pLogger = NulLogger::getInstance());
    ~WatchdogSlot_ClientSide();

    void syncSettingsWithWServer(const SlotSettings& newSettings);

    WatchdogUnit newUnit(const std::string& name);

    static void setWatchdogServerName(const std::string& serverName) { m_serverName = serverName; };

    void registerSlot();
    void deregisterSlot();

    bool Pet();

private:
    static std::string m_serverName;

    SharedMemory<SlotStatus> m_slotSharedMemory;
    unsigned int m_offset;

    ILogger* m_pLogger;

    Timer m_timer;

};

class WatchdogUnit
{
public:
    typedef enum : BYTE
    {
        WARNING = BIT(0),
        TERMINATE = BIT(1)
    } WatchdogUnitPriority;

    WatchdogUnit(const std::string& name, WatchdogSlot& parentSlot, WatchdogUnitPriority priority = TERMINATE);

    bool Pet();

    ~WatchdogUnit();
private:
    BYTE m_offset;

    std::string m_name;
};
*/
#endif