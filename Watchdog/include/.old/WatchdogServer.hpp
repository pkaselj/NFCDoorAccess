#ifndef WATCHDOG_SERVER_HPP
#define WATCHDOG_SERVER_HPP

#include "IWatchdogSlot.hpp"
#include "DataMailbox.hpp"
#include "SharedMemory.hpp"

#include<vector>
#include <string>

/**
 * @brief Watchdog server side class.
 * 
 */

/*
class WatchdogServer
{
    public:
        WatchdogServer(const std::string& name, unsigned int initialNumberOfSlots, ILogger* pLogger = NulLogger::getInstance());

    private:
        ILogger* m_pLogger;

    DataMailbox m_mailbox;
    std::vector<WatchdogSlot_ServerSide> m_slots;
    SharedMemory<SlotStatus> m_slotStatus;
};

class WatchdogSlot_ServerSide : public IWatchdogSlot
{
public:
    WatchdogSlot_ServerSide(const std::string& name, const SlotSettings& settings);
    ~WatchdogSlot_ServerSide();

    void registerSlot();
    void deregisterSlot();

private:


    friend class WatchdogServer;
};

*/
#endif