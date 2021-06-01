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

#ifndef WATCHDOG_SERVER_HPP
#define WATCHDOG_SERVER_HPP

#include "DataMailbox.hpp"
#include "Timer.hpp"
#include "ProcessManager.hpp"
#include "SharedMemory.hpp"

#include<list>

#include <mutex>

class WatchdogServer;

/**
 * @brief Class which represents WatchdogClients. Used by WatchdogServer
*/
class WatchdogUnit
{
public:

	WatchdogUnit(const std::string& name,
		int offset,
		const SlotSettings& settings,
		unsigned int PID,
		TimerCallbackFunctor<WatchdogServer>* pTimeoutCallback,
		enuActionOnFailure onFailure = enuActionOnFailure::RESET_ONLY,
		ILogger* pLogger = NulLogger::getInstance());

	WatchdogUnit(const WatchdogUnit&) = delete;
	WatchdogUnit(WatchdogUnit&&);

	~WatchdogUnit();

	friend bool operator==(const WatchdogUnit& unit, const std::string& name) { return unit.m_name == name; }

	void StartTimer();
	void RestartTimer();
	void RestartTTL();
	void StopTimer();
	void UpdateSettings(const SlotSettings& settings);

	bool Expired();

	/// Decrements TTL and returns it. TTL will not go below 0.
	int DecrementAndReturnTTL();

	std::string getName() const { return m_name; }
	std::string getTimerName() const { return m_pTimer->getName(); }
	unsigned int getPID() const { return m_PID; }
	enuActionOnFailure getActionOnFailure() { return m_onFailure; }
	unsigned int getOffset() const { return m_offset; }

private:

	/*const*/ enuActionOnFailure m_onFailure;
	Timer* m_pTimer; // TODO ranodm int to timer name
	ILogger* m_pLogger;
	std::string m_name;
	SlotSettings m_settings;
	unsigned int m_PID;
	TimerCallbackFunctor<WatchdogServer>* m_pTimeoutCallback;
	int m_TTL;
	int m_offset;
	
};

using unitsIterator = std::list<WatchdogUnit>::iterator;

/**
 * @brief Class used to monitor WatchdogClients
*/
class WatchdogServer
{

public:
	/**
	 * @brief Create a new WatchdogServer object
	 * @param name Name of the server. GLOBALLY UNIQUE
	 * @param pProcessManager Pointer to the ProcessManager object which started the mointored processes
	 * @param pLogger Pointer to an ILogger derived class to log messages to
	*/
	WatchdogServer(const std::string name, ProcessManager* pProcessManager, ILogger* pLogger = NulLogger::getInstance());
	~WatchdogServer();

	// void StartWatchdogServer(volatile sig_atomic_t stopFlag);

	/// Starts listening for messages (requests), parses them and takes appropriate actions. Runs until m_terminationFlag get set to true
	void WaitForRequestAndParse();

	void DisableTimeouts() { m_bTimeoutsEnabled = false; };
	
	void Stop();

	/// Terminates all attached units
	void TerminateAll();

	/// UNUSED TODO PRIVATE
	void MarkTimerExpired(void* pExpiredTimer_voidptr);

	/// Sets the WatchdogServer period -> sleep time between rounds of checking timers
	void SetPeriod_us(unsigned int period_ns);

	ProcessManager* getProcessManager() { return m_pProcessManager; }

	/// USED INTERNALLY TODO PRIVATE
	bool timedOut() const { return m_readTimeoutFlag; }

	/// USED INTERNALLY TODO PRIVATE
	bool nonblockingReadEmpty() const { return m_nonblockingReadEmptyFlag; }

	bool hasRequestedTermination() const { return m_terminationFlag; }

private:

	/*const*/ TimerCallbackFunctor<WatchdogServer> m_timeoutCallbackFunctor;

	std::mutex objectMutex;

	bool m_bTimeoutsEnabled = true;

	/// Sets the callback for Timer timeout signal (SIGALRM).
	void InitTimeoutCallback();

	/// Starts the synchronization period. UNUSED
	void StartSynchronization(unsigned int timeout_ms, unsigned int BaseTTL);

	/// Parses the received `request` and takes appropriate action
	void ParseRequest(WatchdogMessage& request);

	/// Starts checking for expired units and decrements TTL and resets/kills unresponsive units. Runs until m_terminationFlag get set to true
	void StartCheckingForExpiredUnits();

	/// Sends termination signal to `unit`
	void SendTerminateBroadcast(WatchdogUnit& unit);

	/// Handles the expiration (TTL == 0) of `expiredUnit`. Can be used in a for-each loop as expiredUnit gets decremented before the unit gets removed from list.
	void HandleUnitExpiration(unitsIterator& expiredUnitIter);

	void AddNewUnit(WatchdogMessage& request);
	void RemoveUnit(WatchdogMessage& request);
	unitsIterator RemoveUnit(unitsIterator& unitIter);
	void UpdateSettings(WatchdogMessage& request);
	void KickTimer(WatchdogMessage& request);
	void StartTimer(WatchdogMessage& request);
	void StopTimer(WatchdogMessage& request);

	void fillSharedUnitMemory();
	int getFirstAvailableSlot();

	/// UNUSED
	void SetUnitStatusReadyAndWaiting(WatchdogMessage& request);

	/// Handles the situation when a unit requests termination
	void UnitRequestedTermination(WatchdogMessage& request);

	/// Listens for message of class `messageClass` with `options`. Sets appropriate flags `m_readTimeoutFlag` and `m_nonblockingReadEmptyFlag`
	WatchdogMessage listenForMessage(WatchdogMessage::MessageClass messageClass = WatchdogMessage::MessageClass::ANY,
		enuReceiveOptions options = enuReceiveOptions::NORMAL);

	typedef enum
	{
		Ignore = 0,
		Warn,
		Crash
	} enuActionOnSearchFailure;

	/// Returns the iterator of the unit wih the `name` in `m_units`. If such unit is not found then it does the `action`
	unitsIterator getIteratorMatching(const std::string& name, enuActionOnSearchFailure action = Ignore);
	unitsIterator getIteratorMatchingTimer(const std::string& name, enuActionOnSearchFailure action = Ignore);

	std::string m_name;
	ILogger* m_pLogger;
	ILogger* m_pUnitLogger;
	ProcessManager* m_pProcessManager;

	DataMailbox m_mailbox;
	// TimerCallbackFunctor<WatchdogServer> m_timeoutCallback;

	bool m_terminationFlag = false;
	bool m_readTimeoutFlag = false;
	bool m_nonblockingReadEmptyFlag = false;

	unsigned int m_period_us;

	std::list<WatchdogUnit> m_units;
	SharedMemory<WatchdogUnitControlBlock> m_smhUnitControls;

	// volatile std::list<Timer*> m_expired_pTimers;
};



#endif