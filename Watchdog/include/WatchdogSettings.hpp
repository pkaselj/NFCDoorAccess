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

#ifndef WATCHDOG_SETTINGS_HPP
#define WATCHDOG_SETTINGS_HPP

struct SlotSettings;

bool operator==(const SlotSettings& s1, const SlotSettings& s2);

struct SlotSettings
{
    unsigned int m_BaseTTL;
    unsigned int m_timeout_ms;

	bool timeoutIsZero() const { return m_timeout_ms == 0; }
	bool isZero() const { return *this == (SlotSettings) { 0, 0 }; }
};

enum class enuActionOnFailure
{
	RESET_ONLY = 0,
	KILL_ALL
};

struct WatchdogUnitControlBlock
{
	bool m_bAlive = false;
	bool m_bTerminate = false;
	bool m_bTaken = false;

	void Clear()
	{
		m_bAlive = m_bTerminate = m_bTaken = false;
	}
};

#endif
