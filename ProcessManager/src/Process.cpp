#include"Process.hpp"

#include<unistd.h>
#include<signal.h>
#include<sys/wait.h>

#include<iostream> // DEBUG

Process::Process(const std::string& pathname, enuInitOptions options)
    : m_pathname(pathname), m_arguments({nullptr}), m_status(enuProcessStatus::NOT_STARTED), m_initiator(&Process::initiate_normal)
{
    setProcessInitiator(options);
}

Process::Process(const std::string& pathname, const std::vector<char*>& arguments, enuInitOptions options)
    : m_pathname(pathname), m_arguments(arguments), m_status(enuProcessStatus::NOT_STARTED), m_initiator(&Process::initiate_normal)
{
    m_arguments.push_back(nullptr);
    setProcessInitiator(options);
}

int Process::initiate()
{ 
    return (this->*m_initiator)();
}

void Process::setProcessInitiator(enuInitOptions options)
{
    if (options == enuInitOptions::MEMCHECK)
    {
        m_initiator = &Process::initiate_wMemcheck;
        return;
    }

    else if (options == enuInitOptions::CALLGRIND)
    {
        m_initiator = &Process::initiate_wCallgrind;
        return;
    }

    else if (options == enuInitOptions::GDB)
    {
        m_initiator = &Process::initiate_wGDB;
        return;
    }

    m_initiator = &Process::initiate_normal;
}

int Process::initiate_wMemcheck()
{
    m_pid = fork();
    if (m_pid == 0)
    {
        m_status = RUNNING;

        static int valgrind_log_nr = 0;
        
        const std::string valgrindLogFile = "valgrindLog_" + std::to_string(std::rand()) + "." + std::to_string(++valgrind_log_nr) + ".val.log";
        const std::string valgrindMemcheckShellCommand = "valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --error-limit=no --log-file="
            + valgrindLogFile + " ./" + m_pathname;

        system(valgrindMemcheckShellCommand.c_str());

        m_status = STOPPED;

        exit(0);
    }

    return m_pid;
}

int Process::initiate_normal()
{
    m_pid = fork();
    if (m_pid == 0)
    {
        m_status = RUNNING;
        if (m_arguments.size() > 0)
        {
            m_arguments.insert(m_arguments.begin(), const_cast<char*>(m_pathname.c_str())); // TODO move to constructor ?
            execv(m_pathname.c_str(), &m_arguments[0]);

            // DEBUG
            // system(("./" + m_pathname + ";echo " + m_pathname + " $?").c_str());
        }
        else
        {
            // execl(m_pathname.c_str(), m_pathname.c_str(), (void*)NULL);
            // DEBUG
            system( ("./" + m_pathname + ";echo " + m_pathname + " $?").c_str() );
        }

        m_status = STOPPED;

        exit(0);
    }

    return m_pid;
}

int Process::initiate_wCallgrind()
{
    m_pid = fork();
    if (m_pid == 0)
    {
        m_status = RUNNING;

        const std::string valgrindMemcheckShellCommand = "valgrind --tool=callgrind ./" + m_pathname;

        system(valgrindMemcheckShellCommand.c_str());

        m_status = STOPPED;

        exit(0);
    }

    return m_pid;
}

int Process::initiate_wGDB()
{
    m_pid = fork();
    if (m_pid == 0)
    {
        m_status = RUNNING;

        const std::string gdbShellCommand = "sudo gdb ./" + m_pathname;

        system(gdbShellCommand.c_str());

        m_status = STOPPED;

        exit(0);
    }

    return m_pid;
}

void Process::forceKillProcess()
{
    kill(m_pid, SIGKILL);
}

void Process::killProcess()
{
    kill(m_pid, SIGTERM);
}

std::string Process::getName() const
{
    return m_pathname;
}

void Process::restart()
{

    int processExited = waitpid(m_pid, nullptr, WNOHANG);
    if (processExited == 0)
    {
        killProcess();
        waitpid(m_pid, nullptr, 0);
    }

    

    /*
    if (m_status == RUNNING)
    {
        // killProcess();
        forceKillProcess();

        // sleep(waitTime);
    }

    waitpid(m_pid, nullptr, 0);
    */
    
    

    initiate();
}

int Process::waitForProcess()
{
    return waitpid(m_pid, nullptr, 0);
}

bool Process::hasExited()
{
    int childExited = waitpid(m_pid, nullptr, WNOHANG);
    return childExited > 0; // process exited
}

