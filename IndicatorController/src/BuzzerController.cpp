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

#include "BuzzerController.hpp"

#include <unistd.h>

#include <pigpio.h>

#include "Kernel.hpp"
#include "propertiesclass.h"

const unsigned int pingDuration_ms = GlobalProperties::Get().BUZZER_PING_DURATION_MS;

Buzzer::Buzzer(unsigned int pinBCM)
	:	 m_pinBCM(pinBCM)
{
	int pinStatus = gpioSetMode(m_pinBCM, PI_OUTPUT);
	if (pinStatus < 0)
	{
		Kernel::Fatal_Error("Buzzer invalid pin: " + std::to_string(m_pinBCM));
	}

	gpioSetPullUpDown(m_pinBCM, PI_PUD_DOWN);
	gpioWrite(m_pinBCM, PI_LOW);
}

Buzzer::~Buzzer()
{
}

void Buzzer::SignalSuccess()
{
	gpioWrite(m_pinBCM, PI_HIGH);
	usleep(5 * pingDuration_ms * 1000);

	gpioWrite(m_pinBCM, PI_LOW);
	usleep(5 * pingDuration_ms * 1000);

	gpioWrite(m_pinBCM, PI_HIGH);
	usleep(5 * pingDuration_ms * 1000);

	gpioWrite(m_pinBCM, PI_LOW);
	//usleep(5 * pingDuration_ms * 1000);

}

void Buzzer::SignalFailure()
{
	gpioWrite(m_pinBCM, PI_HIGH);
	usleep(5 * pingDuration_ms * 1000);

	gpioWrite(m_pinBCM, PI_LOW);
	usleep(5 * pingDuration_ms * 1000);

	gpioWrite(m_pinBCM, PI_HIGH);
	usleep(5 * 5 * pingDuration_ms * 1000);

	gpioWrite(m_pinBCM, PI_LOW);
	//usleep(5 * pingDuration_ms * 1000);
}

void Buzzer::SignalPing()
{
	gpioWrite(m_pinBCM, PI_HIGH);
	usleep(2 * pingDuration_ms * 1000);

	gpioWrite(m_pinBCM, PI_LOW);
	//usleep(2 * pingDuration_ms * 1000);

}
