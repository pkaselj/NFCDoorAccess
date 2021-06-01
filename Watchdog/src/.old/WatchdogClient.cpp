#include"WatchdogClient.hpp"

WatchdogSlot_ClientSide::WatchdogSlot_ClientSide(const std::string& name, const SlotSettings& settings, ILogger* pLogger)
	:	IWatchdogSlot(name), m_settings(settings), m_pLogger(pLogger)
{
	if (m_pLogger == nullptr)
		m_pLogger = NulLogger::getInstance();

	DataMailbox mailbox(m_serverName, m_pLogger);
}

