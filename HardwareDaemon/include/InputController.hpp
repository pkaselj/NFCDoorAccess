#ifndef KEYPAD_CONTROLLER_HPP
#define KEYPAD_CONTROLLER_HPP

#include "DataMailbox.hpp"
#include "pipe.hpp"
#include "InputAutomaton.hpp"
#include "IndicatorController.hpp"


/// Class which processes inputs into messages
class InputController
{
public:
	/**
	 * @brief Construct a new InputController object
	 * @param pPipeKeypad Pointer to a FIFO Pipe which is used by the Keypad on the other end to send data
	 * @param pPipeRFID Pointer to a FIFO Pipe which is used by the RFID Reader on the other end to send data
	 * @param pMailbox Pointer to a Mailbox
	 * @param pRefMainApp Pointer to a MailboxReference of MainApplication
	 * @param pIndicators Pointer to IndicatorController Client object
	 * @param pLogger Pointer to a Logger object
	*/
	InputController(Pipe* pPipeKeypad,
		Pipe* pPipeRFID,
		DataMailbox* pMailbox,
		MailboxReference* pRefMainApp,
		IndicatorController_Client* pIndicators,
		ILogger* pLogger = NulLogger::getInstance());

	~InputController(){}

	/// Listens for inputs and then processes it to messages
	void ProcessInput();

private:

	typedef void(InputController::* action) ();

	ILogger* m_pLogger;
	Pipe* m_pPipeKeypad;
	Pipe* m_pPipeRFID;
	DataMailbox* m_pMailbox;
	MailboxReference* m_pRefMainApp;
	IndicatorController_Client* m_pIndicators;

	InputAutomaton m_inputAutomaton;

	/// Return InputParameter which represents Keypad/RFID Reader input
	InputParameter getInput();

	InputParameter parseKeypadInput(const std::string& input);
	InputParameter parseRFIDInput(const std::string& input);

	/// Maps InputParameter into matching InputAutomatonEvent object
	OWNER InputAutomatonEvent* parseInputParameterToInputAutomatonEvent(InputParameter& input);
	/// Specialization of `parseInputParameterToInputAutomatonEvent()` for commands
	OWNER InputAutomatonEvent* parseCommandInputParameterToInputAutomatonEvent(InputParameter& command);


};

#endif