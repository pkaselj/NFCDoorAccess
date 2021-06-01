#include "InputController.hpp"
#include "Time.hpp"

#include<poll.h>
#include<sstream>

#include<map>

#define CALL_ACTION_ON_THIS(X) (this->*X)();
#define ACTION(X) &InputController::X

#define DEBUG(X) std::cout << X << std::endl;


InputController::InputController(Pipe* pPipeKeypad,
	Pipe* pPipeRFID,
	DataMailbox* pMailbox,
	MailboxReference* pRefMainApp,
	IndicatorController_Client* pIndicators,
	ILogger* pLogger)
	:	m_pLogger(pLogger),
	m_pPipeKeypad(pPipeKeypad),
	m_pPipeRFID(pPipeRFID),
	m_pMailbox(pMailbox),
	m_pRefMainApp(pRefMainApp),
	m_pIndicators(pIndicators),
	m_inputAutomaton(536, pMailbox, pRefMainApp, pIndicators, pLogger) // TODO id
{
	if (m_pLogger == nullptr)
	{
		m_pLogger = NulLogger::getInstance();
	}

	if (m_pPipeKeypad == nullptr)
	{
		*m_pLogger << "InputController - invalid keypad pipe pointer in ctor!";
		Kernel::Fatal_Error("InputController - invalid keypad pipe pointer in ctor!");
	}

	if (m_pPipeRFID == nullptr)
	{
		*m_pLogger << "InputController - invalid RFID pipe pointer in ctor!";
		Kernel::Fatal_Error("InputController - invalid RFID pipe pointer in ctor!");
	}

	if (m_pMailbox == nullptr)
	{
		*m_pLogger << "InputController - invalid Mailbox pointer in ctor!";
		Kernel::Fatal_Error("InputController - invalid Mailbox pointer in ctor!");
	}

	if (m_pRefMainApp == nullptr)
	{
		*m_pLogger << "InputController - invalid Main App Mailbox Reference pointer in ctor!";
		Kernel::Fatal_Error("InputController - invalid Main App Mailbox Reference pointer in ctor!");
	}

	if (m_pIndicators == nullptr)
	{
		*m_pLogger << "InputController - invalid Indicator Controller pointer in ctor!";
		Kernel::Fatal_Error("InputController - invalid Indicator Controller pointer in ctor!");
	}


	m_inputAutomaton.initialize();

}



void InputController::ProcessInput()
{
	InputParameter input = getInput();

	InputAutomatonEvent* pEvent = parseInputParameterToInputAutomatonEvent(input);
	
	if (pEvent == nullptr)
	{
		// *m_pLogger << "Invalid input!";
		// std::cout << "Invalid input" << std::endl;
		return;
	}

	m_inputAutomaton.processEvent(pEvent);
	
}

InputParameter InputController::getInput()
{
	int keypadFd = m_pPipeKeypad->getFd();
	int rfidFd = m_pPipeRFID->getFd();

	pollfd keypadPollStruct = {
		.fd = keypadFd,
		.events = POLLIN
	};

	pollfd rfidPollStruct =
	{
		.fd = rfidFd,
		.events = POLLIN
	};

	pollfd fdPollArray[] = { rfidPollStruct, keypadPollStruct };
	int openFds = 2;
	int timeout_ms = 10;

	pollfd* pRFIDPollStruct = &fdPollArray[0];
	pollfd* pKeypadPollStruct = &fdPollArray[1];

	
	int retval = poll(fdPollArray, openFds, timeout_ms);
	// int retval = poll(fdPollArray, openFds, -1);
	
	
	if (retval == -1)
	{
		
		*m_pLogger << "poll() error!";
		Kernel::Fatal_Error("poll error!");
	}
	else if (retval == 0)
	{
		
		// *m_pLogger << "poll() timed out!";
		return InputParameter();
	}
	
	// *m_pLogger << "poll(): #" + std::to_string(retval) + " of fd's ready I/O";
	
	if (pKeypadPollStruct->revents & POLLIN)
	{
		DEBUG("KEYPAD - MARK 1");
		std::string input = m_pPipeKeypad->receive();
		
		*m_pLogger << "\tKeypad: " + input;
		
		return parseKeypadInput(input);
	}
	else if (pRFIDPollStruct->revents & POLLIN)
	{
		DEBUG("RFID - MARK 1");
		std::string input = m_pPipeRFID->receive();
		
		*m_pLogger << "\tRFID: " + input;
		
		return parseRFIDInput(input);
	}
	
	std::stringstream errorStringBuilder;
	errorStringBuilder.str("");
	
	errorStringBuilder
		<< "Error while polling!" << "\n"
		<< "Keypad poll struct: "
			<< "\tfd: " << pKeypadPollStruct->fd << "\n"
			<< "\tevents: " << "0x" << std::hex << pKeypadPollStruct->events << "\n"
			<< "\trevents: " << "0x" << std::hex << pKeypadPollStruct->revents << "\n"
		<< "\n"
		<< "RFID poll struct: "
			<< "\tfd: " << pRFIDPollStruct->fd << "\n"
			<< "\tevents: " << "0x" << std::hex << pRFIDPollStruct->events << "\n"
			<< "\trevents: " << "0x" << std::hex << pRFIDPollStruct->revents << "\n"
		<< "\n";
	
	*m_pLogger << errorStringBuilder.str();
	
	return InputParameter();
}

InputParameter InputController::parseKeypadInput(const std::string& input)
{
	if (input == "ENTER")
	{
		return InputParameter(InputParameter::enuType::Enter);
	}
	else if (input == "BCKSPC")
	{
		return InputParameter(InputParameter::enuType::Cancel);
	}

	switch (input[0])
	{
	case '*':
	{
		std::string password = input.substr(1);
		return InputParameter(InputParameter::enuType::KeypadPIN, password);
		break;
	}

	case '/':
	{
		std::string command = input.substr(1);
		return InputParameter(InputParameter::enuType::KeypadCommand, command);
		break;
	}

	default:
		return InputParameter(InputParameter::enuType::PlainData, input);
		break;
	}
}

InputParameter InputController::parseRFIDInput(const std::string& input)
{
	
	return InputParameter(InputParameter::enuType::RFIDCard, input);
}


OWNER InputAutomatonEvent* InputController::parseInputParameterToInputAutomatonEvent(InputParameter& input)
{
	using enuType = InputParameter::enuType;

	switch (input.getType())
	{
	case enuType::KeypadPIN:
		return new InputAutomatonEvent(InputAutomaton::enuEvtKeypadPIN, input);

	case enuType::PlainData:
		return new InputAutomatonEvent(InputAutomaton::enuEvtPlain, input);

	case enuType::RFIDCard:
		return new InputAutomatonEvent(InputAutomaton::enuEvtRFIDCard, input);

	case enuType::Enter:
		return new InputAutomatonEvent(InputAutomaton::enuEvtEnter, input);

	case enuType::Cancel:
		return new InputAutomatonEvent(InputAutomaton::enuEvtCancel, input);

	case enuType::KeypadCommand:
		return parseCommandInputParameterToInputAutomatonEvent(input);
	}

	return nullptr;
}

OWNER InputAutomatonEvent* InputController::parseCommandInputParameterToInputAutomatonEvent(InputParameter& command)
{
	using MAP = std::map<const std::string, InputAutomaton::enumAutEventType>;
	using Iter = MAP::const_iterator;

	std::string commandString = command.getData();

	const MAP conversionMap =
	{
		{"1111", InputAutomaton::enuEvtAddCommand},
		{"2222", InputAutomaton::enuEvtRemoveCommand},
		{"3333", InputAutomaton::enuEvtSetClearanceCommand},
		{"4444", InputAutomaton::enuEvtGuestAccessEnable},
		{"5555", InputAutomaton::enuEvtGuestAccessDisable}
	};

	Iter result = conversionMap.find(commandString);
	if (result == conversionMap.end())
	{
		return nullptr;
	}

	InputAutomaton::enumAutEventType eventType = result->second;
	return new InputAutomatonEvent(eventType, command);
}
