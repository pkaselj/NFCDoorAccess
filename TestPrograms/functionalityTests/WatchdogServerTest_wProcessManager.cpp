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

#include "WatchdogServer.hpp"
#include "LoggerToStdout.hpp"
#include <sys/wait.h>
#include <sys/resource.h>

const std::string SERVER_MB_NAME = "server";

int main(int argc, char** argv)
{
	std::cout << "Server started!" << std::endl;

	/*struct rlimit resource_limit;
	int result = getrlimit(RLIMIT_NOFILE, &resource_limit);

	std::cout << "SOFT: " << resource_limit.rlim_cur << std::endl
		<< "HARD: " << resource_limit.rlim_max << std::endl;

	int no_of_open_fd = 0;
	int max_open_fd = resource_limit.rlim_cur;
	for (int i = 0; i < max_open_fd; i++)
		if (fcntl(i, F_GETFD) != 0)
			++no_of_open_fd;

	std::cout << "OPEN: " << no_of_open_fd << "/" << max_open_fd << std::endl;

	resource_limit.rlim_cur = 1024 * 1024;
	if (setrlimit(RLIMIT_NOFILE, &resource_limit) < 0)
		exit(-5);*/

	std::cout << "START" << std::endl;

	LoggerToStdout logger("server.log");

	ProcessManager processManager(&logger);

	
	processManager.createProcess("WatchdogServerTest_wProcessManager_ClientWorking", { "1" });
	// processManager.createProcess("WatchdogServerTest_wProcessManager_ClientWorking", { "2" });
	// processManager.createProcess("WatchdogServerTest_wProcessManager_ClientWorking", { "3" });
	// processManager.createProcess("WatchdogServerTest_wProcessManager_ClientWorking", { "4" });
	// processManager.createProcess("WatchdogServerTest_wProcessManager_ClientWorking", { "5" });
	// processManager.createProcess("WatchdogServerTest_wProcessManager_ClientWorking");

	processManager.initiateAll();
	
	WatchdogServer server(SERVER_MB_NAME, &processManager, &logger);

	server.SetPeriod_us(500 * Time::ms_to_us);

	while (!server.hasRequestedTermination())
	{
		server.WaitForRequestAndParse();
	}

	std::cout << "Server ended!" << std::endl;

}


