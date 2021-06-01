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

#ifndef DOOR_CONTROLLER_HPP
#define DOOR_CONTROLLER_HPP

#include "Timer.hpp"

/// Class which represents electronic lock
class Door
{
public:
	Door() = delete;

	/**
	 * @brief Create new Door object
	 * @param pinBCM Door `SIGNAL` pin (Broadcomm/BCM pin numbering)
	 * @param doorOpenTime_ms Time duration which the door stays unlocked (in milliseconds)
	*/
	Door(unsigned int pinBCM, unsigned int doorOpenTime_ms);
	~Door();

	void OpenDoors();

private:
	unsigned int m_pinBCM;
	unsigned int m_doorOpenTime_ms;

	Timer m_tmrDoor;
	TimerCallbackFunctor<Door>* m_pTimeoutCallback;

	void closeTheDoor_timerCallback(void*);
};

#endif