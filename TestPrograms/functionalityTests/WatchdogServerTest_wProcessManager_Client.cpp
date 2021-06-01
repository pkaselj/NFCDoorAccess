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

	Logger logger("client" + nameExt + ".log");
	// DataMailbox mailbox("clientUnit", &logger);
	// MailboxReference server(SERVER_MB_NAME + ".server");


	SlotSettings settings;
	settings.m_BaseTTL = 5;
	settings.m_timeout_ms = 1 * Time::s_to_ms;

	sleep(2);

	/*

	WatchdogMessage register_request("clientUnit", settings, getpid(), enuActionOnFailure::KILL_ALL, WatchdogMessage::REGISTER_REQUEST);
	mailbox.send(server, &register_request);

	sleep(1);

	WatchdogMessage start_timer("clientUnit", settings, getpid(), enuActionOnFailure::KILL_ALL, WatchdogMessage::START);
	mailbox.send(server, &start_timer);

	sleep(1);

	WatchdogMessage kicker("clientUnit", settings, getpid(), enuActionOnFailure::KILL_ALL, WatchdogMessage::KICK);
	for (int i = 0; i < 5; i++)
	{
		usleep(900 * Time::ms_to_us);
		mailbox.send(server, &kicker);
	}

	sleep(3);

	for (int i = 0; i < 5; i++)
	{
		usleep(900 * Time::ms_to_us);
		mailbox.send(server, &kicker);
	}
	*/

	WatchdogClient client("clientUnit" + nameExt, SERVER_MB_NAME, settings, enuActionOnFailure::RESET_ONLY, &logger);

	client.Start();

	sleep(1);

	for (int i = 0; i < 5; i++)
	{
		usleep(900 * Time::ms_to_us);
		client.Kick();
	}

	/*sleep(3);

	for (int i = 0; i < 5; i++)
	{
		usleep(900 * Time::ms_to_us);
		client.Kick();
	}*/


	std::cout << "Client" << nameExt <<  " sleeping!" << std::endl;

	sleep(100);

	std::cout << "Client" << nameExt << " ended!" << std::endl;
}

