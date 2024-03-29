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
#include <thread>

#include "SimplifiedMailbox.hpp"

#include<sys/resource.h>
#include<sys/fcntl.h>
#include<cstring>

using namespace std;

typedef struct
{
	int m_len;
	char* m_pMsg;
} Message;

int main()
{
	/*struct rlimit resource_limit;
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
		exit(-5);*/

	cout << "START" << endl;

	cout << "R-" << "START" << endl;


	Logger logger("receiver.log");
	SimplifiedMailbox receiver("receiver", &logger);

	for (int i = 0; i < 10; i++)
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

	return 0;


	cout << "R-" << "END" << endl;

	cout << "END" << endl;
}
