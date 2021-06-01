#ifndef IPROCESS_HPP
#define IPROCESS_HPP

#include"/home/pi/Shared/WatchdogLib/0.0.0/WatchdogClient.hpp"

class IProcess
{
    private:
    
    WatchdogClient processWatchdog;
    MailboxReference mailboxReference;

    public:
    int status; //
    IProcess(WatchdogClient _wdc, const std::string& name) : processWatchdog(_wdc), mailboxReference(name) {};

    virtual void initialize() = 0;
    virtual void start() = 0;

    void synchronize()
    {
        processWatchdog.Synchronize();
    }

    MailboxReference& getMailboxReference()
    {
        return mailboxReference;
    }
};

#endif