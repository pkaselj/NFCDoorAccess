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
		exit(-5);
		*/
	cout << "START" << endl;
	cout << "S-" << "START" << endl;

	Logger logger("sender.log");
	SimplifiedMailbox sender("sender", &logger);

	MailboxReference destination("receiver");

	Message message = { .m_len = 12, .m_pMsg = "Test Message" };

	char serializedMessage[message.m_len + sizeof(int)];

	memcpy((void*)serializedMessage, (const void*)&message.m_len, sizeof(int));
	memcpy((void*)(serializedMessage + sizeof(int)), (const void*)message.m_pMsg, message.m_len);

	for (int i = 0; i < 10; i++)
	{
		cout << "====================================================" << endl;
		sender.send(destination, serializedMessage, sizeof(serializedMessage));
		cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
	}



	cout << "S-" << "END" << endl;

	cout << "END" << endl;
}
