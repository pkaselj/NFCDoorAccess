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

#include "WatchdogServer.hpp"

#include <algorithm>
#include <thread>
#include <sstream>

#include <csignal>
#include <string.h>

const unsigned int initialShmCapacity = 15;
// const std::string shmName = "watchdog.units";

WatchdogServer::WatchdogServer(const std::string name, ProcessManager * pProcessManager, ILogger * pLogger)
	: m_mailbox(name + ".server"),
	m_pLogger(pLogger),
	m_name(name),
	objectMutex(),
	m_terminationFlag(false),
	m_period_us(100 * Time::ms_to_us),
	m_pProcessManager(pProcessManager),
	m_timeoutCallbackFunctor(this, &WatchdogServer::MarkTimerExpired),
	m_smhUnitControls(name + ".shm", initialShmCapacity, pLogger)
{
	if (m_pLogger == nullptr)
	{
		m_pLogger = NulLogger::getInstance();
	}

	if (m_name == "")
	{
		*m_pLogger << "WatchdogServer name cannot be empty!";
		Kernel::Fatal_Error("WatchdogServer name cannot be empty!");
	}

	if (m_pProcessManager == nullptr)
	{
		*m_pLogger << m_name + " - WatchdogServer must have process manager.";
		Kernel::Fatal_Error(m_name + " - WatchdogServer must have process manager.");
	}

	m_units.clear();

	fillSharedUnitMemory();

	InitTimeoutCallback();
}

WatchdogServer::~WatchdogServer()
{
	m_units.clear();
}


void WatchdogServer::WaitForRequestAndParse()
{
	m_mailbox.setRTO_ns(m_period_us * Time::us_to_ns);

	WatchdogMessage received_request = listenForMessage(WatchdogMessage::ANY, enuReceiveOptions::TIMED);

	if (m_readTimeoutFlag == true)
	{
		m_readTimeoutFlag = false;
		return;
	}

	// TODO New thread?
	// std::lock_guard<std::mutex> lock(objectMutex);
	ParseRequest(received_request);

}

WatchdogMessage WatchdogServer::listenForMessage(WatchdogMessage::MessageClass messageClass, enuReceiveOptions options)
{
	m_readTimeoutFlag = false;
	m_nonblockingReadEmptyFlag = false;

	DataMailboxMessage* pReceivedMessage = m_mailbox.receive(options);
	MessageDataType messageType = pReceivedMessage->getDataType();

	if (messageType == MessageDataType::enuType::DataMailboxErrorMessage)
	{
		DataMailboxErrorMessage* pErrorMessage = dynamic_cast<DataMailboxErrorMessage*>(pReceivedMessage);
		switch (pErrorMessage->getErrorStatus())
		{
		case DataMailboxErrorMessage::enuErrorStatus::TimedOut:
			m_readTimeoutFlag = true;

		case DataMailboxErrorMessage::enuErrorStatus::QueueEmpty:
			m_nonblockingReadEmptyFlag = true;
		}

		delete pErrorMessage;
		return WatchdogMessage(WatchdogMessage::MessageClass::NONE);
	}

	else if (messageType != MessageDataType::enuType::WatchdogMessage)
	{
		*m_pLogger << m_name + " - WatchdogServer received invalid type of message: MessageDataType =  " + pReceivedMessage->getDataType().toString();
		Kernel::Fatal_Error(m_name + " - WatchdogServer received invalid type of message: MessageDataType =  " + pReceivedMessage->getDataType().toString());
	}

	WatchdogMessage decodedMessage = std::move( *(dynamic_cast<WatchdogMessage*>(pReceivedMessage)) );

	delete pReceivedMessage;
	// *m_pLogger << m_name + " - WatchdogMessage received: " + received_message_parsed.getInfo();

	return decodedMessage;
}

void WatchdogServer::StartSynchronization(unsigned int timeout_ms, unsigned int BaseTTL)
{
	*m_pLogger << m_name + " - Watchdog sync period start.";

	// Wait for the first unit to register
	WatchdogMessage received_request = listenForMessage(WatchdogMessage::REGISTER_REQUEST);
	AddNewUnit(received_request);


	timespec oldTimeoutSettings = m_mailbox.getTimeout_settings();
	m_mailbox.setRTO_ns(timeout_ms * Time::ms_to_ns);

	// set TTL to BaseTTL
	// every timeout decrease TTL
	// every message == REGISTER_REQUEST restart TTL to BaseTTL
	unsigned int TTL = BaseTTL;

	do
	{
		*m_pLogger << m_name + " - Sync period TTL left: " + std::to_string(TTL);

		WatchdogMessage received_request = listenForMessage(WatchdogMessage::REGISTER_REQUEST, enuReceiveOptions::TIMED);
		if (m_readTimeoutFlag == true)
		{
			m_readTimeoutFlag = false;
			continue;
		}

		// else
		// reset TTL
		TTL = BaseTTL + 1; // +1 to compensate for --TTL at the end of the loop iteration
		AddNewUnit(received_request);

	} while (--TTL > 0);

	*m_pLogger << m_name + " - Sync listen period over. Sending broadcast signals";

	// Send SYNC_BROADCAST to all connected units (CONNECTIONLESS)

	WatchdogMessage sync_broadcast(m_name, WatchdogMessage::SYNC_BROADCAST);
	for (auto& unit : m_units)
	{
		MailboxReference destination(unit.getName());
		m_mailbox.sendConnectionless(destination, &sync_broadcast);
		*m_pLogger << m_name + " - Sync broadcast sent to " + unit.getName();
	}

	// Reset mailbox timeout settings
	m_mailbox.setTimeout_settings(oldTimeoutSettings);

	*m_pLogger << m_name + " - Sync period over. Broadcast signals sent";
}

void WatchdogServer::SetPeriod_us(unsigned int period_us)
{
	m_period_us = period_us;
}

void WatchdogServer::ParseRequest(WatchdogMessage& request)
{
	std::lock_guard<std::mutex> lock(objectMutex);
	using MessageClass = WatchdogMessage::MessageClass;
	switch (request.getMessageClass())
	{
	case MessageClass::REGISTER_REQUEST:
	{
		// std::lock_guard<std::mutex> lock(objectMutex);
		AddNewUnit(request);
	}
	break;
	case MessageClass::UNREGISTER_REQUEST:
	{
		// std::lock_guard<std::mutex> lock(objectMutex);
		RemoveUnit(request);
	}
	break;
	case MessageClass::UPDATE_SETTINGS:
		UpdateSettings(request);
		break;
	case MessageClass::KICK:
		KickTimer(request);
		break;
	case MessageClass::START:
		StartTimer(request);
		break;
	case MessageClass::STOP:
		StopTimer(request);
		break;
	case MessageClass::SYNC_REQUEST:
		SetUnitStatusReadyAndWaiting(request);
		break;
	case MessageClass::TERMINATE_REQUEST:
		UnitRequestedTermination(request);
		break;
	default:
		// ERROR TODO
		*m_pLogger << m_name + " - Watchdog Server received invalid WatchdogMessage: " + request.getInfo();
		Kernel::Warning(m_name + " - Watchdog Server received invalid WatchdogMessage: " + request.getInfo());
		break;

	}
}

void WatchdogServer::MarkTimerExpired(void* pExpiredTimer_voidptr)
{

	Timer* pExpiredTimer = reinterpret_cast<Timer*>(pExpiredTimer_voidptr);
	if (pExpiredTimer == nullptr)
	{
		*m_pLogger << "Watchdog Server - Expired timer is nullptr!";
		Kernel::Fatal_Error("Watchdog Server - Expired timer is nullptr!");
	}

	auto position = getIteratorMatchingTimer(pExpiredTimer->getName(), WatchdogServer::enuActionOnSearchFailure::Crash);

	*m_pLogger << position->getTimerName() + " - Timer expired!";

	int offset = position->getOffset();

	// std::cout << "Expiration offset: " << offset << std::endl;

	if (offset < 0 || offset >= m_smhUnitControls.getLength())
	{
		*m_pLogger << "Offset out of bounds! Offset: " + std::to_string(offset);
		Kernel::Fatal_Error("Offset out of bounds! Offset: " + std::to_string(offset));
	}

	if (m_smhUnitControls[offset].m_bAlive == true)
	{
		m_smhUnitControls[offset].m_bAlive = false;
		pExpiredTimer->Reset();
		return;
	}

	int RemainingTTL = position->DecrementAndReturnTTL();

	if (RemainingTTL <= 0)
	{
		HandleUnitExpiration(position);
	}
	else
	{
		pExpiredTimer->Reset();
	}
}


void WatchdogServer::HandleUnitExpiration(unitsIterator& expiredUnitIter)
{
	if (expiredUnitIter->getActionOnFailure() == enuActionOnFailure::RESET_ONLY)
	{
		int processPID = expiredUnitIter->getPID();

		expiredUnitIter = RemoveUnit(expiredUnitIter);

		m_pProcessManager->resetProcess(processPID);
	}
	else if (expiredUnitIter->getActionOnFailure() == enuActionOnFailure::KILL_ALL)
	{

		m_terminationFlag = true;
	}
}

void WatchdogServer::Stop()
{
	UNIX_SignalHandler::BlockSignal(UNIX_SignalHandler::enuSIGALRM);
	for (auto& unit : m_units)
	{
		unit.StopTimer();
	}
	UNIX_SignalHandler::UnblockSignal(UNIX_SignalHandler::enuSIGALRM);
}

void WatchdogServer::TerminateAll()
{
	*m_pLogger << m_name + " - Terminate all!";
	m_terminationFlag = true;

	while (m_units.empty() == false)
	{
		WatchdogUnit* pUnit = &m_units.front();
		m_units.pop_front();

		pUnit->StopTimer();
		SendTerminateBroadcast(*pUnit);
	}

	m_pProcessManager->killAll();
}

void WatchdogServer::SendTerminateBroadcast(WatchdogUnit& unit)
{
	*m_pLogger << m_name + " - Terminating unit: " + unit.getName();

	WatchdogMessage terminate_broadcast(m_name, WatchdogMessage::TERMINATE_BROADCAST);
	MailboxReference destination(unit.getName());

	m_mailbox.sendConnectionless(destination, &terminate_broadcast);

	*m_pLogger << m_name + " - Terminate broadcast sent to unit: " + unit.getName();

}

void WatchdogServer::InitTimeoutCallback()
{
	Timer::ignoreAlarmSignals();
}

void WatchdogServer::AddNewUnit(WatchdogMessage& request)
{
	*m_pLogger << m_name + " - WatchdogServer - requested addition of unit: " + request.getName();

	auto matchesName = [&](WatchdogUnit& unit) {return unit.getName() == request.getName();};
	auto position = std::find_if(m_units.begin(), m_units.end(), matchesName);


	if (position != m_units.end())
	{

		*m_pLogger << m_name + " - WatchdogServer Unit tries to register itself multiple times. Request info: " + request.getInfo();
		Kernel::Fatal_Error(m_name + " - WatchdogServer Unit tries to register itself multiple times. Request info: " + request.getInfo());
		return;
	}

	int offset = getFirstAvailableSlot();
	if (offset == -1)
	{
		*m_pLogger << m_name + " - WatchdogServer Unit - no more available slots in shared memory control block.";
		Kernel::Fatal_Error(m_name + " - WatchdogServer Unit - no more available slots in shared memory control block.");
	}

	request.setOffset(offset);

	std::stringstream stringBuilder;

	stringBuilder << "Name: " << request.getName() << "\n"
		<< "\tTimeout(ms): " << request.getSettings().m_timeout_ms << "\n"
		<< "\tBase TTL: " << request.getSettings().m_BaseTTL << "\n"
		<< "\tPID: " << request.getPID() << "\n"
		<< "\tOffset: " << request.getOffset() << "\n"
		<< "\tAction on failure code: " << (int)request.getActionOnFailure() << "\n"
		<< "\tLogger pointer address: " << std::hex << (int)m_pLogger;

	*m_pLogger << stringBuilder.str();

	m_units.push_back(
		WatchdogUnit(
			request.getName(),
			offset,
			request.getSettings(),
			request.getPID(),
			&m_timeoutCallbackFunctor,
			request.getActionOnFailure(),
			m_pLogger
		)
	);



	*m_pLogger << m_name + " - WatchdogServer - unit matching request: " + request.getName() + " - added!";

	WatchdogMessage register_reply(m_name, WatchdogMessage::REGISTER_REPLY);
	register_reply.setOffset(offset);

	m_mailbox.sendConnectionless(request.getSource(), &register_reply);

	// std::cout << "ServerSide offset: " << offset << std::endl;

	*m_pLogger << m_name + " - WatchdogServer - sent register ACK to unit: " + request.getName();

}

int WatchdogServer::getFirstAvailableSlot()
{
	for (size_t i = 0; i < m_smhUnitControls.getLength(); ++i)
	{
		if (m_smhUnitControls[i].m_bTaken == false)
		{
			m_smhUnitControls[i].m_bTaken = true;
			return i;
		}
	}

	return -1;
}

void WatchdogServer::fillSharedUnitMemory()
{
	WatchdogUnitControlBlock emptyControlBlock;
	for (size_t i = 0; i < m_smhUnitControls.getLength(); ++i)
	{
		memcpy(&m_smhUnitControls[i], &emptyControlBlock, sizeof(WatchdogUnitControlBlock));
	}
}

unitsIterator WatchdogServer::RemoveUnit(unitsIterator& unitIter)
{
	auto unitToBeErasedIter = unitIter--;

	unitToBeErasedIter->StopTimer();

	m_smhUnitControls[unitToBeErasedIter->getOffset()].Clear();

	m_units.erase(unitToBeErasedIter);

	return unitIter;
}

void WatchdogServer::RemoveUnit(WatchdogMessage& request)
{
	*m_pLogger << m_name + " - WatchdogServer - requested removal of unit: " + request.getName();

	auto equalsRequestName = [&](WatchdogUnit& unit) {return unit == request.getName(); };
	auto position = std::find_if(m_units.begin(), m_units.end(), equalsRequestName);


	if (position == m_units.end())
	{
		*m_pLogger << m_name + " - WatchdogServer - no WatchdogUnits matching request: " + request.getName() + " - found! No units removed!";
		Kernel::Warning(m_name + " - WatchdogServer - no WatchdogUnits matching request: " + request.getName() + " - found! No units removed!");
		return;
	}

	RemoveUnit(position);

	*m_pLogger << m_name + " - WatchdogServer - unit matching request: " + request.getName() + " - removed!";
}

void WatchdogServer::UpdateSettings(WatchdogMessage& request)
{
	*m_pLogger << m_name + " - WatchdogServer - request to update settings: " + request.getInfo();

	auto position = getIteratorMatching(request.getName(), enuActionOnSearchFailure::Crash);

	position->UpdateSettings(request.getSettings());
}

void WatchdogServer::KickTimer(WatchdogMessage& request)
{
	*m_pLogger << m_name + " - WatchdogServer - request to kick timer: " + request.getName();

	auto position = getIteratorMatching(request.getName(), enuActionOnSearchFailure::Warn);

	if (position != m_units.end())
	{
		position->RestartTTL();
		position->RestartTimer();
	}

}

void WatchdogServer::StartTimer(WatchdogMessage& request)
{
	*m_pLogger << m_name + " - WatchdogServer - request to start timer: " + request.getName();

	auto position = getIteratorMatching(request.getName(), enuActionOnSearchFailure::Crash);

	position->StartTimer();
}

void WatchdogServer::StopTimer(WatchdogMessage& request)
{
	*m_pLogger << m_name + " - WatchdogServer - request to stop timer: " + request.getName();

	auto position = getIteratorMatching(request.getName(), enuActionOnSearchFailure::Crash);

	position->StopTimer();
}

void WatchdogServer::SetUnitStatusReadyAndWaiting(WatchdogMessage& request)
{
	*m_pLogger << m_name + " - WatchdogServer - sync request from: " + request.getName();

	auto position = getIteratorMatching(request.getName(), enuActionOnSearchFailure::Ignore);

	// TODO
}
void WatchdogServer::UnitRequestedTermination(WatchdogMessage& request)
{
	*m_pLogger << m_name + " - WatchdogServer - terminate request from: " + request.getName();

	auto position = getIteratorMatching(request.getName(), enuActionOnSearchFailure::Ignore);

	if (position != m_units.end())
	{
		HandleUnitExpiration(position);
	}

}

unitsIterator WatchdogServer::getIteratorMatching(const std::string& name, enuActionOnSearchFailure action)
{
	auto matchesName = [&](WatchdogUnit& unit) {return unit == name;};
	auto position = std::find_if(m_units.begin(), m_units.end(), matchesName);

	if (position == m_units.end())
	{
		std::string error_string = m_name + " - WatchdogServer - unit matching name: " + name + " - not found.";
		*m_pLogger << error_string;

		if (action == enuActionOnSearchFailure::Warn)
		{
			Kernel::Warning(error_string);
		}
		else if (action == enuActionOnSearchFailure::Crash)
		{
			Kernel::Fatal_Error(error_string);
		}

	}

	return position;
}

unitsIterator WatchdogServer::getIteratorMatchingTimer(const std::string& name, enuActionOnSearchFailure action)
{
	auto matchesName = [&](WatchdogUnit& unit) {return unit.getTimerName() == name;};
	auto position = std::find_if(m_units.begin(), m_units.end(), matchesName);

	if (position == m_units.end())
	{
		std::string error_string = m_name + " - WatchdogServer - unit matching timer name: " + name + " - not found.";
		*m_pLogger << error_string;

		if (action == enuActionOnSearchFailure::Warn)
		{
			Kernel::Warning(error_string);
		}
		else if (action == enuActionOnSearchFailure::Crash)
		{
			Kernel::Fatal_Error(error_string);
		}

	}

	return position;
}



WatchdogUnit::WatchdogUnit(const std::string& name,
	int offset,
	const SlotSettings& settings,
	unsigned int PID,
	TimerCallbackFunctor<WatchdogServer>* pTimeoutCallback,
	enuActionOnFailure onFailure,
	ILogger* pLogger)
	: m_name(name),
	m_settings(settings),
	m_pLogger(pLogger),
	m_pTimer(nullptr),
	m_PID(PID),
	m_pTimeoutCallback(pTimeoutCallback),
	m_onFailure(onFailure),
	m_offset(offset)
{

	m_pTimer = new Timer(name + std::string(".timer"));

	if (m_pLogger == nullptr)
	{
		m_pLogger = NulLogger::getInstance();
	}

	if (m_name == "")
	{
		*m_pLogger << "WatchdogUnit name cannot be empty!";
		Kernel::Fatal_Error("WatchdogUnit name cannot be empty!");
	}

	if (m_settings.m_BaseTTL == 0 || m_settings.m_timeout_ms == 0)
	{
		std::string error_string = "WatchdogUnit: " + m_name + " timeout settings are invalid!\n" +
			"\tBaseTTL = " + std::to_string(m_settings.m_BaseTTL) + "\n" +
			"\tTimeout_ms = " + std::to_string(m_settings.m_timeout_ms) + "\n";

		*m_pLogger << error_string;
		Kernel::Fatal_Error(error_string);
	}

	m_TTL = m_settings.m_BaseTTL;
	m_pTimer->setTimeout_ms(m_settings.m_timeout_ms);
	m_pTimer->setTimeoutCallback(m_pTimeoutCallback);
}

WatchdogUnit::WatchdogUnit(WatchdogUnit&& other)
{
	m_name = other.m_name;
	m_settings = other.m_settings;
	m_pLogger = other.m_pLogger;
	m_pTimer = other.m_pTimer;
	m_PID = other.m_PID;
	m_pTimeoutCallback = other.m_pTimeoutCallback;
	m_onFailure = other.m_onFailure;
	m_TTL = other.m_TTL;
	m_offset = other.m_offset;

	other.m_pTimer = nullptr;
}

WatchdogUnit::~WatchdogUnit()
{
	if (m_pTimer != nullptr)
	{
		m_pTimer->Stop();
		delete m_pTimer;
	}

}

void WatchdogUnit::StartTimer()
{
	m_pTimer->clearTimeoutSettings();
	m_pTimer->setTimeout_ms(m_settings.m_timeout_ms);
	m_pTimer->Start();
}

void WatchdogUnit::RestartTimer()
{
	m_pTimer->Reset();
}

void WatchdogUnit::RestartTTL()
{
	m_TTL = m_settings.m_BaseTTL;
}

void WatchdogUnit::StopTimer()
{
	m_pTimer->Stop();
}

void WatchdogUnit::UpdateSettings(const SlotSettings& settings)
{
	m_settings = settings;
	m_pTimer->setTimeout_ms(m_settings.m_timeout_ms);

	// ?
	m_TTL = m_settings.m_BaseTTL;
}

bool WatchdogUnit::Expired()
{
	return m_pTimer->getTimerStatus() == Timer::enuTimerStatus::Expired;
}

int WatchdogUnit::DecrementAndReturnTTL()
{

	if (m_TTL != 0)
	{
		--m_TTL;
	}

	return m_TTL;
}