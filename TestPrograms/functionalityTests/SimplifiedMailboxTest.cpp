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

#include <iostream>
#include <unistd.h>
#include<sys/resource.h>
#include<sys/fcntl.h>
#include <sys/wait.h>

#include "SimplifiedMailbox.hpp"
#include "Logger.hpp"
#include "Time.hpp"

#include<thread>
#include<cstring>

constexpr int cycles = 100;

using namespace std;

typedef struct
{
	int m_len;
	char* m_pMsg;
} Message;

using namespace std;

void receiver()
{
	sleep(2);
	cout << "START" << endl;

	cout << "R-" << "START" << endl;


	Logger logger("receiver.log");
	SimplifiedMailbox receiver("receiver", &logger);

	for (int i = 0; i < cycles; i++)
	{

		SimpleMailboxMessage message = receiver.receive();

		char* data = message.m_pData;

		//delete p_message->m_pHeader;
		//delete p_message;

		int size = 0;
		memcpy((void*)&size, (const void*)data, sizeof(int));

		char string_array[size + 1];
		memset((void*)string_array, 0, size + 1);

		memcpy((void*)string_array, (const void*)(data + sizeof(int)), size);

		Message decodedMessage{ .m_len = size, .m_pMsg = string_array };

		cout << " - Received message: " << decodedMessage.m_len << " " << decodedMessage.m_pMsg << endl;

		delete[] data;
	}


	usleep(10000);



	cout << "R-" << "END" << endl;

	cout << "END" << endl;

	return;

}

void sender()
{
	sleep(3);
	cout << "START" << endl;
	cout << "S-" << "START" << endl;

	Logger logger("sender.log");
	SimplifiedMailbox sender("sender", &logger);

	MailboxReference destination("receiver");

	Message message = { .m_len = 12, .m_pMsg = "Test Message" };

	for (int i = 0; i < cycles; i++)
	{
		char serializedMessage[message.m_len + sizeof(int)];

		memcpy((void*)serializedMessage, (const void*)&message.m_len, sizeof(int));
		memcpy((void*)(serializedMessage + sizeof(int)), (const void*)message.m_pMsg, message.m_len);

		sender.send(destination, serializedMessage, sizeof(serializedMessage));
	}


	usleep(10000);



	cout << "S-" << "END" << endl;

	cout << "END" << endl;
	return;
}

int main(int argc, char** argv)
{
	struct rlimit resource_limit;
	int result = getrlimit(RLIMIT_NOFILE, &resource_limit);

	cout << "SOFT: " << resource_limit.rlim_cur << endl
		<< "HARD: " << resource_limit.rlim_max << endl;

	int no_of_open_fd = 0;
	int max_open_fd = resource_limit.rlim_cur;
	for (int i = 0; i < max_open_fd; i++)
		if (fcntl(i, F_GETFD) != 0)
			++no_of_open_fd;

	cout << "OPEN: " << no_of_open_fd << "/" << max_open_fd << endl;

	resource_limit.rlim_cur = 1024 * 1024;
	if (setrlimit(RLIMIT_NOFILE, &resource_limit) < 0)
		exit(-5);

	cout << "START" << endl;

	cout << "Start Time: " << Time::getTime() << endl;

	int pid = fork();
	if (pid == -1)
		exit(-1);

	bool inverted = false;

	if (argc == 2 && strcmp(argv[1], "-i") == 0)
	{
		inverted = true;
		cout << "INVERTED" << endl;
	}
		


	if (pid == 0)
	{
		inverted ? sender() : receiver();
		exit(0);
	}
	else
	{
		inverted ? receiver() : sender();
	}

	wait(nullptr);

	cout << "End Time: " << Time::getTime() << endl;

	cout << "END" << endl;
}
