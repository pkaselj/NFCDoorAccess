#include"ThreadLoggerClient.hpp"
#include<memory>

ThreadLoggerClient::ThreadLoggerClient(const std::string& log_file_name)
    :   mailbox(log_file_name + ".tclient"), loggerMailbox(log_file_name + ".tserver")
    {
        m_name = log_file_name;
        if(m_name == "")
        {
            Kernel::Fatal_Error("Thread Logger Client log file name cannot be empty!");
        }
    }


ThreadLoggerClient& ThreadLoggerClient::logString(std::string const& report)
{
    mailbox.sendImmediate(loggerMailbox, report);
    return *this;
}

ThreadLoggerClient::~ThreadLoggerClient()
{}

std::string ThreadLoggerClient::getName() const
{
    return m_name;
}
