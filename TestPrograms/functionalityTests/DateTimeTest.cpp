#include <iostream>

#include "Time.hpp"

int main()
{
	std::cout << "DateTime: " << Time::getDateTime_ISO8601() << std::endl;

	return 0;
}