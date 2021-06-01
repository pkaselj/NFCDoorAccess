#include "WatchdogSettings.hpp"

bool operator==(const SlotSettings& s1, const SlotSettings& s2)
{
	return (s1.m_BaseTTL == s2.m_BaseTTL) && (s1.m_timeout_ms == s2.m_timeout_ms);
}