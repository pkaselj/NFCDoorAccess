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

#ifndef WATCHDOG_SLOT_HPP
#define WATCHDOG_SLOT_HPP

#include "Kernel.hpp"

struct SlotSettings
{
    unsigned int m_BaseTTL;
    unsigned int m_timeout_ms;
};

struct SlotStatus
{
    unsigned int m_status;
    unsigned int m_terminationMask;
    unsigned int m_warningMask;
    unsigned int m_errorPosition;
    unsigned int m_warningPosition;

    FLAG_REGISTER m_fTerminate : 1,
        m_fSync : 1,
        m_fFree : 1;
};

class IWatchdogSlot
{
public:
    IWatchdogSlot(const std::string& name, SlotStatus* pStatus = nullptr);

    virtual ~IWatchdogSlot() = 0;

    void setSettings(const SlotSettings& newSettings) { m_settings = newSettings; };

    SlotSettings getCurrentSettings() { return m_settings; };
protected:

    std::string m_name;

    SlotStatus* m_pStatus;

    SlotSettings m_settings;

    unsigned int m_PID;
};






#endif
