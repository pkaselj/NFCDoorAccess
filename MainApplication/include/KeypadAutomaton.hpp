#ifndef KEYPAD_AUTOMATON_HPP
#define KEYPAD_AUTOMATON_HPP

#include "mautomat.h"
#include "DataMailbox.hpp"
#include "TransitionInfo.hpp"
#include "Timer.hpp"
#include "IndicatorController.hpp"


class MainAutomaton;

class KeypadAutomaton : public MAutomat
{
public:

	KeypadAutomaton(short int siInstID,
		DataMailbox* pMailbox,
		MailboxReference* pRefDatabase,
		IndicatorController_Client* pIndicators,
		ILogger* pLogger = nullptr);

	virtual ~KeypadAutomaton() {}
	
	void setMainAutomatonPointer(MainAutomaton* pMainAutomaton);

	bool initialize();

	// STATES =================================================

	typedef enum
	{
		enuIdle = 0,
		enuWaitingDatabase_Request = 1,
		enuWaitingDatabase_Authorize = 2,
		enuWaitingDatabase_Authorize_GuestAccessEnable = 3,
		enuWaitingDatabase_Authorize_GuestAccessDisable = 4

	} enuAutStateType;
	// ========================================================

	// EVENTS =================================================

	typedef enum
	{
		enuEvtInsufficientPermissions = 0,
		enuEvtSuccess,
		enuEvtSuccess_Guest,
		enuEvtTimedOut,
		enuEvtError,
		enuEvtInvalidCommand,
		enuEvtInvalidParameter,
		enuEvtReceivedClearanceFromDB,
		enuEvtReceivedAddRequest,
		enuEvtReceivedRemoveRequest,
		enuEvtReceiveAuthorizeRequest,
		enuEvtReceiveSetClearanceRequest,
		enuEvtReceivedRequest,
		enuEvtCancel,
		enuEvtEnter,
		enuEvtGuestAccessDisable,
		enuEvtGuestAccessEnable

	}enuAutEventType;

	// ========================================================

	virtual bool reset();
	virtual bool processEvent(MAutEvent* pEvent);

	std::string getAutomatonName() const { return "KeypadAutomaton"; }

private:
	ILogger* m_pLogger;
	MainAutomaton* m_pMainAutomaton;
	DataMailbox* m_pMailbox;
	MailboxReference* m_pRefDatabase;
	IndicatorController_Client* m_pIndicators;
	Timer m_deadlineTimer;
	TimerCallbackFunctor<KeypadAutomaton> m_timeoutCallback;


	// AUTOMAT FUNCTIONS ======================================

	bool doVoid(MAutEvent* pEvent);

	bool doSignalTimeout(MAutEvent* pEvent);

	bool doOpenDoors(MAutEvent* pEvent);

	bool doSendRequestToDB (MAutEvent* pEvent);

	bool doSignalSuccess(MAutEvent* pEvent);

	bool doOpenDoorsAndSignalSuccess(MAutEvent* pEvent);

	bool doSignalError(MAutEvent* pEvent);

	bool doSignalInvalidCommand(MAutEvent* pEvent);

	bool doSignalInvalidParameter(MAutEvent* pEvent);

	bool doSignalInsufficientPermissions(MAutEvent* pEvent);

	bool doSignalBusy(MAutEvent* pEvent);

	bool doCancelAction(MAutEvent* pEvent);

	bool doSignalGuestAccessDenied(MAutEvent* pEvent);

	bool doEnableGuestAccess(MAutEvent* pEvent);

	bool doDisableGuestAccess(MAutEvent* pEvent);

	bool doOpenDoorsIfAuthorized(MAutomat* pEvent);

	// ========================================================

	void signalFinishToMainAutomaton_wTempMessage(const std::string& tempMessageToDisplay);
	void signalFinishToMainAutomaton();

	TransitionInfo getTransitionInfo(MAutEvent* pReceivedEvent);

	void doSendTimeoutEvent_timerWrapper(void*);
};

class KeypadAutomatonEvent : public MAutEvent
{
public:
	KeypadAutomatonEvent(int idEvent, DataMailboxMessage* pMessage);

	virtual ~KeypadAutomatonEvent() {}

	DataMailboxMessage* m_pMessage;
};


#endif