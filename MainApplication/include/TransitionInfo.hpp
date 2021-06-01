/*
*	 Copyright (C) Petar Kaselj 2021
*
*	 This file is part of NFCDoorAccess.
*
*	 NFCDoorAccess is written by Petar Kaselj as an employee of
*	 Emovis tehnologije d.o.o. which allowed its release under
*	 this license.
*
*    NFCDoorAccess is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NFCDoorAccess is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NFCDoorAccess.  If not, see <https://www.gnu.org/licenses/>.
*
*/

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