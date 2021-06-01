#ifndef PROCESS_MANAGER_HPP
#define PROCESS_MANAGER_HPP

#include<vector>

#include"Process.hpp"

#include"Logger.hpp"
#include"NulLogger.hpp"

/**
 * @brief Class used to manage Processes
 * 
 */
class ProcessManager
{

    public:

    /**
     * @brief Create an ProcessManager object
     * @param _p_logger Pointer to an ILogger derived class to log messages
    */
    ProcessManager(ILogger* _p_logger = NulLogger::getInstance());

    /**
     * @brief Destroy the Process Manager object
     * 
     */
    ~ProcessManager();

    /**
     * @brief Creates a process and attaches it to the processes vector.
     * 
     * Process is NOT started until `initiateAll()` is called.
     * 
     * @param pathname Pathname to executable to be executed in a new process
     */
    Process* createProcess(const std::string& pathname, Process::enuInitOptions options = Process::enuInitOptions::NORMAL);

    /**
     * @brief Creates a process and attaches it to the processes vector.
     * 
     * Process is NOT started until `initiateAll()` is called.
     * 
     * @param pathname Pathname to executable to be executed in a new process
     * @param arguments Vector of strings which are passed to the process as arguments. (Process name implicitly passed as the first argument)
     * @return 
    */
    Process* createProcess(const std::string& pathname,
        const std::vector<char*>& arguments,
        Process::enuInitOptions options = Process::enuInitOptions::NORMAL);

    // TODO Move semantics   
    /**
     * @brief Creates a process and attaches it to the processes vector. (WIP)
     * 
     * Using move semantics \n
     * Process is NOT started until `initiateAll()` is called. \n
     * 
     * @param pathname Pathname to executable to be executed in a new process
     */
    // Process* createProcess(std::string&& pathname);
    //void attachProcess(Process* p_process);
    
    /// Initiates (starts) all attached processes
    int initiateAll();

    /// Kill all attached processes. (soft kill - sends )
    void killAll();

    /// Kill all attached processes immediately
    void forceKillAll();

    /// Kill and restart process with PID only if it is owned by this ProcessManager
    void resetProcess(unsigned int PID);

private:
    /// Vector of pointers to attached processes. Attach processes automatically by `createProcess()`
    std::vector<Process*> m_processes;

    ILogger* m_pLogger = nullptr;

    void logProcess_Killed(Process* pProcess);
    void logProcess_ForceKilled(Process* pProcess);
    void logProcess_NotResponding(Process* pProcess);
    inline void logProcess_Nullptr();

};

#endif