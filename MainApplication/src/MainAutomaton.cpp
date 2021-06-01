#include "MainAutomaton.hpp"

#include "KeypadAutomaton.hpp"

#include<sstream>


#define AUT_ACTION(X) (bool (MAutomat::*)(MAutEvent*)) &MainAutomaton::X

OWNER KeypadAutomatonEvent* parseMessageForKeypadAutomaton(DataMailboxMessage* pMessage);

namespace parserHelper
{
	OWNER KeypadAutomatonEvent* parseDatabseReplyForKeypadAutomaton(DataMailboxMessage* pMessage);
	OWNER KeypadAutomatonEvent* parseCommandMessageForKeypadAutomaton(DataMailboxMessage* pMessage);
	OWNER KeypadAutomatonEvent* parseCommandMessageForKeypadAutomaton_CommandNONE(CommandMessage* pParsedMessage);
}

MainAutomatonEvent::MainAutomatonEvent(int idEvent, DataMailboxMessage* pMessage)
	: m_pMessage(pMessage)
{
	m_iEventID = idEvent;

	switch (m_iEventID)
	{
	case MainAutomaton::enuEvtKeypadMessageReceived:
		m_sDesc = "enuEvtKeypadMessageReceived";
		break;

	case MainAutomaton::enuEvtKeypadAutomatonFinished:
		m_sDesc = "enuEvtKeypadAutomatonFinished";
		break;

	default:
		m_sDesc = "ERROR: Unknown Event!";
		break;
	}
}

MainAutomaton::MainAutomaton(short int siInstID,
	IndicatorController_Client* pIndicators,
	ILogger* pLogger)
	:	MAutomat(siInstID),
	m_pIndicators(pIndicators),
	m_pLogger(pLogger),
	m_bGuestAccessEnable(true)
{
	if (pLogger == nullptr)
		m_pLogger = NulLogger::getInstance();

	/**********************************************
	 * TRANSITION TABLE INITIALIZATION            *
	 **********************************************/

	MAutState* pState = nullptr;

	// enuIdle = 0
	pState = new MAutState(enuIdle);
	pState->addTransition(new MAutTransition(enuEvtKeypadMessageReceived, enuBusy_KeypadRFID, this, AUT_ACTION(doForwardEventToKeypadAutomaton)) );
	this->addAutState(pState);

	// enuBusy = 1
	pState = new MAutState(enuBusy_KeypadRFID);
	pState->addTransition(new MAutTransition(enuEvtKeypadMessageReceived, enuBusy_KeypadRFID, this, AUT_ACTION(doForwardEventToKeypadAutomaton)) );
	pState->addTransition(new MAutTransition(enuEvtKeypadAutomatonFinished, enuIdle, this, AUT_ACTION(doDisplayTempMsg_AndClear)));
	this->addAutState(pState);

	m_iInitialStateID = enuIdle;
}

void MainAutomaton::setKeypadAutomatonPointer(KeypadAutomaton* pKeypadAutomaton)
{
	m_pKeypadAutomaton = pKeypadAutomaton;
}

bool MainAutomaton::initialize()
{
	if (m_pKeypadAutomaton == nullptr)
	{
		*m_pLogger << "MainAutomaton: KeypadAutomaton not found (nullptr) in ctor.";
		Kernel::Fatal_Error("MainAutomaton: KeypadAutomaton not found (nullptr) in ctor.");
	}

	if (m_pIndicators == nullptr)
	{
		*m_pLogger << "Main automaton - Indicator Controller is nullptr!";
		Kernel::Fatal_Error("Main automaton - Indicator Controller is nullptr!");
	}

	return this->test();
}

bool MainAutomaton::processEvent(MAutEvent* pEvent)
{

	// Save the event before calling processEvent because it frees the event object.
	TransitionInfo info = getTransitionInfo(pEvent);

	*m_pLogger << info.createLogEntry_Start();

	bool status = MAutomat::processEvent(pEvent);

	*m_pLogger << info.createLogEntry_End();

	return status;

}

bool MainAutomaton::reset()
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

bool MainAutomaton::doVoid(MAutEvent* pEvent)
{
	return true;
}

bool MainAutomaton::doDisplayTempMsg_AndClear(MAutEvent* pEvent)
{
	m_pIndicators->LCD_Clear_wDefaultMsg();

	MainAutomatonEvent* pParsedEvent = dynamic_cast<MainAutomatonEvent*>(pEvent);

	if (pParsedEvent->m_pMessage == nullptr)
	{
		return true;
	}

	CommandMessage* pParsedMessage = dynamic_cast<CommandMessage*>(pParsedEvent->m_pMessage);

	if (pParsedMessage->getParameterCount() < 1)
	{
		*m_pLogger << "Error in doDisplayTempMsg_AndClear(...) while trying to display temporary message! Too few args";
		Kernel::Fatal_Error("Error in doDisplayTempMsg_AndClear(...) while trying to display temporary message! Too few args");
	}

	const std::string message = pParsedMessage->getParameterAt(0).getData();
	m_pIndicators->LCD_Put_wTimeout(message);

	delete pParsedMessage;

	return true;
}

bool MainAutomaton::doForwardEventToKeypadAutomaton(MAutEvent* pEvent)
{
	MainAutomatonEvent* pReceivedEvent = dynamic_cast<MainAutomatonEvent*>(pEvent);
	if (pReceivedEvent == nullptr)
	{
		reset();
		return false;
	}

	KeypadAutomatonEvent* pOutgoingEvent = parseMessageForKeypadAutomaton(pReceivedEvent->m_pMessage);
	m_pKeypadAutomaton->processEvent(pOutgoingEvent);

	return true;
}

OWNER KeypadAutomatonEvent* parseMessageForKeypadAutomaton(DataMailboxMessage* pMessage)
{
	MessageDataType type = pMessage->getDataType();

	if (type == MessageDataType::enuType::CommandMessage)
	{
		return parserHelper::parseCommandMessageForKeypadAutomaton(pMessage);
	}

	if (type == MessageDataType::enuType::DatabaseReply)
	{
		return parserHelper::parseDatabseReplyForKeypadAutomaton(pMessage);
	}

	return nullptr;
}

OWNER KeypadAutomatonEvent* parserHelper::parseCommandMessageForKeypadAutomaton(DataMailboxMessage* pMessage)
{
	CommandMessage* pParsedMessage = dynamic_cast<CommandMessage*>(pMessage);

	if (pParsedMessage == nullptr)
	{
		return nullptr;
	}
	else if (pParsedMessage->getCommandId() == CommandMessage::NONE &&
		pParsedMessage->getParameterCount() >= 1)
	{
		return parseCommandMessageForKeypadAutomaton_CommandNONE(pParsedMessage);
	}

	switch (pParsedMessage->getCommandId())
	{
	case CommandMessage::enuCommand::ADD:
	case CommandMessage::enuCommand::REMOVE:
	case CommandMessage::enuCommand::SET_CLNC:
		return new KeypadAutomatonEvent(KeypadAutomaton::enuEvtReceivedRequest, pParsedMessage);

	case CommandMessage::enuCommand::AUTHENTICATE:
		return new KeypadAutomatonEvent(KeypadAutomaton::enuEvtReceiveAuthorizeRequest, pParsedMessage);

	case CommandMessage::enuCommand::GUEST_ACCESS_ENABLE:
		return new KeypadAutomatonEvent(KeypadAutomaton::enuEvtGuestAccessEnable, pParsedMessage);

	case CommandMessage::enuCommand::GUEST_ACCESS_DISABLE:
		return new KeypadAutomatonEvent(KeypadAutomaton::enuEvtGuestAccessDisable, pParsedMessage);
	}

	return nullptr;
}

OWNER KeypadAutomatonEvent* parserHelper::parseCommandMessageForKeypadAutomaton_CommandNONE(CommandMessage* pParsedMessage)
{
	const InputParameter& param = pParsedMessage->getParameterAt(0);
	InputParameter::enuType paramType = param.getType();
	const std::string& paramData = param.getData();

	switch (paramType)
	{
	case InputParameter::enuType::Cancel:
		return new KeypadAutomatonEvent(KeypadAutomaton::enuEvtCancel, nullptr);

	case InputParameter::enuType::Enter:
		return new KeypadAutomatonEvent(KeypadAutomaton::enuEvtEnter, nullptr);
	}

	return nullptr;
}

OWNER KeypadAutomatonEvent* parserHelper::parseDatabseReplyForKeypadAutomaton(DataMailboxMessage* pMessage)
{
	DatabaseReply* pParsedMessage = dynamic_cast<DatabaseReply*>(pMessage);

	if (pParsedMessage == nullptr)
	{
		return nullptr;
	}

	switch (pParsedMessage->getReplyStatus())
	{
	case DatabaseReply::enuStatus::SUCCESS:
		return new KeypadAutomatonEvent(KeypadAutomaton::enuEvtSuccess, pParsedMessage);

	case DatabaseReply::enuStatus::INVALID_PARAMETER:
		return new KeypadAutomatonEvent(KeypadAutomaton::enuEvtInvalidParameter, pParsedMessage);

	case DatabaseReply::enuStatus::INVALID_COMMAND:
		return new KeypadAutomatonEvent(KeypadAutomaton::enuEvtInvalidCommand, pParsedMessage);

	case DatabaseReply::enuStatus::CLEARANCE:
		return new KeypadAutomatonEvent(KeypadAutomaton::enuEvtReceivedClearanceFromDB, pParsedMessage);

	case DatabaseReply::enuStatus::ERROR:
		return new KeypadAutomatonEvent(KeypadAutomaton::enuEvtError, pParsedMessage);

	case DatabaseReply::enuStatus::INSUFFICIENT_PERMISSIONS:
		return new KeypadAutomatonEvent(KeypadAutomaton::enuEvtInsufficientPermissions, pParsedMessage);

	}

	return nullptr;
}

TransitionInfo MainAutomaton::getTransitionInfo(MAutEvent* pReceivedEvent)
{
	if (pReceivedEvent == nullptr)
	{
		return {};
	}

	TransitionInfo info;
	info.m_automatonName = "MainAutomaton";
	info.m_stateId = getCurrentStateId();
	info.m_eventId = pReceivedEvent->getEventId();
	info.m_eventDesc = pReceivedEvent->getEventDesc();

	return info;

}

