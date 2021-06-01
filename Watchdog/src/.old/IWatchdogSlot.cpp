#include "IWatchdogSlot.hpp"

IWatchdogSlot::IWatchdogSlot(const std::string& name, SlotStatus* pStatus)
	: m_name(name), m_pStatus(pStatus), m_settings(), m_PID(0)
{
	if (m_name == "")
		Kernel::Fatal_Error("Name of the IWatchdogSlot cannot be empty!");

	/*if (m_pStatus == nullptr)
		Kernel::Fatal_Error(m_name + " - IWatchdogSlot::m_pSlot cannot point to nullptr!");*/

}

IWatchdogSlot::~IWatchdogSlot()
{

}