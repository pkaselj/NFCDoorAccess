#include "UNIX_SignalHandler.hpp"

#include<iostream>

#include<unistd.h>

volatile sig_atomic_t flag = 0;

int main()
{
	UNIX_SignalHandler::bindSignalToFlag(UNIX_SignalHandler::enuSIGINT, &flag);

	std::cout << "Waiting..." << std::endl;

	while (!flag)
	{
		
	}

	std::cout << "Success!" << std::endl;

	return 0;
}