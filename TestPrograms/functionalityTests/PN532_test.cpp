#include "PN532_NFC.hpp"
#include "Logger.hpp"

#include<iostream>

int main()
{
	Logger logger("PN532.log");
	PN532_NFC cardReader(&logger);
	while (true)
	{
		std::cout << cardReader.readCardUUID() << std::endl;
	}
	return 0;
}