#include "WatchdogClient.hpp"
#include "Time.hpp"

#include<sstream>

WatchdogClient::WatchdogClient(const std::string& unitName,
	const std::string& serverName,
	const SlotSettings& settings,
	enuActionOnFailure onFailure,
	ILogger* pLogger)
	: m_unitName(unitName),
	m_server(serverName + ".server"),
	m_settings({0, 0}),
	m_pLogger(pLogger),
	m_PID(getpid()),
	// m_bAlive(false),
	// m_pCallbackFunctor(new TimerCallbackFunctor<WatchdogClient>(this, &WatchdogClient::sendAliveMessageToServer_TimeoutCallback)),
	// m_tmrKickAlive(unitName, pLogger),
	m_mailbox(unitName, pLogger),
	m_status(enuStatus::UNREGISTERED),
	m_onFailure(onFailure),
	m_offset(-1),
	m_shmControlBlock(serverName + ".shm")
{
	if (m_pLogger == nullptr)
	{
		m_pLogger = NulLogger::getInstance();
	}

	if (m_unitName == "")
	{
		*m_pLogger << "WatchdogClient - unitName cannot be empty!";
		Kernel::Fatal_Error("WatchdogClient - unitName cannot be empty!");
	}

	setSettings(settings);

	*m_pLogger << m_unitName + " - WatchdogClient successfully created";

	// m_tmrKickAlive.setTimeoutCallback(m_pCallbackFunctor);
	// m_tmrKickAlive.clearTimeoutSettings();

	Register();
}

WatchdogClient::~WatchdogClient()
{
	if (m_status != enuStatus::UNREGISTERED)
	{
		Unregister();
	}

	// delete m_pCallbackFunctor;
}

void WatchdogClient::Register()
{
	*m_pLogger << m_unitName + " - WatchdogUnit - Registration request to: " + m_server.getName();

	// if (serverName == "")
	// {
	// 	*m_pLogger << m_serverName + " - WatchdogClient - serverName cannot be empty!";
	// 	Kernel::Fatal_Error(m_serverName + " - WatchdogClient - serverName cannot be empty!");
	// }

	// m_serverName = serverName;
	// m_server = MailboxReference(m_serverName + ".server");

	

	WatchdogMessage registration_request(m_unitName, m_settings, m_PID, m_onFailure, WatchdogMessage::REGISTER_REQUEST);
	m_mailbox.send(m_server, &registration_request);
	//m_mailbox.sendConnectionless(m_server, &registration_request);

	const int serverReplyTimeout_s = 10;
	WatchdogMessage reply = waitForServerReply(WatchdogMessage::MessageClass::REGISTER_REPLY, enuReceiveOptions::TIMED, serverReplyTimeout_s);

	// DEBUG
	// std::cout << reply.getInfo() << std::endl;

	int offset = reply.getOffset();
	if (offset < 0)
	{
		*m_pLogger << m_unitName + " - invalid offset from WatchdogServer: " + std::to_string(offset);
		Kernel::Fatal_Error(m_unitName + " - invalid offset from WatchdogServer: " + std::to_string(offset));
		return;
	}

	m_offset = offset;
	// std::cout << "Offset value = " << m_offset << std::endl;

	// kickTimer();

	*m_pLogger << m_unitName + " - WatchdogUnit - Registration confirmed by: " + m_serverName;

}

void WatchdogClient::setSettings(const SlotSettings& settings)
{
	if (settings.isZero() || settings.timeoutIsZero())
	{
		*m_pLogger << m_unitName + " - WatchdogUnit - registration settings cannot be null!";
		Kernel::Fatal_Error(m_unitName + " - WatchdogUnit - registration settings cannot be null!");
	}

	m_settings = settings;

	// const int timerFactor = 3;
	// m_tmrKickAlive.setTimeout_ms(m_settings.m_timeout_ms / timerFactor);
}

WatchdogMessage WatchdogClient::waitForServerReply(WatchdogMessage::MessageClass expectedMessageClass, enuReceiveOptions timedFlag, long timeout_s)
{
	*m_pLogger << "Expecting reponse from: " + m_serverName + " - of class: " + WatchdogMessage::getMessageClassName(expectedMessageClass);

	WatchdogMessage received_response = receiveAndCheckInputMessageFrom(m_server.getName());

	if (received_response.getMessageClass() != expectedMessageClass)
	{
		std::stringstream stringBuilder;

		stringBuilder << m_unitName << " - WatchdogUnit - wrong response from - " << received_response.getSource().getName() << " - on registration request!\n"
			<< "Expected reponse from: " << m_serverName << " - of class: " << WatchdogMessage::getMessageClassName(expectedMessageClass)
			<< " (instead got: " << received_response.getMessageClassName() << " )\n"
			<< "Message info: " << received_response.getInfo();

		*m_pLogger << stringBuilder.str();
		Kernel::Fatal_Error(stringBuilder.str());
		return WatchdogMessage();
	}

	if (received_response.getSource() != m_server)
	{
		*m_pLogger << m_unitName + " - WatchdogUnit - Expected reponse from: " + m_serverName + " instead got from: " + received_response.getSource().getName();
		Kernel::Fatal_Error(m_unitName + " - WatchdogUnit - Expected reponse from: " + m_serverName + " instead got from: " + received_response.getSource().getName());
		return WatchdogMessage();
	}

	*m_pLogger << m_unitName + " - WatchdogUnit - received expected response from: " + m_serverName;

	return received_response;
}

WatchdogMessage WatchdogClient::receiveAndCheckInputMessageFrom(const std::string& sourceName, enuReceiveOptions timedFlag, long timeout_ms)
{
	timespec oldSettings = m_mailbox.getTimeout_settings();

	timespec response_timeout_struct = Time::getTimespecFrom_ms(timeout_ms);
	m_mailbox.setTimeout_settings(response_timeout_struct); // does nothing if timedFlag == NORMAL (raises kernel warning?)

	DataMailboxMessage* pReceivedMessage = m_mailbox.receive(timedFlag);
	MessageDataType messageType = pReceivedMessage->getDataType();

	if (messageType == MessageDataType::enuType::DataMailboxErrorMessage)
	{
		*m_pLogger << m_unitName + " - WatchdogUnit - no response!";
		Kernel::Fatal_Error(m_unitName + " - WatchdogUnit - no response!");
	}
	else if (messageType != MessageDataType::enuType::WatchdogMessage || pReceivedMessage->getSource().getName() != sourceName)
	{
		std::stringstream stringBuilder;

		stringBuilder << m_unitName << " - WatchdogUnit - wrong response from - " << pReceivedMessage->getSource().getName() << "!\n"
			<< "Expected reponse from: " << sourceName << " - of type WatchdogMessage (instead got: " << pReceivedMessage->getDataType().toString() << " )\n"
			<< "Message info: " << pReceivedMessage->getInfo();

		*m_pLogger << stringBuilder.str();
		Kernel::Fatal_Error(stringBuilder.str());
	}

	WatchdogMessage receivedMessageCopy = std::move(*(dynamic_cast<WatchdogMessage*>(pReceivedMessage)));

	delete pReceivedMessage;

	m_mailbox.setTimeout_settings(oldSettings);

	return receivedMessageCopy;
}

/*
void WatchdogClient::waitForServerReply(WatchdogMessage::MessageClass expectedMessageClass, enuReceiveOptions timedFlag, long timeout_s)
{
	*m_pLogger << "Expecting reponse from: " + m_serverName + " - of class: " + WatchdogMessage::getMessageClassName(expectedMessageClass);

	WatchdogMessage received_response = receiveAndCheckInputMessageFrom(m_server.getName());

	if (received_response.getMessageClass() != expectedMessageClass)
	{
		std::stringstream stringBuilder;

		stringBuilder << m_unitName << " - WatchdogUnit - wrong response from - " << received_response.getSource().getName() << " - on registration request!\n"
			<< "Expected reponse from: " << m_serverName << " - of class: " << WatchdogMessage::getMessageClassName(expectedMessageClass)
			<< " (instead got: " << received_response.getMessageClassName() << " )\n"
			<< "Message info: " << received_response.getInfo();

		*m_pLogger << stringBuilder.str();
		Kernel::Fatal_Error(stringBuilder.str());
	}

	if (received_response.getSource() != m_server)
	{
		*m_pLogger << m_unitName + " - WatchdogUnit - Expected reponse from: " + m_serverName + " instead got from: " + received_response.getSource().getName();
		Kernel::Fatal_Error(m_unitName + " - WatchdogUnit - Expected reponse from: " + m_serverName + " instead got from: " + received_response.getSource().getName());
	}

	*m_pLogger << m_unitName + " - WatchdogUnit - received expected response from: " + m_serverName;

}

WatchdogMessage WatchdogClient::receiveAndCheckInputMessageFrom(const std::string& sourceName, enuReceiveOptions timedFlag, long timeout_ms)
{
	timespec oldSettings = m_mailbox.getTimeout_settings();


	if (timedFlag == enuReceiveOptions::NORMAL)
	{
		timeout_ms = 1; // DUMMY VALUE, IGNORED IF timedFlag IS NOT SET TO TIMED,
						// BUT NOT SET TO ZERO AS NOT TO TRIGGER 0 AND POSSILY NEGATIVE ERRORS IN mailbox SETTINGS (setTimeout_settings)
						// PURELY PRECAUTIONARY MEASURE
	}

	timespec response_timeout_struct = Time::getTimespecFrom_ms(timeout_ms);
	m_mailbox.setTimeout_settings(response_timeout_struct); // does nothing if timedFlag == NORMAL (raises kernel warning?)

	DataMailboxMessage* pReceivedMessage = m_mailbox.receive(timedFlag);
	MessageDataType messageType = pReceivedMessage->getDataType();

	if (messageType == MessageDataType::enuType::DataMailboxErrorMessage)
	{
		*m_pLogger << m_unitName + " - WatchdogUnit - no response!";
		Kernel::Fatal_Error(m_unitName + " - WatchdogUnit - no response!");
	}
	else if (messageType != MessageDataType::enuType::WatchdogMessage || pReceivedMessage->getSource().getName() != sourceName)
	{
		std::stringstream stringBuilder;

		stringBuilder << m_unitName << " - WatchdogUnit - wrong response from - " << pReceivedMessage->getSource().getName() << "!\n"
			<< "Expected reponse from: " << sourceName << " - of type WatchdogMessage (instead got: " << pReceivedMessage->getDataType().toString() << " )\n"
			<< "Message info: " << pReceivedMessage->getInfo();

		*m_pLogger << stringBuilder.str();
		Kernel::Fatal_Error(stringBuilder.str());
	}

	WatchdogMessage receivedMessageCopy = std::move( *(dynamic_cast<WatchdogMessage*>(pReceivedMessage)) );

	delete pReceivedMessage;

	m_mailbox.setTimeout_settings(oldSettings);

	return receivedMessageCopy;
}
*/

void WatchdogClient::sendSignal(WatchdogMessage::MessageClass messageClass, enuSendOptions sendOptions)
{
	*m_pLogger << m_unitName + " - WatchdogUnit - preparing signal( " + WatchdogMessage::getMessageClassName(messageClass) + " ) for: " + m_serverName;

	// std::cout << "Message class: " << messageClass << std::endl;


	WatchdogMessage signal_message(m_unitName, m_settings, m_PID, m_onFailure, messageClass);

	if (sendOptions % enuSendOptions::NORMAL)
	{
		m_mailbox.send(m_server, &signal_message);
	}
	else if (sendOptions % enuSendOptions::CONNECTIONLESS)
	{
		m_mailbox.sendConnectionless(m_server, &signal_message);
	}
	else
	{
		*m_pLogger << m_unitName + " - WatchdogUnit - signal( " +
			WatchdogMessage::getMessageClassName(messageClass) + " ) for: " + m_serverName + " - could not be sent, invalid options!";

		Kernel::Warning(m_unitName + " - WatchdogUnit - signal( " +
			WatchdogMessage::getMessageClassName(messageClass) + " ) for: " + m_serverName + " - could not be sent, invalid options!");

		return;
	}
	

	*m_pLogger << m_unitName + " - WatchdogUnit - signal( " + WatchdogMessage::getMessageClassName(messageClass) + " ) sent to: " + m_serverName;
}

void WatchdogClient::Unregister()
{
	sendSignal(WatchdogMessage::MessageClass::UNREGISTER_REQUEST, enuSendOptions::CONNECTIONLESS);
}

void WatchdogClient::Start()
{
	sendSignal(WatchdogMessage::MessageClass::START, enuSendOptions::CONNECTIONLESS);
}

void WatchdogClient::Stop()
{
	sendSignal(WatchdogMessage::MessageClass::STOP, enuSendOptions::CONNECTIONLESS);
}



bool WatchdogClient::Kick()
{
	// sendSignal(WatchdogMessage::MessageClass::KICK, enuSendOptions::CONNECTIONLESS);
	*m_pLogger << "Kick!";

	// std::cout << "Offset val #2 = " << m_offset << std::endl;

	if (m_offset < 0)
	{
		*m_pLogger << m_unitName + " - offset not set!";
		Kernel::Fatal_Error(m_unitName + " - offset not set!");
		return false;
	}

	m_shmControlBlock[m_offset].m_bAlive = true;
	return !(m_shmControlBlock[m_offset].m_bTerminate);

	// m_bAlive = true;

	// kickTimer();

	/*

	DataMailboxMessage* pReceivedMessage = m_mailbox.receive(enuReceiveOptions::NONBLOCKING);

	if (pReceivedMessage == nullptr)
	{
		std::cout << "NULLPTR" << std::endl;
		*m_pLogger << m_unitName + " - received message while kicking was nullptr";
		Kernel::Fatal_Error(m_unitName + " - received message while kicking was nullptr");
	}

	if (pReceivedMessage->getDataType() == MessageDataType::enuType::DataMailboxErrorMessage)
	{ // TODO WEAK CONDITION

		*m_pLogger << m_unitName + " - No messages in queue!";

		delete pReceivedMessage;

		return true; // NO PENDING MESSAGES (terminate broadcast)
	}
	else if(pReceivedMessage->getDataType() == MessageDataType::enuType::WatchdogMessage)
	{

		WatchdogMessage* pReceivedResponse = dynamic_cast<WatchdogMessage*>(pReceivedMessage);

		if (pReceivedResponse->getMessageClass() == WatchdogMessage::MessageClass::TERMINATE_BROADCAST)
		{
			*m_pLogger << m_unitName + " - Terminate boradcast received!";
			delete pReceivedResponse;
			return false;
		}
		else
		{
			*m_pLogger << m_unitName + " - received invalid WatchdogMessage while kicking: " + pReceivedResponse->getInfo();
			Kernel::Warning(m_unitName + " - received invalid WatchdogMessage while kicking: " + pReceivedResponse->getInfo());
			delete pReceivedResponse;
			return true;
		}
	}

	// srand(time(NULL));

	// pReceivedMessage->DumpSerialData("client_invalid_message_" + std::to_string(rand()%1000));
	*m_pLogger << m_unitName + " - received invalid message while kicking: " + pReceivedMessage->getInfo();
	Kernel::Warning(m_unitName + " - received invalid message while kicking: " + pReceivedMessage->getInfo());

	// pReceivedMessage->DumpSerialData("INVALID_MESSAGE_WHILE_KICKING_" + Time::getTime());

	delete pReceivedMessage;
	return false;
	*/


}

void WatchdogClient::Terminate()
{
	sendSignal(WatchdogMessage::MessageClass::TERMINATE_REQUEST, enuSendOptions::CONNECTIONLESS);
}

void WatchdogClient::UpdateSettings(const SlotSettings& settings)
{
	setSettings(settings);
	sendSignal(WatchdogMessage::MessageClass::UPDATE_SETTINGS, enuSendOptions::CONNECTIONLESS);
}

void WatchdogClient::Sync()
{
	// sendSignal(WatchdogMessage::MessageClass::SYNC_REQUEST);

	*m_pLogger << m_unitName + " - WatchdogUnit - waiting for server: " + m_serverName + " - to send `synchronized` signal.";

	waitForServerReply(WatchdogMessage::MessageClass::SYNC_BROADCAST, enuReceiveOptions::CONNECTIONLESS);
}

// void WatchdogClient::sendAliveMessageToServer_TimeoutCallback(void*)
// {
// 	*m_pLogger << "Sent alive signal!";
// 
// 	if (m_bAlive)
// 	{
// 		sendSignal(WatchdogMessage::MessageClass::KICK, enuSendOptions::CONNECTIONLESS);
// 	}
// 
// 	m_tmrKickAlive.Reset();
// }
// 
// void WatchdogClient::kickTimer()
// {
// 	*m_pLogger << "Timer kicked!";
// 	if (m_tmrKickAlive.getTimerStatus() != Timer::Started)
// 	{
// 		m_tmrKickAlive.Start();
// 	}
// }
