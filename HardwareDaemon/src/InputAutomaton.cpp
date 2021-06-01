#include "InputAutomaton.hpp"

#include <sstream>

#define AUT_ACTION(X) (bool (MAutomat::*)(MAutEvent*)) (&InputAutomaton::X)

#include<iostream>
#define MARK(X) std::cout << "MARK: " << X << std::endl;

InputAutomatonEvent::InputAutomatonEvent(int idEvent, const InputParameter& parameter)
	: m_parameter(parameter)
{
	init(idEvent);
}

InputAutomatonEvent::InputAutomatonEvent(int idEvent)
	: m_parameter()
{
	init(idEvent);
}

void InputAutomatonEvent::init(int idEvent)
{
	m_iEventID = idEvent;

	switch (m_iEventID)
	{
	case InputAutomaton::enuEvtPlain:
		m_sDesc = "enuEvtPlain";
		break;

	case InputAutomaton::enuEvtEnter:
		m_sDesc = "enuEvtEnter";
		break;

	case InputAutomaton::enuEvtCancel:
		m_sDesc = "enuEvtCancel";
		break;

	case InputAutomaton::enuEvtKeypadPIN:
		m_sDesc = "enuEvtKeypadPIN";
		break;

	case InputAutomaton::enuEvtKeypadCommand:
		m_sDesc = "enuEvtKeypadCommand";
		break;

	case InputAutomaton::enuEvtRFIDCard:
		m_sDesc = "enuEvtRFIDCard";
		break;

	case InputAutomaton::enuEvtAddCommand:
		m_sDesc = "enuEvtAddCommand";
		break;

	case InputAutomaton::enuEvtRemoveCommand:
		m_sDesc = "enuEvtRemoveCommand";
		break;

	case InputAutomaton::enuEvtSetClearanceCommand:
		m_sDesc = "enuEvtSetClearanceCommand";
		break;

	case InputAutomaton::enuEvtGuestAccessEnable:
		m_sDesc = "enuEvtGuestAccessEnable";
		break;

	case InputAutomaton::enuEvtGuestAccessDisable:
		m_sDesc = "enuEvtGuestAccessDisable";
		break;

	default:
		m_sDesc = "INVALID EVENT";
		break;
	}
}

std::string InputAutomaton::getStateName(enumAutStateType state)
{
	switch (state)
	{
	case enuIdle:
		return "enuIdle";

	case enuAdd:
		return "enuAdd";

	case enuAddParam1:
		return "enuAddParam1";

	case enuAddParam2:
		return "enuAddParam2";

	case enuAddParam3:
		return "enuAddParam3";

	case enuRemove:
		return "enuRemove";

	case enuRemoveParam1:
		return "enuRemoveParam1";

	case enuSetClearance:
		return "enuSetClearance";

	case enuSetClearanceParam1:
		return "enuSetClearanceParam1";

	case enuSetClearanceParam2:
		return "enuSetClearanceParam2";

	case enuGuestAccessControlParam1:
		return "enuGuestAccessControlParam1";

	default:
		return "INVALID STATE";
	}
}

void InputAutomaton::FatalError(const std::string& message)
{
	*m_pLogger << message;
	Kernel::Fatal_Error(message);
}

void InputAutomaton::Warning(const std::string& message)
{
	*m_pLogger << message;
	Kernel::Warning(message);
}

InputAutomaton::InputAutomaton(int siInstID,
	DataMailbox* pMailbox,
	MailboxReference* pMainApplication,
	IndicatorController_Client* pIndicators,
	ILogger* pLogger)
	:	MAutomat(siInstID),
	m_pMailbox(pMailbox),
	m_pMainApplication(pMainApplication),
	m_pIndicators(pIndicators),
	m_pLogger(pLogger)
{

	checkResourcePointers();

	m_iInitialStateID = enumAutStateType::enuIdle;

	/**********************************************
	* TRANSITION TABLE INITIALIZATION            *
	**********************************************/

	MAutState* pState = nullptr;

	pState = new MAutState(enuIdle);
	pState->addTransition(new MAutTransition(enuEvtAddCommand, enuAdd, this, AUT_ACTION(doCreateCommand_Add)));
	pState->addTransition(new MAutTransition(enuEvtRemoveCommand, enuRemove, this, AUT_ACTION(doCreateCommand_Remove)));
	pState->addTransition(new MAutTransition(enuEvtSetClearanceCommand, enuSetClearance, this, AUT_ACTION(doCreateCommand_SetClearance)));
	pState->addTransition(new MAutTransition(enuEvtKeypadPIN, enuIdle, this, AUT_ACTION(doAppendParameterAndSend_Idle)));
	pState->addTransition(new MAutTransition(enuEvtRFIDCard, enuIdle, this, AUT_ACTION(doAppendParameterAndSend_Idle)));
	pState->addTransition(new MAutTransition(enuEvtEnter, enuIdle, this, AUT_ACTION(doAppendParameterAndSend)));
	pState->addTransition(new MAutTransition(enuEvtCancel, enuIdle, this, AUT_ACTION(doAppendParameterAndSend)));
	pState->addTransition(new MAutTransition(enuEvtPlain, enuIdle, this, AUT_ACTION(doWarnWrongInput)));
	pState->addTransition(new MAutTransition(enuEvtGuestAccessEnable, enuGuestAccessControlParam1, this, AUT_ACTION(doCreateGuestAccessCtlCommand)));
	pState->addTransition(new MAutTransition(enuEvtGuestAccessDisable, enuGuestAccessControlParam1, this, AUT_ACTION(doCreateGuestAccessCtlCommand)));
	this->addAutState(pState);

	pState = new MAutState(enuAdd);
	pState->addTransition(new MAutTransition(enuEvtCancel, enuIdle, this, AUT_ACTION(doCancel)));
	pState->addTransition(new MAutTransition(enuEvtKeypadPIN, enuAddParam1, this, AUT_ACTION(doAppendParameter_Add)));
	pState->addTransition(new MAutTransition(enuEvtRFIDCard, enuAddParam1, this, AUT_ACTION(doAppendParameter_Add)));
	pState->addTransition(new MAutTransition(enuEvtEnter, enuIdle, this, AUT_ACTION(doWarnWrongInput)));
	pState->addTransition(new MAutTransition(enuEvtPlain, enuIdle, this, AUT_ACTION(doWarnWrongInput)));
	this->addAutState(pState);

	pState = new MAutState(enuAddParam1);
	pState->addTransition(new MAutTransition(enuEvtCancel, enuIdle, this, AUT_ACTION(doCancel)));
	pState->addTransition(new MAutTransition(enuEvtKeypadPIN, enuAddParam2, this, AUT_ACTION(doAppendParameter_AddParam1)));
	pState->addTransition(new MAutTransition(enuEvtRFIDCard, enuAddParam2, this, AUT_ACTION(doAppendParameter_AddParam1)));
	pState->addTransition(new MAutTransition(enuEvtEnter, enuIdle, this, AUT_ACTION(doWarnWrongInput)));
	pState->addTransition(new MAutTransition(enuEvtPlain, enuIdle, this, AUT_ACTION(doWarnWrongInput)));
	this->addAutState(pState);

	pState = new MAutState(enuAddParam2);
	pState->addTransition(new MAutTransition(enuEvtCancel, enuIdle, this, AUT_ACTION(doCancel)));
	pState->addTransition(new MAutTransition(enuEvtKeypadPIN, enuIdle, this, AUT_ACTION(doWarnWrongInput)));
	pState->addTransition(new MAutTransition(enuEvtRFIDCard, enuIdle, this, AUT_ACTION(doWarnWrongInput)));
	pState->addTransition(new MAutTransition(enuEvtPlain, enuIdle, this, AUT_ACTION(doAppendParameterAndSend_AddParam2)));
	pState->addTransition(new MAutTransition(enuEvtEnter, enuIdle, this, AUT_ACTION(doSend_AddParam2)));
	this->addAutState(pState);

	pState = new MAutState(enuRemove);
	pState->addTransition(new MAutTransition(enuEvtCancel, enuIdle, this, AUT_ACTION(doCancel)));
	pState->addTransition(new MAutTransition(enuEvtEnter, enuIdle, this, AUT_ACTION(doWarnWrongInput)));
	pState->addTransition(new MAutTransition(enuEvtKeypadPIN, enuRemoveParam1, this, AUT_ACTION(doAppendParameter_Remove)));
	pState->addTransition(new MAutTransition(enuEvtRFIDCard, enuRemoveParam1, this, AUT_ACTION(doAppendParameter_Remove)));
	pState->addTransition(new MAutTransition(enuEvtPlain, enuIdle, this, AUT_ACTION(doWarnWrongInput)));
	this->addAutState(pState);

	pState = new MAutState(enuRemoveParam1);
	pState->addTransition(new MAutTransition(enuEvtCancel, enuIdle, this, AUT_ACTION(doCancel)));
	pState->addTransition(new MAutTransition(enuEvtEnter, enuIdle, this, AUT_ACTION(doWarnWrongInput)));
	pState->addTransition(new MAutTransition(enuEvtKeypadPIN, enuIdle, this, AUT_ACTION(doAppendParameterAndSend_RemoveParam1)));
	pState->addTransition(new MAutTransition(enuEvtRFIDCard, enuIdle, this, AUT_ACTION(doAppendParameterAndSend_RemoveParam1)));
	pState->addTransition(new MAutTransition(enuEvtPlain, enuIdle, this, AUT_ACTION(doWarnWrongInput)));
	this->addAutState(pState);

	pState = new MAutState(enuSetClearance);
	pState->addTransition(new MAutTransition(enuEvtCancel, enuIdle, this, AUT_ACTION(doCancel)));
	pState->addTransition(new MAutTransition(enuEvtEnter, enuIdle, this, AUT_ACTION(doWarnWrongInput)));
	pState->addTransition(new MAutTransition(enuEvtKeypadPIN, enuSetClearanceParam1, this, AUT_ACTION(doAppendParameter_SetClearance)));
	pState->addTransition(new MAutTransition(enuEvtRFIDCard, enuSetClearanceParam1, this, AUT_ACTION(doAppendParameter_SetClearance)));
	pState->addTransition(new MAutTransition(enuEvtPlain, enuIdle, this, AUT_ACTION(doWarnWrongInput)));
	this->addAutState(pState);

	pState = new MAutState(enuSetClearanceParam1);
	pState->addTransition(new MAutTransition(enuEvtCancel, enuIdle, this, AUT_ACTION(doCancel)));
	pState->addTransition(new MAutTransition(enuEvtEnter, enuIdle, this, AUT_ACTION(doWarnWrongInput)));
	pState->addTransition(new MAutTransition(enuEvtKeypadPIN, enuIdle, this, AUT_ACTION(doWarnWrongInput)));
	pState->addTransition(new MAutTransition(enuEvtRFIDCard, enuIdle, this, AUT_ACTION(doWarnWrongInput)));
	pState->addTransition(new MAutTransition(enuEvtPlain, enuSetClearanceParam2, this, AUT_ACTION(doAppendParameter_SetClearanceParam1)));
	this->addAutState(pState);

	pState = new MAutState(enuSetClearanceParam2);
	pState->addTransition(new MAutTransition(enuEvtCancel, enuIdle, this, AUT_ACTION(doCancel)));
	pState->addTransition(new MAutTransition(enuEvtEnter, enuIdle, this, AUT_ACTION(doWarnWrongInput)));
	pState->addTransition(new MAutTransition(enuEvtKeypadPIN, enuIdle, this, AUT_ACTION(doAppendParameterAndSend_SetClearanceParam2)));
	pState->addTransition(new MAutTransition(enuEvtRFIDCard, enuIdle, this, AUT_ACTION(doAppendParameterAndSend_SetClearanceParam2)));
	pState->addTransition(new MAutTransition(enuEvtPlain, enuIdle, this, AUT_ACTION(doWarnWrongInput)));
	this->addAutState(pState);

	pState = new MAutState(enuGuestAccessControlParam1);
	pState->addTransition(new MAutTransition(enuEvtCancel, enuIdle, this, AUT_ACTION(doCancel)));
	pState->addTransition(new MAutTransition(enuEvtKeypadPIN, enuIdle, this, AUT_ACTION(doAppendParameterAndSend_GuestAccessCtlCommand)));
	pState->addTransition(new MAutTransition(enuEvtRFIDCard, enuIdle, this, AUT_ACTION(doAppendParameterAndSend_GuestAccessCtlCommand)));
	pState->addTransition(new MAutTransition(enuEvtEnter, enuIdle, this, AUT_ACTION(doWarnWrongInput)));
	pState->addTransition(new MAutTransition(enuEvtPlain, enuIdle, this, AUT_ACTION(doWarnWrongInput)));
	this->addAutState(pState);

}

void InputAutomaton::checkResourcePointers()
{
	if (m_pLogger == nullptr)
	{
		m_pLogger = NulLogger::getInstance();
	}

	if (m_pMailbox == nullptr)
	{
		FatalError("InputAutomaton: pMailbox cannot be nullptr!");
	}

	if (m_pMainApplication == nullptr)
	{
		FatalError("InputAutomaton: pMainApplication MailboxReference cannot be nullptr!");
	}

	if (m_pIndicators == nullptr)
	{
		FatalError("InputAutomaton: Indicator Controller cannot be nullptr!");
	}

}

bool InputAutomaton::initialize()
{
	return this->test();
}

void InputAutomaton::logTransitionStart(MAutEvent* event)
{
	int currentStateId = getCurrentStateId();
	int currentEventId = getCurrentEventId();
	InputAutomatonEvent currentEventRecreation(currentEventId);

	enumAutStateType currentState = static_cast<enumAutStateType>(currentStateId);

	std::stringstream logStringBuilder;

	logStringBuilder << "*****START****"
		<< " STATE: " << getStateName(currentState) << " [ " << currentStateId << " ] "
		<< " EVENT: " << currentEventRecreation.getEventDesc() << " "
		<< "*******";

	*m_pLogger << logStringBuilder.str();
}

void InputAutomaton::logTransitionEnd(MAutEvent* event)
{
	int currentStateId = getCurrentStateId();
	int currentEventId = getCurrentEventId();
	InputAutomatonEvent currentEventRecreation(currentEventId);

	enumAutStateType currentState = static_cast<enumAutStateType>(currentStateId);

	std::stringstream logStringBuilder;

	logStringBuilder << "*****END****"
		<< " STATE: " << getStateName(currentState) << " [ " << currentStateId << " ] "
		<< " EVENT: " << currentEventRecreation.getEventDesc() << " "
		<< "*******";

	*m_pLogger << logStringBuilder.str();
}

bool InputAutomaton::processEvent(MAutEvent* event)
{
	logTransitionStart(event);

	bool result = MAutomat::processEvent(event);

	logTransitionEnd(event);

	return result;

}


bool InputAutomaton::reset()
{
	m_iCurrentStateID = m_iInitialStateID;

	std::stringstream logStringBuilder;

	int currentStateId = getCurrentStateId();
	enumAutStateType currentState = static_cast<enumAutStateType>(currentStateId);

	logStringBuilder << "\n******************************************************"
		<< "\n******************************************************"
		<< "\nAUTOMATON RESET"
		<< "\nSTATE: " << getStateName(currentState) << " [ " << currentStateId << " ]"
		<< "\n******************************************************"
		<< "\n******************************************************";

	*m_pLogger << logStringBuilder.str();

	return true;
}

void InputAutomaton::clearMessageBuffer()
{
	m_messageBuffer = std::move(CommandMessage());
}

void InputAutomaton::appendParameterToMessageBuffer(const InputParameter& param)
{
	m_messageBuffer.addParameter(param);
}

void InputAutomaton::doCreateNewCommandMessage(CommandMessage::enuCommand command)
{
	m_messageBuffer = std::move(CommandMessage(command));
}

// ========================================================================

bool InputAutomaton::doWarnWrongInput(MAutEvent* pEvent)
{
	m_pIndicators->LCD_Clear_wDefaultMsg();
	m_pIndicators->LCD_Put_wTimeout("Invalid input! Restarting...");
	m_pIndicators->BuzzerFailure();
	clearMessageBuffer();
	return true;
}

bool InputAutomaton::doAppendParameter(MAutEvent* pEvent)
{
	InputAutomatonEvent* pParsedEvent = dynamic_cast<InputAutomatonEvent*>(pEvent);
	if (pParsedEvent == nullptr)
	{
		FatalError("dynamic_cast-ing an event(MAutEvent* to InputAutomatonEvent*) results in nullptr. doAppendParameter()");
	}

	m_pIndicators->BuzzerPing();

	appendParameterToMessageBuffer(pParsedEvent->m_parameter);

	return true;
}

bool InputAutomaton::doCancel(MAutEvent* pEvent)
{
	clearMessageBuffer();

	//putToLCD_wTimeout("Command canceled...");

	m_pIndicators->LCD_Clear_wDefaultMsg();

	m_pIndicators->BuzzerFailure();

	return true;
}

bool InputAutomaton::doAppendParameterAndSend(MAutEvent* pEvent)
{
	doAppendParameter(pEvent);
	doSend(nullptr);


	return true;
}

bool InputAutomaton::doSend(MAutEvent* pEvent)
{
	m_pMailbox->send(*m_pMainApplication, &m_messageBuffer);

	clearMessageBuffer();

	return true;
}

bool InputAutomaton::doSend_wBuzzerPing(MAutEvent* pEvent)
{
	m_pIndicators->BuzzerPing();

	doSend(pEvent);

	return true;
}

bool InputAutomaton::doVoid(MAutEvent* pEvent)
{


	return true;
}

bool InputAutomaton::doCreateCommand_Add(MAutEvent* pEvent)
{
	doCreateNewCommandMessage(CommandMessage::enuCommand::ADD);
	m_pIndicators->LCD_Put_Permanently("ADD: PIN/Card to Add:");
	m_pIndicators->BuzzerPing();
	return true;
}

bool InputAutomaton::doCreateCommand_Remove(MAutEvent* pEvent)
{
	doCreateNewCommandMessage(CommandMessage::enuCommand::REMOVE);
	m_pIndicators->LCD_Put_Permanently("REM: PIN/Card to Remove:");
	m_pIndicators->BuzzerPing();
	return true;
}

bool InputAutomaton::doCreateCommand_SetClearance(MAutEvent* pEvent)
{
	doCreateNewCommandMessage(CommandMessage::enuCommand::SET_CLNC);
	m_pIndicators->LCD_Put_Permanently("SETCL: PIN/Card to Set clearance of:");
	m_pIndicators->BuzzerPing();
	return true;
}

bool InputAutomaton::doAppendParameterAndSend_Idle(MAutEvent* pEvent)
{
	doCreateNewCommandMessage(CommandMessage::enuCommand::AUTHENTICATE);
	doAppendParameterAndSend(pEvent);
	m_pIndicators->LCD_Clear_wDefaultMsg();
	return true;
}

bool InputAutomaton::doAppendParameter_Add(MAutEvent* pEvent)
{
	doAppendParameter(pEvent);
	m_pIndicators->LCD_Put_Permanently("ADD: PIN/Card for authorization:");
	return true;
}

bool InputAutomaton::doAppendParameter_AddParam1(MAutEvent* pEvent)
{
	doAppendParameter(pEvent);
	m_pIndicators->LCD_Put_Permanently("ADD: [Optional] clearance (ENTER to skip)");
	return true;
}

bool InputAutomaton::doAppendParameterAndSend_AddParam2(MAutEvent* pEvent)
{
	doAppendParameterAndSend(pEvent);
	m_pIndicators->LCD_Clear_wDefaultMsg();
	return true;
}

bool InputAutomaton::doSend_AddParam2(MAutEvent* pEvent)
{
	doSend_wBuzzerPing(pEvent);
	m_pIndicators->LCD_Clear_wDefaultMsg();
	return true;
}

bool InputAutomaton::doAppendParameter_Remove(MAutEvent* pEvent)
{
	doAppendParameter(pEvent);
	m_pIndicators->LCD_Put_Permanently("REM: PIN/Card for authorization:");
	return true;
}

bool InputAutomaton::doAppendParameterAndSend_RemoveParam1(MAutEvent* pEvent)
{
	doAppendParameterAndSend(pEvent);
	m_pIndicators->LCD_Clear_wDefaultMsg();
	return true;
}

bool InputAutomaton::doAppendParameter_SetClearance(MAutEvent* pEvent)
{
	doAppendParameter(pEvent);
	m_pIndicators->LCD_Put_Permanently("SETCL: New clearance:");
	return true;
}

bool InputAutomaton::doAppendParameter_SetClearanceParam1(MAutEvent* pEvent)
{
	doAppendParameter(pEvent);
	m_pIndicators->LCD_Put_Permanently("SETCL: PIN/Card for authorization:");
	return true;
}

bool InputAutomaton::doAppendParameterAndSend_SetClearanceParam2(MAutEvent* pEvent)
{
	doAppendParameterAndSend(pEvent);
	m_pIndicators->LCD_Clear_wDefaultMsg();
	return true;
}

bool InputAutomaton::doCreateGuestAccessCtlCommand(MAutEvent* pEvent)
{
	InputAutomatonEvent* pParsedEvent = dynamic_cast<InputAutomatonEvent*>(pEvent);
	if (pParsedEvent == nullptr)
	{
		Kernel::Fatal_Error("doCreateGuestAccessCtlCommand() parsed event is nullptr!");
	}

	if (pParsedEvent->getEventId() == enuEvtGuestAccessEnable)
	{
		doCreateNewCommandMessage(CommandMessage::enuCommand::GUEST_ACCESS_ENABLE);
		m_pIndicators->LCD_Put_Permanently("Authorize GuestAccessEnable:");
	}
	else if (pParsedEvent->getEventId() == enuEvtGuestAccessDisable)
	{
		doCreateNewCommandMessage(CommandMessage::enuCommand::GUEST_ACCESS_DISABLE);
		m_pIndicators->LCD_Put_Permanently("Authorize GuestAccessDisable:");
	}
	else
	{
		Kernel::Fatal_Error("doCreateGuestAccessCtlCommand() pParsedEvent invalid event type!");
	}

	m_pIndicators->BuzzerPing();

	return true;
}

bool InputAutomaton::doAppendParameterAndSend_GuestAccessCtlCommand(MAutEvent* pEvent)
{
	doAppendParameterAndSend(pEvent);
	m_pIndicators->BuzzerPing();
	return true;
}


