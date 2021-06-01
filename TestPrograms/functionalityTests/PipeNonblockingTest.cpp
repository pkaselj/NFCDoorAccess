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