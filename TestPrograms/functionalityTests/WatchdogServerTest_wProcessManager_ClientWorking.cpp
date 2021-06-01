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

#include "WatchdogClient.hpp"
#include "LoggerToStdout.hpp"
#include <sys/wait.h>

const std::string SERVER_MB_NAME = "server";

int main(int argc, char** argv)
{
	std::cout << "Client started!" << std::endl;

	std::string nameExt = "";
	if (argc > 1)
	{
		nameExt = argv[1];
		std::cout << "Client started! ---- " << nameExt << std::endl;
	}
	else
	{
		std::cout << "Client started! ---- " << "NO ARG" << std::endl;
	}

	Logger logger("clientWorking" + nameExt + ".log");
	// DataMailbox mailbox("clientWorking", &logger);
	// MailboxReference server(SERVER_MB_NAME + ".server");


	SlotSettings settings;
	settings.m_BaseTTL = 5;
	settings.m_timeout_ms = 1 * Time::s_to_ms;

	sleep(2);


	WatchdogClient client("clientWorking" + nameExt, SERVER_MB_NAME, settings, enuActionOnFailure::KILL_ALL, &logger);

	client.Start();

	sleep(1);

	while (client.Kick())
	{
		usleep(100 * Time::ms_to_us);
		logger << "Aliveeeeeeeee!";
	}

	std::cout << "Working Client ended!" << std::endl;
}

