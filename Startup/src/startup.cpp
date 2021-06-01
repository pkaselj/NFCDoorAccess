#include<cstdlib>
#include<cstdio>
#include<unistd.h>
#include<string>
#include<iostream>
#include<sys/types.h>
#include<signal.h>
#include<vector>
#include<sys/resource.h>
#include<sys/fcntl.h>
#include<thread>

#include "Settings.hpp"
#include "WatchdogServer.hpp"
#include "ProcessManager.hpp"
#include "propertiesclass.h"

volatile sig_atomic_t globalTerminationFlag = 0;

void waitForExitSignal()
{
    std::string input = "";
    do
    {
        std::cout << "Enter 'exit' to close all processes!" << std::endl;
        std::cin >> input;
    } while (input != "exit");

    globalTerminationFlag = 1;
}

void raise_mailbox_limit()
{
    struct rlimit resource_limit;
    int result = getrlimit(RLIMIT_NOFILE, &resource_limit);

    std::cout << "SOFT: " << resource_limit.rlim_cur << std::endl
        << "HARD: " << resource_limit.rlim_max << std::endl;

    int no_of_open_fd = 0;
    int max_open_fd = resource_limit.rlim_cur;
    for (int i = 0; i < max_open_fd; i++)
        if (fcntl(i, F_GETFD) != 0)
            ++no_of_open_fd;

    std::cout << "OPEN: " << no_of_open_fd << "/" << max_open_fd << std::endl;

    resource_limit.rlim_cur = 1024 * 1024;
    if (setrlimit(RLIMIT_NOFILE, &resource_limit) < 0)
        exit(-5);

    std::cout << "START" << std::endl;
}

int main(int argc, char** argv)
{
    // int chdir_status = chdir(STARTUP_PATH.c_str());
    // if (chdir_status < 0)
    // {
    //     return 123;
    // }

    std::cout << "START" << std::endl;

    //std::thread inputReaderThread(waitForExitSignal);

    UNIX_SignalHandler::bindSignalToFlag(UNIX_SignalHandler::enuSIGINT, &globalTerminationFlag);

    // raise_mailbox_limit();

    Logger logger("startup.log");
    Logger processManager_logger("startup.process_manager.log");

    ProcessManager processManager(&processManager_logger);

    const std::string WATCHDOG_SERVER_NAME = GlobalProperties::Get().WATCHDOG_SERVER_NAME;
    WatchdogServer watchdog(WATCHDOG_SERVER_NAME, &processManager, &logger);

    processManager.createProcess(GlobalProperties::Get().HARDWARED_EXECUTABLE /*, Process::enuInitOptions::MEMCHECK*/ );
    processManager.createProcess(GlobalProperties::Get().MAIN_APP_EXECUTABLE /*, Process::enuInitOptions::MEMCHECK*/ );
    processManager.createProcess(GlobalProperties::Get().DBGW_EXECUTABLE /*, Process::enuInitOptions::GDB*/ );

    // processManager.createProcess("WatchdogServerTest_wProcessManager", Process::enuInitOptions::MEMCHECK);
    
    processManager.initiateAll();

    const unsigned int WATCHDOG_SERVER_PERIOD_MS = GlobalProperties::Get().WATCHDOG_SERVER_PERIOD_MS;
    watchdog.SetPeriod_us(WATCHDOG_SERVER_PERIOD_MS * Time::ms_to_us);
    
    while (!globalTerminationFlag && !watchdog.hasRequestedTermination())
    {
        watchdog.WaitForRequestAndParse();
        sleep(1);
    }


    logger << "Program requested termination!";

    // sleep(100);

    processManager.killAll();

    logger << "Program finished successfully!";

    //inputReaderThread.join();

    exit(0);
}
