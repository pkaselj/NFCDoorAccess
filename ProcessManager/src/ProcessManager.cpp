#include "ProcessManager.hpp"
#include "Kernel.hpp"

#include<sys/wait.h>

#include<algorithm>

ProcessManager::ProcessManager(ILogger* _p_logger)
{
    m_pLogger = _p_logger;
    if(m_pLogger == nullptr)
        m_pLogger = NulLogger::getInstance();

    *m_pLogger << "ProcessManager created!";
}

ProcessManager::~ProcessManager()
{
    /*
        for (auto& process : m_processes)
            delete process;
    */
    while (m_processes.empty() == false)
    {
        delete m_processes.back();
        m_processes.pop_back();
    }

    *m_pLogger << "ProcessManager destroyed!";
    
}

Process* ProcessManager::createProcess(const std::string& pathname, Process::enuInitOptions options)
{
    return createProcess(pathname, {}, options);
}

Process* ProcessManager::createProcess(const std::string& pathname, const std::vector<char*>& arguments, Process::enuInitOptions options)
{
    Process* p_process = new Process(pathname, arguments, options);
    m_processes.push_back(p_process);

    if (p_process == nullptr)
    {
        *m_pLogger << "ProcessManager - error while trying to allocate process pointer for: " + pathname;
        Kernel::Fatal_Error("ProcessManager - error while trying to allocate process pointer for: " + pathname);
    }

    *m_pLogger << "Created process with name: " + pathname;

    return p_process;
}

/*
    Process* ProcessManager::createProcess(std::string&& pathname)
    {
        Process* p_process = new Process(pathname);
        m_processes.push_back(p_process);
    
        if (p_process == nullptr)
        {
            *m_pLogger << "ProcessManager - error while trying to allocate process pointer for: " + pathname;
            Kernel::Fatal_Error("ProcessManager - error while trying to allocate process pointer for: " + pathname);
        }
    
        *m_pLogger << "Created process with name: " + pathname;
    
        return p_process;
    }
*/


int ProcessManager::initiateAll()
{
    int pid = -1;

    for(const auto& process : m_processes)
    {
        pid = process->initiate();
        if(pid == 0)
        {
            exit(0);
            break;
        }

        *m_pLogger << "Initiated process: \"" + process->getName() + "\" with PID: " + std::to_string(pid);
        Kernel::Trace("Initiated process: \"" + process->getName() + "\" with PID: " + std::to_string(pid));
    }

    return pid;
        
}

void ProcessManager::killAll()
{ 
    const unsigned int childExitTimeout_s = 10;

    *m_pLogger << "Kill all!";
    for (auto& process : m_processes)
    {
        *m_pLogger << "Process: " + process->getName();

        *m_pLogger << "\tChecking if it already exited.";
        if (process->hasExited() == false)
        {
            *m_pLogger << "\tProcess still running!";
            process->killProcess();

            sleep(childExitTimeout_s);

            if (process->hasExited() == false)
            {
                logProcess_NotResponding(process);

                process->forceKillProcess();
                process->waitForProcess();

                logProcess_ForceKilled(process);
            }
        }

        logProcess_Killed(process);
        
    }

}

void ProcessManager::forceKillAll()
{
    *m_pLogger << "Force kill all!";
    for (auto& process : m_processes)
    {
        *m_pLogger << "Process: " + process->getName();

        *m_pLogger << "\tChecking if it already exited.";
        if (process->hasExited() == false)
        {
            *m_pLogger << "\tProcess still running!";
            process->forceKillProcess();
            process->waitForProcess();
        }

        logProcess_ForceKilled(process);
        
        delete process;
    }
        
    /*
        int pid_done = -1;
        while ((pid_done = wait(NULL)) > 0)
        {
            *m_pLogger << "Child with PID: " + std::to_string(pid_done) + " exited!";
            Kernel::Trace("Child with PID: " + std::to_string(pid_done) + " exited!");
        }
    */

        

    *m_pLogger << "All children exited!";
    Kernel::Trace("All children exited!");
}

void ProcessManager::resetProcess(unsigned int PID)
{
    auto matchesPID = [=](Process* pProcess) {return pProcess->getPID() == PID;};
    auto position = std::find_if(m_processes.begin(), m_processes.end(), matchesPID);

    if (position == m_processes.end())
    {
        *m_pLogger << "ProcessManager: Trying to reset process out of process managers jurisdiction - PID: " + std::to_string(PID);
        Kernel::Warning("ProcessManager: Trying to reset process out of process managers jurisdiction - PID: " + std::to_string(PID));
        return;
    }

    (*position)->restart();

    *m_pLogger << "Restarted process: \"" + (*position)->getName() + "\" with PID: " + std::to_string((*position)->getPID());
    Kernel::Trace("Restarted process: \"" + (*position)->getName() + "\" with PID: " + std::to_string((*position)->getPID()) );

}

void ProcessManager::logProcess_Killed(Process* pProcess)
{
    if (pProcess == nullptr)
    {
        logProcess_Nullptr();
        return;
    }

    *m_pLogger << "Child with PID: " + std::to_string(pProcess->getPID()) + " killed!";
    Kernel::Trace("Child with PID: " + std::to_string(pProcess->getPID()) + " killed!");
}

void ProcessManager::logProcess_ForceKilled(Process* pProcess)
{
    if (pProcess == nullptr)
    {
        logProcess_Nullptr();
        return;
    }

    *m_pLogger << "Child with PID: " + std::to_string(pProcess->getPID()) + " force killed!";
    Kernel::Trace("Child with PID: " + std::to_string(pProcess->getPID()) + " force killed!");
}

void ProcessManager::logProcess_NotResponding(Process* pProcess)
{
    if (pProcess == nullptr)
    {
        logProcess_Nullptr();
        return;
    }

    *m_pLogger << "Child with PID: " + std::to_string(pProcess->getPID()) + " not responding!";
    Kernel::Trace("Child with PID: " + std::to_string(pProcess->getPID()) + " not responding!");
}

inline void ProcessManager::logProcess_Nullptr()
{
    *m_pLogger << "Process pointer argument points to null!";
}