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

#ifndef WATCHDOG_CLIENT_HPP
#define WATCHDOG_CLIENT_HPP

#include "DataMailbox.hpp"
#include "Timer.hpp"
#include "SharedMemory.hpp"


class WatchdogClient
{
private:
	typedef enum
	{
		UNREGISTERED,
		STOPPED,
		RUNNING,
		SYNC,
		TERMINATING
	} enuStatus;

public:
	/**
	 * @brief Create a new WatchdogClient object
	 * @param unitName GLOBALLY UNIQUE watchdog name TODO add random extension to name
	 * @param pLogger Pointer to an ILogger derived object to log mesages to
	*/
	// TODO DOCS
	WatchdogClient(const std::string& unitName,
		const std::string& serverName,
		const SlotSettings& settings,
		enuActionOnFailure onFailure,
		ILogger* pLogger = NulLogger::getInstance());
	~WatchdogClient();

	/**
	 * @brief Resgister the WatchdogClient with the `serverName` server. Doesn't stat the timer until `Start()` is called.
	 * @param serverName WatchdogServer name
	 * @param settings \see SlotSettings
	 * @param onFailure Action WatchdogServer will take if this unit/client becomes unresponsive
	*/
	void Register();

	/// Unregisters the WatchdogClient from the WatchdogServer. Unregistered watchdogClient is not monitored after unregister
	void Unregister();

	/// Signals the WatchdogServer to start the timer with the settings defined when unit called `Regiter(..., SlotSettings& setings, ...)`. (Optional, can be started with `Kick()`).
	void Start();

	/// Signals the WatchdogServer to stop the timer.
	void Stop();

	/// Sends signal to the registered WatchdogServer to restart the timer. (KEEPALIVE signal)
	bool Kick();

	/// Sends the new settings to the WatchdogServer. New settings will be applied on the next `Kick(), Start() or timeout`
	void UpdateSettings(const SlotSettings& settings);

	/// Signals to the WatchdogServer that the client is ready and waiting for `SYNCHRONIZED` signal. UNUSED
	void Sync();

	/// Signals to the WatchdogServer that the client terminated (or is in process of termination) UNUSED
	void Terminate();

private:

	/// Wait for the reply from `server` of message class `expectedMessageClass` with receive `options` 
	WatchdogMessage waitForServerReply(WatchdogMessage::MessageClass expectedMessageClass,
		enuReceiveOptions timedFlag = enuReceiveOptions::NORMAL,
		long timeout_ms = 0);

	/// Send the signal message to `server` of message class `messageClass` with `sendOptions`
	void sendSignal(WatchdogMessage::MessageClass messageClass, enuSendOptions sendOptions = enuSendOptions::NORMAL);

	/// Saves the `settings` locally (not synchronized with the server)
	void setSettings(const SlotSettings& settings);

	/// Wait for message from `sourceName` with options `timedFlag` and `Unpack()` the message if it is WatchdogMessage else throw error/crash
	WatchdogMessage receiveAndCheckInputMessageFrom(const std::string& sourceName, enuReceiveOptions timedFlag = enuReceiveOptions::NORMAL, long timeout_ms = 0);

	void sendAliveMessageToServer_TimeoutCallback(void*);
	void kickTimer();

	unsigned int m_PID;
	int m_offset;

	// bool m_bAlive;

	// Timer m_tmrKickAlive;
	// TimerCallbackFunctor<WatchdogClient>* m_pCallbackFunctor;

	SlotSettings m_settings;

	std::string m_unitName;
	std::string m_serverName;

	DataMailbox m_mailbox;

	ILogger* m_pLogger;

	enuStatus m_status;
	enuActionOnFailure m_onFailure;

	MailboxReference m_server;

	SharedMemory<WatchdogUnitControlBlock> m_shmControlBlock;

};

#endif
