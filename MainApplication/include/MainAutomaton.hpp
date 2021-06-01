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

#ifndef MAIN_AUTOMATON_HPP
#define MAIN_AUTOMATON_HPP

#include "mautomat.h"
#include "DataMailbox.hpp"
#include "TransitionInfo.hpp"
#include "IndicatorController.hpp"

class KeypadAutomaton;

class MainAutomaton : public MAutomat
{
public:
	MainAutomaton(short int siInstID,
		IndicatorController_Client* pIndicators,
		ILogger* pLogger = NulLogger::getInstance());

	virtual ~MainAutomaton() {};

	void setKeypadAutomatonPointer(KeypadAutomaton* pKeypadAutomaton);

	bool initialize();

	// TODO not a very good idea!
	bool m_bGuestAccessEnable;
	

	// STATES =================================================

	typedef enum
	{
		enuIdle = 0,
		enuBusy_KeypadRFID = 1
	} enuAutStateType;
	// ========================================================

	// EVENTS =================================================

	typedef enum
	{
		enuEvtKeypadMessageReceived = 0,
		enuEvtKeypadAutomatonFinished,

	}enuAutEventType;

	// ========================================================

	virtual bool reset();
	virtual bool processEvent(MAutEvent* pEvent);

	std::string getAutomatonName() const { return "MainAutomaton"; }
	
private:
	ILogger* m_pLogger;
	KeypadAutomaton* m_pKeypadAutomaton;
	IndicatorController_Client* m_pIndicators;


	// AUTOMAT FUNCTIONS ======================================

	bool doForwardEventToKeypadAutomaton(MAutEvent* pEvent);

	bool doVoid(MAutEvent* pEvent);

	bool doDisplayTempMsg_AndClear(MAutEvent* pEvent);

	// ========================================================

	TransitionInfo getTransitionInfo(MAutEvent* pReceivedEvent);

};

class MainAutomatonEvent : public MAutEvent
{
public:
	MainAutomatonEvent(int idEvent, DataMailboxMessage* pMessage);

	virtual ~MainAutomatonEvent() {}

	DataMailboxMessage* m_pMessage;
};

#endif

