#include "KeypadAutomaton.hpp"

#include "MainAutomaton.hpp"

#ifdef AUT_ACTION
#undef AUT_ACTION
#endif

#define AUT_ACTION(X) (bool (MAutomat::*)(MAutEvent*)) &KeypadAutomaton::X

#include<sstream>

const unsigned int deadlineTimerTimeout_s = 30;


// OWNER MainAutomatonEvent* parseMessageForMainAutomaton(DataMailboxMessage* pMessage);

OWNER KeypadAutomatonEvent* preprocessEvent(MAutEvent* pEvent);
OWNER KeypadAutomatonEvent* parseClearanceToEvent(KeypadAutomatonEvent* pEvent);


KeypadAutomatonEvent::KeypadAutomatonEvent(int idEvent, DataMailboxMessage* pMessage)
	: m_pMessage(pMessage)
{
	m_iEventID = idEvent;

	switch (m_iEventID)
	{
	case KeypadAutomaton::enuEvtInsufficientPermissions:
		m_sDesc = "enuEvtInsufficientPermissions";
		break;

	case KeypadAutomaton::enuEvtSuccess:
		m_sDesc = "enuEvtSuccess";
		break;

	case KeypadAutomaton::enuEvtSuccess_Guest:
		m_sDesc = "enuEvtSuccess_Guest";
		break;

	case KeypadAutomaton::enuEvtTimedOut:
		m_sDesc = "enuEvtTimedOut";
		break;

	case KeypadAutomaton::enuEvtError:
		m_sDesc = "enuEvtError";
		break;

	case KeypadAutomaton::enuEvtInvalidCommand:
		m_sDesc = "enuEvtInvalidCommand";
		break;

	case KeypadAutomaton::enuEvtInvalidParameter:
		m_sDesc = "enuEvtInvalidParameter";
		break;

	case KeypadAutomaton::enuEvtReceivedClearanceFromDB:
		m_sDesc = "enuEvtReceivedClearanceFromDB";
		break;

	case KeypadAutomaton::enuEvtReceivedAddRequest:
		m_sDesc = "enuEvtReceivedAddRequest";
		break;

	case KeypadAutomaton::enuEvtReceivedRemoveRequest:
		m_sDesc = "enuEvtReceivedRemoveRequest";
		break;

	case KeypadAutomaton::enuEvtReceiveAuthorizeRequest:
		m_sDesc = "enuEvtReceiveAuthorizeRequest";
		break;

	case KeypadAutomaton::enuEvtReceiveSetClearanceRequest:
		m_sDesc = "enuEvtReceiveSetClearanceRequest";
		break;

	case KeypadAutomaton::enuEvtReceivedRequest:
		m_sDesc = "enuEvtReceivedRequest";
		break;

	case KeypadAutomaton::enuEvtCancel:
		m_sDesc = "enuEvtCancel";
		break;

	case KeypadAutomaton::enuEvtEnter:
		m_sDesc = "enuEvtEnter";
		break;

	case KeypadAutomaton::enuEvtGuestAccessEnable:
		m_sDesc = "enuEvtGuestAccessEnable";
		break;

	case KeypadAutomaton::enuEvtGuestAccessDisable:
		m_sDesc = "enuEvtGuestAccessDisable";
		break;

	default:
		m_sDesc = "ERROR: Unknown Event!";
		break;
	}
}

KeypadAutomaton::KeypadAutomaton(short int siInstID,
	DataMailbox* pMailbox,
	MailboxReference* pRefDatabase,
	IndicatorController_Client* pIndicators,
	ILogger* pLogger)
	: MAutomat(siInstID),
	m_pMailbox(pMailbox),
	m_pRefDatabase(pRefDatabase),
	m_pIndicators(pIndicators),
	m_pLogger(pLogger),
	m_deadlineTimer("keypad.automaton.timer", pLogger),
	m_timeoutCallback(this, &KeypadAutomaton::doSendTimeoutEvent_timerWrapper)
{
	if (pLogger == nullptr)
		m_pLogger = NulLogger::getInstance();

	m_deadlineTimer.setTimeoutCallback(&m_timeoutCallback);
	m_deadlineTimer.setTimeout_s(deadlineTimerTimeout_s);


	/**********************************************
	* TRANSITION TABLE INITIALIZATION            *
	**********************************************/

	MAutState* pState = nullptr;

	// enuIdle
	pState = new MAutState(enuIdle);
		pState->addTransition(new MAutTransition(enuEvtReceivedRequest, enuWaitingDatabase_Request, this, AUT_ACTION(doSendRequestToDB)));
		pState->addTransition(new MAutTransition(enuEvtReceiveAuthorizeRequest, enuWaitingDatabase_Authorize, this, AUT_ACTION(doSendRequestToDB)));
		pState->addTransition(new MAutTransition(enuEvtGuestAccessEnable, enuWaitingDatabase_Authorize_GuestAccessEnable, this, AUT_ACTION(doSendRequestToDB)));
		pState->addTransition(new MAutTransition(enuEvtGuestAccessDisable, enuWaitingDatabase_Authorize_GuestAccessDisable, this, AUT_ACTION(doSendRequestToDB)));
		pState->addTransition(new MAutTransition(enuEvtCancel, enuIdle, this, AUT_ACTION(doVoid)));
	this->addAutState(pState);

	// enuWaitingDatabase_Request
	pState = new MAutState(enuWaitingDatabase_Request);
		pState->addTransition(new MAutTransition(enuEvtSuccess, enuIdle, this, AUT_ACTION(doSignalSuccess)));
		pState->addTransition(new MAutTransition(enuEvtError, enuIdle, this, AUT_ACTION(doSignalError)));
		pState->addTransition(new MAutTransition(enuEvtInvalidCommand, enuIdle, this, AUT_ACTION(doSignalInvalidCommand)));
		pState->addTransition(new MAutTransition(enuEvtInvalidParameter, enuIdle, this, AUT_ACTION(doSignalInvalidParameter)));
		pState->addTransition(new MAutTransition(enuEvtInsufficientPermissions, enuIdle, this, AUT_ACTION(doSignalInsufficientPermissions)));
		pState->addTransition(new MAutTransition(enuEvtTimedOut, enuIdle, this, AUT_ACTION(doSignalTimeout)));
		pState->addTransition(new MAutTransition(enuEvtReceivedRequest, enuWaitingDatabase_Request, this, AUT_ACTION(doSignalBusy)));
		pState->addTransition(new MAutTransition(enuEvtReceiveAuthorizeRequest, enuWaitingDatabase_Request, this, AUT_ACTION(doSignalBusy)));
		pState->addTransition(new MAutTransition(enuEvtCancel, enuWaitingDatabase_Request, this, AUT_ACTION(doCancelAction)));
	this->addAutState(pState);

	// enuWaitingDatabase_Authorize
	pState = new MAutState(enuWaitingDatabase_Authorize);
		pState->addTransition(new MAutTransition(enuEvtSuccess, enuIdle, this, AUT_ACTION(doOpenDoorsAndSignalSuccess)));
		pState->addTransition(new MAutTransition(enuEvtSuccess_Guest, enuIdle, this, AUT_ACTION(doOpenDoorsIfAuthorized)));
		pState->addTransition(new MAutTransition(enuEvtError, enuIdle, this, AUT_ACTION(doSignalError)));
		pState->addTransition(new MAutTransition(enuEvtInvalidCommand, enuIdle, this, AUT_ACTION(doSignalInvalidCommand)));
		pState->addTransition(new MAutTransition(enuEvtInvalidParameter, enuIdle, this, AUT_ACTION(doSignalInvalidParameter)));
		pState->addTransition(new MAutTransition(enuEvtInsufficientPermissions, enuIdle, this, AUT_ACTION(doSignalInsufficientPermissions)));
		pState->addTransition(new MAutTransition(enuEvtTimedOut, enuIdle, this, AUT_ACTION(doSignalTimeout)));
		pState->addTransition(new MAutTransition(enuEvtReceivedRequest, enuWaitingDatabase_Authorize, this, AUT_ACTION(doSignalBusy)));
		pState->addTransition(new MAutTransition(enuEvtReceiveAuthorizeRequest, enuWaitingDatabase_Authorize, this, AUT_ACTION(doSignalBusy)));
		pState->addTransition(new MAutTransition(enuEvtCancel, enuWaitingDatabase_Request, this, AUT_ACTION(doCancelAction)));
	this->addAutState(pState);

	// enuWaitingDatabase_Authorize_GuestAccessEnable
	pState = new MAutState(enuWaitingDatabase_Authorize_GuestAccessEnable);
		pState->addTransition(new MAutTransition(enuEvtSuccess, enuIdle, this, AUT_ACTION(doEnableGuestAccess)));
		pState->addTransition(new MAutTransition(enuEvtSuccess_Guest, enuIdle, this, AUT_ACTION(doEnableGuestAccess)));
		pState->addTransition(new MAutTransition(enuEvtError, enuIdle, this, AUT_ACTION(doSignalError)));
		pState->addTransition(new MAutTransition(enuEvtInvalidCommand, enuIdle, this, AUT_ACTION(doSignalInvalidCommand)));
		pState->addTransition(new MAutTransition(enuEvtInvalidParameter, enuIdle, this, AUT_ACTION(doSignalInvalidParameter)));
		pState->addTransition(new MAutTransition(enuEvtInsufficientPermissions, enuIdle, this, AUT_ACTION(doSignalInsufficientPermissions)));
		pState->addTransition(new MAutTransition(enuEvtTimedOut, enuIdle, this, AUT_ACTION(doSignalTimeout)));
		pState->addTransition(new MAutTransition(enuEvtReceivedRequest, enuWaitingDatabase_Authorize, this, AUT_ACTION(doSignalBusy)));
		pState->addTransition(new MAutTransition(enuEvtReceiveAuthorizeRequest, enuWaitingDatabase_Authorize, this, AUT_ACTION(doSignalBusy)));
		pState->addTransition(new MAutTransition(enuEvtCancel, enuWaitingDatabase_Request, this, AUT_ACTION(doCancelAction)));
	this->addAutState(pState);

	// enuWaitingDatabase_Authorize_GuestAccessDisable
	pState = new MAutState(enuWaitingDatabase_Authorize_GuestAccessDisable);
		pState->addTransition(new MAutTransition(enuEvtSuccess, enuIdle, this, AUT_ACTION(doDisableGuestAccess)));
		pState->addTransition(new MAutTransition(enuEvtSuccess_Guest, enuIdle, this, AUT_ACTION(doDisableGuestAccess)));
		pState->addTransition(new MAutTransition(enuEvtError, enuIdle, this, AUT_ACTION(doSignalError)));
		pState->addTransition(new MAutTransition(enuEvtInvalidCommand, enuIdle, this, AUT_ACTION(doSignalInvalidCommand)));
		pState->addTransition(new MAutTransition(enuEvtInvalidParameter, enuIdle, this, AUT_ACTION(doSignalInvalidParameter)));
		pState->addTransition(new MAutTransition(enuEvtInsufficientPermissions, enuIdle, this, AUT_ACTION(doSignalInsufficientPermissions)));
		pState->addTransition(new MAutTransition(enuEvtTimedOut, enuIdle, this, AUT_ACTION(doSignalTimeout)));
		pState->addTransition(new MAutTransition(enuEvtReceivedRequest, enuWaitingDatabase_Authorize, this, AUT_ACTION(doSignalBusy)));
		pState->addTransition(new MAutTransition(enuEvtReceiveAuthorizeRequest, enuWaitingDatabase_Authorize, this, AUT_ACTION(doSignalBusy)));
		pState->addTransition(new MAutTransition(enuEvtCancel, enuWaitingDatabase_Request, this, AUT_ACTION(doCancelAction)));
	this->addAutState(pState);

	m_iInitialStateID = enuIdle;
}

void KeypadAutomaton::setMainAutomatonPointer(MainAutomaton* pMainAutomaton)
{
	m_pMainAutomaton = pMainAutomaton;
}

bool KeypadAutomaton::initialize()
{
	if (m_pMainAutomaton == nullptr)
	{
		*m_pLogger << "KeypadAutomaton: MainAutomaton not found (nullptr) in ctor.";
		Kernel::Fatal_Error("KeypadAutomaton: MainAutomaton not found (nullptr) in ctor.");
	}

	if (m_pMailbox == nullptr)
	{
		*m_pLogger << "KeypadAutomaton: Mailbox not found (nullptr) in ctor.";
		Kernel::Fatal_Error("KeypadAutomaton: Mailbox not found (nullptr) in ctor.");
	}

	if (m_pRefDatabase == nullptr)
	{
		*m_pLogger << "KeypadAutomaton: Mailbox Reference to Database not found (nullptr) in ctor.";
		Kernel::Fatal_Error("KeypadAutomaton: Mailbox Reference to Database not found (nullptr) in ctor.");
	}

	if (m_pIndicators == nullptr)
	{
		*m_pLogger << "KeypadAutomaton: Indicator Controller not found (nullptr) in ctor.";
		Kernel::Fatal_Error("KeypadAutomaton: Indicator Controller  not found (nullptr) in ctor.");
	}

	return this->test();
}

bool KeypadAutomaton::processEvent(MAutEvent* pEvent)
{
	KeypadAutomatonEvent* pPreProcessedEvent = preprocessEvent(pEvent);

	// Save the event before calling processEvent because it frees the event object.
	TransitionInfo info = getTransitionInfo(pPreProcessedEvent);

	*m_pLogger << info.createLogEntry_Start();

	bool status = MAutomat::processEvent(pPreProcessedEvent);

	*m_pLogger << info.createLogEntry_End();

	return status;
}

bool KeypadAutomaton::reset()
{
	m_iCurrentStateID = m_iInitialStateID;

	std::stringstream logStringBuilder;

	logStringBuilder
		<< "\n***********************RESET**************************"
		<< "\nAUTOMATON RESET " << getAutomatonName()
		<< "\nSTATE: " << getCurrentStateId()
		<< "\n******************************************************";

	*m_pLogger << logStringBuilder.str();

	return true;
}

// AUTOMATON FUNCTIONS =========================================

bool KeypadAutomaton::doVoid(MAutEvent* pEvent)
{
	// putToLCD_wTimeout("doVoid");
	return true;
}

bool KeypadAutomaton::doSignalTimeout(MAutEvent* pEvent)
{
	m_deadlineTimer.Stop(); // TODO EXTRA?
	m_pIndicators->BuzzerFailure();
	signalFinishToMainAutomaton_wTempMessage("Timed out!");
	return true;
}

bool KeypadAutomaton::doOpenDoors(MAutEvent* pEvent)
{
	m_deadlineTimer.Stop();

	m_pIndicators->LCD_Clear_wDefaultMsg();
	// m_pIndicators->LCD_Put_wTimeout("Doors Open!");

	m_pIndicators->OpenDoor_wBuzzerSuccess();
	signalFinishToMainAutomaton_wTempMessage("Doors Open!");
	
	return true;
}

bool KeypadAutomaton::doSendRequestToDB(MAutEvent* pEvent)
{
	*m_pLogger << "### ENTRY";

	KeypadAutomatonEvent* pDatabaseRequest = dynamic_cast<KeypadAutomatonEvent*>(pEvent);
	if (pDatabaseRequest == nullptr)
	{
		reset();
		return false;
	}

	*m_pLogger << "### Before putting to LCD";

	m_pIndicators->LCD_Put_Permanently("Request sent...");

	*m_pLogger << "### After putting to LCD";
	*m_pLogger << "### Before sending to database";

	m_pMailbox->send(*m_pRefDatabase, pDatabaseRequest->m_pMessage);

	*m_pLogger << "### After sending to database";

	m_deadlineTimer.Start();

	*m_pLogger << "### END";

	return true;
}

bool KeypadAutomaton::doSignalSuccess(MAutEvent* pEvent)
{
	m_deadlineTimer.Stop();
	m_pIndicators->BuzzerSuccess();
	signalFinishToMainAutomaton_wTempMessage("Success!");
	return true;
}

bool KeypadAutomaton::doOpenDoorsAndSignalSuccess(MAutEvent* pEvent)
{
	doOpenDoors(pEvent);
	return true;
}

bool KeypadAutomaton::doSignalError(MAutEvent* pEvent)
{
	m_deadlineTimer.Stop();
	m_pIndicators->BuzzerFailure();
	signalFinishToMainAutomaton_wTempMessage("Error!");
	return true;
}

bool KeypadAutomaton::doSignalInvalidCommand(MAutEvent* pEvent)
{
	m_deadlineTimer.Stop();
	m_pIndicators->BuzzerFailure();
	signalFinishToMainAutomaton_wTempMessage("Invalid command!");
	return true;
}

bool KeypadAutomaton::doSignalInvalidParameter(MAutEvent* pEvent)
{
	m_deadlineTimer.Stop();
	m_pIndicators->BuzzerFailure();
	signalFinishToMainAutomaton_wTempMessage("Invalid parameter!");
	return true;
}

bool KeypadAutomaton::doSignalInsufficientPermissions(MAutEvent* pEvent)
{
	m_deadlineTimer.Stop();
	m_pIndicators->BuzzerFailure();
	signalFinishToMainAutomaton_wTempMessage("Insufficient Permissions!");
	return true;
}

bool KeypadAutomaton::doSignalBusy(MAutEvent* pEvent)
{
	m_pIndicators->BuzzerPing();
	m_pIndicators->LCD_Put_wTimeout("Device busy...");

	return true;
}

bool KeypadAutomaton::doCancelAction(MAutEvent* pEvent)
{
	m_pIndicators->BuzzerFailure();
	signalFinishToMainAutomaton_wTempMessage("Canceled action!");
	return true;
}

bool KeypadAutomaton::doSignalGuestAccessDenied(MAutEvent* pEvent)
{
	m_pIndicators->BuzzerFailure();
	signalFinishToMainAutomaton_wTempMessage("Access disabled for guests!");
	return true;
}

bool KeypadAutomaton::doEnableGuestAccess(MAutEvent* pEvent)
{
	m_pMainAutomaton->m_bGuestAccessEnable = true;

	signalFinishToMainAutomaton_wTempMessage("Guest Access Enabled!");

	return true;
}

bool KeypadAutomaton::doDisableGuestAccess(MAutEvent* pEvent)
{
	m_pMainAutomaton->m_bGuestAccessEnable = false;

	signalFinishToMainAutomaton_wTempMessage("Guest Access Disabled!");

	return true;
}

bool KeypadAutomaton::doOpenDoorsIfAuthorized(MAutomat* pEvent)
{
	if (m_pMainAutomaton->m_bGuestAccessEnable == true)
	{
		doOpenDoorsAndSignalSuccess(nullptr);
		return true;
	}

	doSignalGuestAccessDenied(nullptr);
	return true;
}

// ==========================================================================

void KeypadAutomaton::signalFinishToMainAutomaton_wTempMessage(const std::string& tempMessageToDisplay)
{
	CommandMessage* pTempMessage = new CommandMessage(CommandMessage::enuCommand::NONE);
	pTempMessage->addParameter({ InputParameter::enuType::LCD_Message_wTimeout, tempMessageToDisplay });

	MainAutomatonEvent* pEvent = new MainAutomatonEvent(MainAutomaton::enuEvtKeypadAutomatonFinished, pTempMessage);
	m_pMainAutomaton->processEvent(pEvent);
}

void KeypadAutomaton::signalFinishToMainAutomaton()
{
	MainAutomatonEvent* pEvent = new MainAutomatonEvent(MainAutomaton::enuEvtKeypadAutomatonFinished, nullptr);
	m_pMainAutomaton->processEvent(pEvent);
}


void KeypadAutomaton::doSendTimeoutEvent_timerWrapper(void*)
{
	processEvent(new KeypadAutomatonEvent(enuEvtTimedOut, nullptr));
}

TransitionInfo KeypadAutomaton::getTransitionInfo(MAutEvent* pReceivedEvent)
{
	if (pReceivedEvent == nullptr)
	{
		return {};
	}

	TransitionInfo info;
	info.m_automatonName = "KeypadAutomaton";
	info.m_stateId = getCurrentStateId();
	info.m_eventId = pReceivedEvent->getEventId();
	info.m_eventDesc = pReceivedEvent->getEventDesc();

	return info;
}

OWNER KeypadAutomatonEvent* preprocessEvent(MAutEvent* pEvent)
{
	KeypadAutomatonEvent* pParsedEvent = dynamic_cast<KeypadAutomatonEvent*>(pEvent);

	if (pParsedEvent == nullptr)
	{
		Kernel::Warning("KeypadAutomaton event preprocessor -> parsed event pointer is null!");
		delete pEvent;
		return nullptr;
	}

	else if (pParsedEvent->getEventId() == KeypadAutomaton::enuEvtReceivedClearanceFromDB)
	{
		// *********** WRONG - WARNING
		// Deleting pEvent and then passing it (in the form of the
		// pParsedEvent which is just dynamically casted pEvent)
		// to parseClearanceToEvent(...) leads to using free'd memory !!!!

				// ORIGINAL CODE:
				// delete pEvent;
				// return parseClearanceToEvent(pParsedEvent);

		// ********************************

		// ********** CORRECT WAY
		KeypadAutomatonEvent* pNewTempEvent = parseClearanceToEvent(pParsedEvent);

		// NOTE: parseClearanceEvent(...) MUST NOT return pParsedEventBack !!!
		if (pNewTempEvent == pParsedEvent)
		{
			Kernel::Fatal_Error("CODING ERROR, CHECK COMMENTS AT: " + std::string(__FILE__) + " ON LINE: " + std::to_string(__LINE__));
		}
		delete pParsedEvent;
		return pNewTempEvent;
	}

	/*
	else if (pParsedEvent->getEventId() == KeypadAutomaton::enuEvtGuestAccessEnable ||
		pParsedEvent->getEventId() == KeypadAutomaton::enuEvtGuestAccessDisable)
	{ 
		// Required because automaton just forwards m_pMessage to Database as request
		// Since CommandMessage::GUEST_ACCESS_* types are invalid requests
		// They are changed to CommandMessage::AUTHENTICATE

		// TODO extract to a new method

		CommandMessage* pNewMessage = new CommandMessage(CommandMessage::enuCommand::AUTHENTICATE);
		CommandMessage* pOldMessage = dynamic_cast<CommandMessage*>(pParsedEvent->m_pMessage);

		pNewMessage->addParameter(pOldMessage->getParameterAt(0));

		// Gets deleted in MainApplication where it is constructed!!!!! TODO
		// delete pOldMessage;

		pParsedEvent->m_pMessage = pNewMessage;
		return pParsedEvent;
	}
	*/

	return pParsedEvent;
}

OWNER KeypadAutomatonEvent* parseClearanceToEvent(KeypadAutomatonEvent* pEvent)
{
	if (pEvent == nullptr)
	{
		return nullptr;
	}

	if (pEvent->m_pMessage->getDataType() != MessageDataType::enuType::DatabaseReply)
	{
		return nullptr;
	}

	DatabaseReply* pReplyMessage = dynamic_cast<DatabaseReply*>(pEvent->m_pMessage);
	if (pReplyMessage == nullptr)
	{
		return nullptr;
	}

	if (pReplyMessage->getReplyStatus() != DatabaseReply::enuStatus::CLEARANCE)
	{
		return nullptr;
	}

	// TODO
	if (pReplyMessage->getClearance() > 0)
	{
		return new KeypadAutomatonEvent(KeypadAutomaton::enuEvtSuccess, pReplyMessage);
	}
	else if(pReplyMessage->getClearance() == 0)
	{
		return new KeypadAutomatonEvent(KeypadAutomaton::enuEvtSuccess_Guest, pReplyMessage);
	}
	else
	{
		return new KeypadAutomatonEvent(KeypadAutomaton::enuEvtInsufficientPermissions, pReplyMessage);
	}


}



