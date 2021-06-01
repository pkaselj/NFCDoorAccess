#ifndef WATCHDOG_CLIENT_HPP
#define WATCHDOG_CLIENT_HPP

#include "IWatchdogSlot.hpp"

#include"NulLogger.hpp"
#include"Timer.hpp"
#include"DataMailbox.hpp"
#include"SharedMemory.hpp"

/*
class WatchdogSlot_ClientSide;
class WatchdogUnit;


class WatchdogSlot_ClientSide : public IWatchdogSlot
{
public:
    WatchdogSlot_ClientSide(const std::string& name, const SlotSettings& settings, ILogger* pLogger = NulLogger::getInstance());
    ~WatchdogSlot_ClientSide();

    void syncSettingsWithWServer(const SlotSettings& newSettings);

    WatchdogUnit newUnit(const std::string& name);

    static void setWatchdogServerName(const std::string& serverName) { m_serverName = serverName; };

    void registerSlot();
    void deregisterSlot();

    bool Pet();

private:
    static std::string m_serverName;

    SharedMemory<SlotStatus> m_slotSharedMemory;
    unsigned int m_offset;

    ILogger* m_pLogger;

    Timer m_timer;

};

class WatchdogUnit
{
public:
    typedef enum : BYTE
    {
        WARNING = BIT(0),
        TERMINATE = BIT(1)
    } WatchdogUnitPriority;

    WatchdogUnit(const std::string& name, WatchdogSlot& parentSlot, WatchdogUnitPriority priority = TERMINATE);

    bool Pet();

    ~WatchdogUnit();
private:
    BYTE m_offset;

    std::string m_name;
};
*/
#endif