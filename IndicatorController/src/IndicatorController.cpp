#include "IndicatorController.hpp"

#include "Kernel.hpp"

// const std::string SERVER_SUFFIX = ".identifier.s";
// const std::string CLIENT_SUFFIX = ".identifier.c";
const std::string SERVER_SUFFIX = GlobalProperties::Get().INDICATORS_MB_SERVER_SUFFIX;
const std::string CLIENT_SUFFIX = GlobalProperties::Get().INDICATORS_MB_CLIENT_SUFFIX;

IndicatorController_Client::IndicatorController_Client(
	const std::string& identifier,
	ILogger* pLogger
)
	:	m_mailbox(identifier + CLIENT_SUFFIX),  // TODO client identifier same for everyone! Problem maybe?
	m_refServer(identifier + SERVER_SUFFIX),
	m_pLogger(pLogger)
{
	if (m_pLogger == nullptr)
	{
		m_pLogger = NulLogger::getInstance();
	}
}

void IndicatorController_Client::BuzzerPing()
{
	InputParameter command(InputParameter::BuzzerPing);
	sendConnectionlessRequest(command);
}

void IndicatorController_Client::BuzzerSuccess()
{
	InputParameter command(InputParameter::BuzzerSuccess);
	sendConnectionlessRequest(command);
}

void IndicatorController_Client::BuzzerFailure()
{
	InputParameter command(InputParameter::BuzzerError);
	sendConnectionlessRequest(command);
}

void IndicatorController_Client::LCD_Clear()
{
	InputParameter command(InputParameter::LCD_Clear_wDefaultMsg);
	sendConnectionlessRequest(command);
}

void IndicatorController_Client::LCD_Clear_wDefaultMsg()
{
	InputParameter command(InputParameter::LCD_Clear_wDefaultMsg);
	sendConnectionlessRequest(command);
}

void IndicatorController_Client::LCD_Put_Permanently(const std::string& message)
{
	InputParameter command(InputParameter::LCD_Message_Permanent, message);
	sendConnectionlessRequest(command);
}

void IndicatorController_Client::LCD_Put_wTimeout(const std::string& message)
{
	InputParameter command(InputParameter::LCD_Message_wTimeout, message);
	sendConnectionlessRequest(command);
}

void IndicatorController_Client::OpenDoor_wBuzzerSuccess()
{
	InputParameter command(InputParameter::DoorOpen_wBuzzerSuccess);
	sendConnectionlessRequest(command);
}

void IndicatorController_Client::sendConnectionlessRequest(const InputParameter& command)
{
	CommandMessage message(CommandMessage::enuCommand::NONE);
	message.addParameter(command);

	m_mailbox.sendConnectionless(m_refServer, &message);

	*m_pLogger << "Sent Connectionless: " + command.getInfo();

}

void IndicatorController_Client::sendRequest(const InputParameter& command)
{
	CommandMessage message(CommandMessage::enuCommand::NONE);
	message.addParameter(command);

	m_mailbox.send(m_refServer, &message);
	*m_pLogger << "Sent: " + command.getInfo();
}


IndicatorController_Server::IndicatorController_Server(
	const std::string& identifier,
	const Pinout& pinout,
	ILogger* pLogger
)
	:
	m_pLogger(pLogger),
	m_mailbox(identifier + SERVER_SUFFIX),
	m_buzzer(pinout.m_buzzerPin_BCM),
	m_door(pinout.m_doorPin_BCM, pinout.m_doorOpenTime_ms),
	m_lcd(pinout.m_lcd_i2c_bus)
{
	if (m_pLogger == nullptr)
	{
		m_pLogger = NulLogger::getInstance();
	}
}

void IndicatorController_Server::BuzzerPing()
{
	m_buzzer.SignalPing();
	*m_pLogger << "Ping!";
}

void IndicatorController_Server::BuzzerSuccess()
{
	m_buzzer.SignalSuccess();
	*m_pLogger << "Success!";
}

void IndicatorController_Server::BuzzerFailure()
{
	m_buzzer.SignalFailure();
	*m_pLogger << "Failure!";
}

void IndicatorController_Server::LCD_Clear()
{
	m_lcd.ClearDisplay();
	*m_pLogger << "Clear!";
}

void IndicatorController_Server::LCD_Clear_wDefaultMsg()
{
	m_lcd.ClearDisplayAndDisplayDefaultMsg();
	*m_pLogger << "Clear_wDeafultMsg!";
}

void IndicatorController_Server::LCD_Put_Permanently(const std::string& message)
{
	m_lcd.PutToLCD_Permanently(message);
	*m_pLogger << "Permanent message: " + message;
}

void IndicatorController_Server::LCD_Put_wTimeout(const std::string& message)
{
	m_lcd.PutToLCD_wTimeout(message);
	*m_pLogger << "Temporary message: " + message;
}

void IndicatorController_Server::OpenDoor_wBuzzerSuccess()
{
	*m_pLogger << "Doors open w buzzer!";
	m_buzzer.SignalSuccess();
	m_door.OpenDoors();
}

void IndicatorController_Server::ParseRequest(const InputParameter& request)
{
	InputParameter::enuType paramType = request.getType();
	const std::string& payload = request.getData();

	switch (paramType)
	{
	case InputParameter::enuType::LCD_Message_Permanent:
		LCD_Put_Permanently(payload);
		return;

	case InputParameter::enuType::LCD_Message_wTimeout:
		LCD_Put_wTimeout(payload);
		return;

	case InputParameter::enuType::LCD_Clear_wDefaultMsg:
		LCD_Clear_wDefaultMsg();
		return;

	case InputParameter::enuType::DoorOpen_wBuzzerSuccess:
		OpenDoor_wBuzzerSuccess();
		return;

	case InputParameter::enuType::BuzzerError:
		BuzzerFailure();
		return;

	case InputParameter::enuType::BuzzerSuccess:
		BuzzerSuccess();
		return;

	case InputParameter::enuType::BuzzerPing:
		BuzzerPing();
		return;

	}

	*m_pLogger << "Received invalid request: InputParameter == " + std::to_string((int)paramType);
	//Kernel::Fatal_Error("Received invalid request: InputParameter == " + std::to_string((int)paramType));
}

void IndicatorController_Server::ListenAndParseRequest(unsigned int timeout_ms)
{
	timespec timeout_settings = Time::getTimespecFrom_ms(timeout_ms);
	m_mailbox.setTimeout_settings(timeout_settings);

	DataMailboxMessage* pMessage = m_mailbox.receive(enuReceiveOptions::TIMED);

	CommandMessage* pParsedMessage = castToAppropriateType(pMessage);
	if (pParsedMessage == nullptr)
	{
		delete pMessage;
		return;
	}

	*m_pLogger << "Received request from: " + pParsedMessage->getSource().getName();

	const InputParameter& requestParameter = getParameter(pParsedMessage);
	ParseRequest(requestParameter);

	delete pMessage;
}

CommandMessage* IndicatorController_Server::castToAppropriateType(DataMailboxMessage* pMessage)
{
	if (pMessage->getDataType() == MessageDataType::enuType::DataMailboxErrorMessage) // TimedOut! TODO???
	{
		*m_pLogger << "IndicatorController - timed out!";
		return nullptr;
	}
	else if (pMessage->getDataType() != MessageDataType::enuType::CommandMessage)
	{
		*m_pLogger << "IndicatorController - received invalid message: " + pMessage->getInfo();
		Kernel::Warning("IndicatorController - received invalid message: " + pMessage->getInfo());

		return nullptr;
	}

	CommandMessage* pParsedRequest = dynamic_cast<CommandMessage*>(pMessage);

	if (pParsedRequest == nullptr)
	{
		*m_pLogger << "IndicatorController - could not cast DataMailboxMessage to CommandMessage dynamically: " + pMessage->getInfo();
		Kernel::Warning("IndicatorController - could not cast DataMailboxMessage to CommandMessage dynamically: " + pMessage->getInfo());

		return nullptr;
	}

	return pParsedRequest;
}

const InputParameter IndicatorController_Server::getParameter(CommandMessage* pMessage)
{
	if (pMessage->getParameterCount() < 1)
	{
		*m_pLogger << "IndicatorController - CommandMessage has too few arguments " + std::to_string(pMessage->getParameterCount());

		return InputParameter();
	}

	return pMessage->getParameterAt(0);
}