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