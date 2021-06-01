
#include"ThreadLoggerServer.hpp"

ThreadLoggerServer::ThreadLoggerServer(const std::string& log_file_name, ILogger* _logger)
    :   mailbox(log_file_name + ".tserver"), m_name(log_file_name)
{
    p_logger = _logger;
    if(p_logger == nullptr)
        p_logger = NulLogger::getInstance();

    if(m_name == "")
    {
        Kernel::Fatal_Error("Thread Logger Server log file name cannot be empty");
    }

    available = true;
}


ThreadLoggerServer::~ThreadLoggerServer(void)
{

}

void ThreadLoggerServer::startLogging(void)
{
    if(this->isAvailable() != true)
        return;

    mailbox_message message = mailbox.receiveImmediate();

    if(p_logger != nullptr) // ? NEEDED ?
        *p_logger << "TL: " + message.sender + " : " + message.content;
}
