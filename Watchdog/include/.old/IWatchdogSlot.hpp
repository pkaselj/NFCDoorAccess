#ifndef WATCHDOG_SLOT_HPP
#define WATCHDOG_SLOT_HPP

#include "Kernel.hpp"

struct SlotSettings
{
    unsigned int m_BaseTTL;
    unsigned int m_timeout_ms;
};

struct SlotStatus
{
    unsigned int m_status;
    unsigned int m_terminationMask;
    unsigned int m_warningMask;
    unsigned int m_errorPosition;
    unsigned int m_warningPosition;

    FLAG_REGISTER m_fTerminate : 1,
        m_fSync : 1,
        m_fFree : 1;
};

class IWatchdogSlot
{
public:
    IWatchdogSlot(const std::string& name, SlotStatus* pStatus = nullptr);

    virtual ~IWatchdogSlot() = 0;

    void setSettings(const SlotSettings& newSettings) { m_settings = newSettings; };

    SlotSettings getCurrentSettings() { return m_settings; };
protected:

    std::string m_name;

    SlotStatus* m_pStatus;

    SlotSettings m_settings;

    unsigned int m_PID;
};






#endif
