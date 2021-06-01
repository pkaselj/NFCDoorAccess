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

#ifndef BUZZER_CONTROLLER_HPP
#define BUZZER_CONTROLLER_HPP

/// Class which represents Active buzzer
class Buzzer
{
public:
	Buzzer() = delete;
	/**
	 * @brief Create new Buzzer object
	 * @param pinBCM Buzzer `SIGNAL` pin (Broadcomm/BCM pin numbering)
	*/
	Buzzer(unsigned int pinBCM);
	~Buzzer();

	void SignalSuccess();
	void SignalFailure();
	void SignalPing();

private:
	unsigned int m_pinBCM;
};

#endif