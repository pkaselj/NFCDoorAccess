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

#include "IWatchdogSlot.hpp"

IWatchdogSlot::IWatchdogSlot(const std::string& name, SlotStatus* pStatus)
	: m_name(name), m_pStatus(pStatus), m_settings(), m_PID(0)
{
	if (m_name == "")
		Kernel::Fatal_Error("Name of the IWatchdogSlot cannot be empty!");

	/*if (m_pStatus == nullptr)
		Kernel::Fatal_Error(m_name + " - IWatchdogSlot::m_pSlot cannot point to nullptr!");*/

}

IWatchdogSlot::~IWatchdogSlot()
{

}