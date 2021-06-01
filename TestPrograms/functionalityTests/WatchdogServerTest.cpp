#include "WatchdogServer.hpp"
#include "LoggerToStdout.hpp"
#include <sys/wait.h>

void sender();
void receiver();

int main(int argc, char** argv)
{

	int pid = fork();
	if (pid != 0)
	{
		receiver();
	}
	else
	{
		sleep(3);
		sender();
	}

	wait(nullptr);

	return 0;
}

const std::string SERVER_MB_NAME = "server";

void sender()
{
	std::cout << "Client started!" << std::endl;

	Logger logger("client.log");
	DataMailbox mailbox("client", &logger);
	MailboxReference server(SERVER_MB_NAME + ".server");

	SlotSettings settings;
	settings.m_BaseTTL = 5;
	settings.m_timeout_ms = 1 * Time::s_to_ms;

	WatchdogMessage register_request("clientUnit", settings, getpid(), enuActionOnFailure::RESET_ONLY, WatchdogMessage::REGISTER_REQUEST);
	mailbox.send(server, &register_request);

	WatchdogMessage start_timer("clientUnit", settings, getpid(), enuActionOnFailure::RESET_ONLY, WatchdogMessage::START);
	mailbox.send(server, &start_timer);

	
	WatchdogMessage kicker("clientUnit", settings, getpid(), enuActionOnFailure::RESET_ONLY, WatchdogMessage::KICK);
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

	sleep(50);

	std::cout << "Client ended!" << std::endl;

}

void receiver()
{
	std::cout << "Server started!" << std::endl;

	Logger logger("server.log");
	WatchdogServer server(SERVER_MB_NAME, nullptr, &logger);


	std::cout << "Server ended!" << std::endl;

	exit(-1);
}
