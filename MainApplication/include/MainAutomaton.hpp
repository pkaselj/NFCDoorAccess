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

