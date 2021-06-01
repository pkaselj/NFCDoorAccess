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

#include"pipe.hpp"
#include"LoggerToStdout.hpp"
#include<unistd.h>
#include<string>

int main()
{
	int pid = fork();

	if (pid == 0)
	{
		Logger logger("pipe.log");
		Pipe pipe1("pipename", Kernel::IOMode::IOMode::WRITE, &logger);

		while (true)
		{
			pipe1.send("MSG");
			sleep(1);
		}

	}
	else
	{
		LoggerToStdout logger("pipe.log");
		Pipe pipe1("pipename", Kernel::IOMode::IOMode::READ_NONBLOCKING, &logger);

		while (true)
		{
			std::string message = pipe1.receive();
			usleep(100000);
		}

	}


	return 0;
}