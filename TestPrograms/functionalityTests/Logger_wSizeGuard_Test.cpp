#include "Logger.hpp"

int main()
{
	Logger logger("test.log", 100);

	int i = 0;
	while (++i < 10)
	{
		std::cout << i << std::endl;
		logger << "TEst string test test test test test test tets test fsdf";
		sleep(1);
	}
	
	return 0;
}