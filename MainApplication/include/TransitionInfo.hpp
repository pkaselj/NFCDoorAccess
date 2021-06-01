#ifndef TRANSITION_INFO_HPP
#define TRANSITION_INFO_HPP

#include <sstream>


namespace TransitionInfoDefaults
{
	const std::string BLANK_AUT_NAME = "{BLANK_TRANSITION_INFO_AUT_NAME}";
	const std::string BLANK_EVT_DESC = "{BLANK_TRANSITION_INFO_EVT_DESC}";
	const int BLANK_STATE_ID = -1;
	const int BLANK_EVENT_ID = -1;
}

struct TransitionInfo
{
	std::string m_automatonName = TransitionInfoDefaults::BLANK_AUT_NAME;
	int m_stateId = TransitionInfoDefaults::BLANK_STATE_ID;
	int m_eventId = TransitionInfoDefaults::BLANK_EVENT_ID;
	std::string m_eventDesc = TransitionInfoDefaults::BLANK_EVT_DESC;

	std::string createLogEntry_Start() const;
	std::string createLogEntry_End() const;

};

std::string TransitionInfo::createLogEntry_Start() const
{
	std::stringstream logStringBuilder;

	logStringBuilder
		<< m_automatonName << " "
		<< "*****START******"
		<< "STATE: " << m_stateId << " "
		<< "EVENT: " << m_eventDesc << " [ " << m_eventId << " ] "
		<< "****************";

	return logStringBuilder.str();
}

std::string TransitionInfo::createLogEntry_End() const
{
	std::stringstream logStringBuilder;

	logStringBuilder
		<< m_automatonName << " "
		<< "******END******"
		<< "STATE: " << m_stateId << " "
		<< "EVENT: " << m_eventDesc << " [ " << m_eventId << " ] "
		<< "****************";

	return logStringBuilder.str();
}

#endif