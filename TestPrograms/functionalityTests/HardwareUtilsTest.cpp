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

#include "IndicatorController.hpp"

#include<iostream>
#include<thread>
#include<pigpio.h>

#include <unistd.h>

const std::string IDENTIFIER = "ind";

void Client();
void Server();

int main()
{
	gpioInitialise();
	std::thread serverThread(Server);
	std::thread clientThread(Client);

	clientThread.join();
	gpioTerminate();
	return 0;
}

void Server()
{
	Pinout pins(20, 21, 1500, 1);
	IndicatorController_Server indicators(IDENTIFIER, pins);
	
	while (true)
	{
		indicators.ListenAndParseRequest(1000);
	}

}


typedef void (IndicatorController_Client::*Command)();

void Client()
{
	std::vector<Command> commands = {
		& IndicatorController_Client::BuzzerFailure,
		& IndicatorController_Client::BuzzerPing,
		& IndicatorController_Client::BuzzerSuccess,
		& IndicatorController_Client::OpenDoor_wBuzzerSuccess,
		& IndicatorController_Client::LCD_Clear,
		& IndicatorController_Client::LCD_Clear_wDefaultMsg
	};

	IndicatorController_Client indicators(IDENTIFIER);

	sleep(2);

	for (auto& action : commands)
	{
		(indicators.*action)();
		sleep(1);
	}

	indicators.LCD_Put_Permanently("PermanentMessage");
	sleep(1);
	indicators.LCD_Put_wTimeout("Temp Message");
	sleep(5);
	
}