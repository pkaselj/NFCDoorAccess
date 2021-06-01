#ifndef INPUT_AUTOMATON_HPP
#define INPUT_AUTOMATON_HPP

#include "DataMailbox.hpp"
#include "mautomat.h"
#include "IndicatorController.hpp"

class InputAutomatonEvent : public MAutEvent
{
public:
	InputAutomatonEvent(int idEvent, const InputParameter& parameter);

	InputAutomatonEvent(int idEvent);

	virtual ~InputAutomatonEvent() {}

	const InputParameter m_parameter;

private:

	void init(int idEvent);
};

class InputAutomaton : public MAutomat
{
public:

	// STATES ====================================================
	typedef enum
	{
		enuIdle = 0,
		enuAdd = 1,
		enuAddParam1 = 2,
		enuAddParam2 = 3,
		enuAddParam3 = 4,
		enuRemove = 5,
		enuRemoveParam1 = 6,
		enuSetClearance = 7,
		enuSetClearanceParam1 = 8,
		enuSetClearanceParam2 = 9,
		enuGuestAccessControlParam1 = 10
	} enumAutStateType;

	// EVENTS ====================================================

	typedef enum
	{
		// GeneralInputEvents
		enuEvtPlain = 0,
		enuEvtEnter,
		enuEvtCancel,
		enuEvtKeypadPIN,
		enuEvtKeypadCommand,
		enuEvtRFIDCard,

		// CommandInitInputEvents
		enuEvtAddCommand,
		enuEvtRemoveCommand,
		enuEvtSetClearanceCommand,
		enuEvtGuestAccessEnable,
		enuEvtGuestAccessDisable

	} enumAutEventType;

	// ===========================================================

	InputAutomaton(int siInstID,
		DataMailbox* pMailbox,
		MailboxReference* pMainApplication,
		IndicatorController_Client* pIndicators,
		ILogger* pLogger = NulLogger::getInstance());

	virtual ~InputAutomaton() {};

	virtual bool initialize();

	virtual bool processEvent(MAutEvent* event);
	virtual bool reset();

	static std::string getStateName(enumAutStateType state);


private:
	// PRIVATE DATA ==============================================
	ILogger* m_pLogger;

	DataMailbox* m_pMailbox;

	MailboxReference* m_pMainApplication;
	IndicatorController_Client* m_pIndicators;

	CommandMessage m_messageBuffer;

	// PRIVATE HELPERS ===========================================

	void checkResourcePointers();

	void clearMessageBuffer();
	void appendParameterToMessageBuffer(const InputParameter& param);

	void FatalError(const std::string& message);
	void Warning(const std::string& message);

	void logTransitionStart(MAutEvent* event);
	void logTransitionEnd(MAutEvent* event);

	void doCreateNewCommandMessage(CommandMessage::enuCommand command);

	// AUTOMATON FUNCTIONS =======================================

	bool doWarnWrongInput(MAutEvent* pEvent);
	bool doAppendParameter(MAutEvent* pEvent);
	bool doCancel(MAutEvent* pEvent);
	bool doAppendParameterAndSend(MAutEvent* pEvent);
	bool doSend(MAutEvent* pEvent);
	bool doSend_wBuzzerPing(MAutEvent* pEvent);
	bool doVoid(MAutEvent* pEvent);

	// STATE SPECIFIC FUNCTIONS ==================================
	bool doCreateCommand_Add(MAutEvent* pEvent);
	bool doCreateCommand_Remove(MAutEvent* pEvent);
	bool doCreateCommand_SetClearance(MAutEvent* pEvent);
	bool doAppendParameterAndSend_Idle(MAutEvent* pEvent);
	bool doAppendParameter_Add(MAutEvent* pEvent);
	bool doAppendParameter_AddParam1(MAutEvent* pEvent);
	bool doAppendParameterAndSend_AddParam2(MAutEvent* pEvent);
	bool doSend_AddParam2(MAutEvent* pEvent);
	bool doAppendParameter_Remove(MAutEvent* pEvent);
	bool doAppendParameterAndSend_RemoveParam1(MAutEvent* pEvent);
	bool doAppendParameter_SetClearance(MAutEvent* pEvent);
	bool doAppendParameter_SetClearanceParam1(MAutEvent* pEvent);
	bool doAppendParameterAndSend_SetClearanceParam2(MAutEvent* pEvent);
	bool doCreateGuestAccessCtlCommand(MAutEvent* pEvent);
	bool doAppendParameterAndSend_GuestAccessCtlCommand(MAutEvent* pEvent);


};

#endif