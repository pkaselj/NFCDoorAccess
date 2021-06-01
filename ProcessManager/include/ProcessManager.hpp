/*
*	 Copyright (C) Petar Kaselj 2021
*
*	 This file is part of NFCDoorAccess.
*
*	 NFCDoorAccess is written by Petar Kaselj as an employee of
*	 Emovis tehnologije d.o.o. which allowed its release under
*	 this license.
*
*    NFCDoorAccess is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NFCDoorAccess is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NFCDoorAccess.  If not, see <https://www.gnu.org/licenses/>.
*
*/

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