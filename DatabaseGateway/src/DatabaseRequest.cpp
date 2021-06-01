/*
*	 Copyright (C) Petar Kaselj 2021
*
*	 This file is part of NFCDoorAccess.
*
*	 NFCDoorAccess is written by Petar Kaselj as an employee of
*	 Emovis tehnologije d.o.o. which allowed its release under
*	 this license.
*
*    NFCDoorAccess is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NFCDoorAccess is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NFCDoorAccess.  If not, see <https://www.gnu.org/licenses/>.
*
*/

#include "DatabaseRequest.hpp"

#include "ValidationUtils.hpp"

DatabaseRequestFactory::DatabaseRequestFactory(DatabaseResources& resources, ILogger* pLogger)
	: m_resources(resources), m_pLogger(pLogger)
{
	if (m_pLogger == nullptr)
	{
		m_pLogger = NulLogger::getInstance();
	}

	if (resources.m_pMailbox == nullptr)
	{
		*m_pLogger << "DatabaseRequestFactory -- pointer to pMailbox cannot be null!";
		Kernel::Fatal_Error("DatabaseRequestFactory -- pointer to pMailbox cannot be null!");
	}

	if (resources.m_pDatabaseObject == nullptr)
	{
		*m_pLogger << "DatabaseRequestFactory -- pointer to pDatabaseObject cannot be null!";
		Kernel::Fatal_Error("DatabaseRequestFactory -- pointer to pDatabaseObject cannot be null!");
	}

	if (resources.m_pLogMailbox == nullptr)
	{
		*m_pLogger << "DatabaseRequestFactory -- pointer to pLogMailbox cannot be null!";
		Kernel::Fatal_Error("DatabaseRequestFactory -- pointer to pLogMailbox cannot be null!");
	}

}

IDatabaseRequest* DatabaseRequestFactory::createRequestObjectFrom(CommandMessage** ppRequestMessage)
{
	CommandMessage::enuCommand command = (*ppRequestMessage)->getCommandId();
	switch (command)
	{
	case CommandMessage::enuCommand::AUTHENTICATE:
		return new AuthorizeRequest(ppRequestMessage, m_resources, m_pLogger);

	case CommandMessage::enuCommand::ADD:
		return new AddRequest(ppRequestMessage, m_resources, m_pLogger);

	case CommandMessage::enuCommand::REMOVE:
		return new RemoveRequest(ppRequestMessage, m_resources, m_pLogger);

	case CommandMessage::enuCommand::SET_CLNC:
		return new SetClearanceRequest(ppRequestMessage, m_resources, m_pLogger);

	case CommandMessage::enuCommand::GUEST_ACCESS_ENABLE:
	case CommandMessage::enuCommand::GUEST_ACCESS_DISABLE:
		return new GuestAccessControl(ppRequestMessage, m_resources, m_pLogger);


	}

	*m_pLogger << "Invalid command type (CommandMessage::enuCommand) [ " + std::to_string((int)command) + " ]";

	return nullptr;
}

IDatabaseRequest::IDatabaseRequest(CommandMessage** ppRequestMessage, DatabaseResources& resources, ILogger* pLogger)
	: m_pRequest(*ppRequestMessage),
	m_resources(resources),
	m_requiredClearance(-1), // TODO max clearance?
	m_pLogger(pLogger)
{
	if (m_pLogger == nullptr)
	{
		m_pLogger = NulLogger::getInstance();
	}

	if (m_pRequest == nullptr)
	{
		*m_pLogger << "DatabaseRequest -- pointer to pMessage cannot be null!";
		Kernel::Fatal_Error("DatabaseRequest -- pointer to pMessage cannot be null!");
	}


	ppRequestMessage = nullptr;
}

IDatabaseRequest::~IDatabaseRequest()
{
	delete m_pRequest;
}

void IDatabaseRequest::Process()
{
	*m_pLogger << "Started processing new Request: " + m_pRequest->getInfo();

	if (Validate() == false)
	{
		*m_pLogger << "Validation failed for received CommandMessage: " + m_pRequest->getInfo();
		ReplyToRequestSource(DatabaseReply::enuStatus::INVALID_PARAMETER);
		return;
	}
	
	if (Authorize() == false)
	{
		*m_pLogger << "Authorization failed for received CommandMessage: " + m_pRequest->getInfo();
		ReplyToRequestSource(DatabaseReply::enuStatus::INSUFFICIENT_PERMISSIONS);
		return;
	}


	*m_pLogger << "Executing received request";
	Execute();

	Log();
	
}

void IDatabaseRequest::ReplyToRequestSource(DatabaseReply::enuStatus replyStatus)
{

	MailboxReference destination = m_pRequest->getSource();
	
	DatabaseReply reply(replyStatus);
	
	m_resources.m_pMailbox->send(destination, &reply);

}


bool AuthorizeRequest::Validate()
{
	bool parameterCountCondition = hasParameterCount(m_pRequest, 1);
	if (parameterCountCondition == false)
	{
		*m_pLogger << "Parameter count validation failed!";
		return false;
	}

	InputParameter param1 = m_pRequest->getParameterAt(0);
	bool param1Valid = isParameterValid(param1, { InputParameter::enuType::KeypadPIN, InputParameter::enuType::RFIDCard });

	if (param1Valid == false)
	{
		*m_pLogger << "Parameter validation failed!";
		// return false;
	}

	return param1Valid;
}

bool AuthorizeRequest::Authorize()
{
	/*
		CommandMessage::enuCommand requestedCommand = m_pRequest->getCommandId();
	Clearance requiredClearance = m_pDatabaseObject->getRequiredClearanceForCommand(requestedCommand);

	InputParameter clientIdentification = m_pRequest->getParameterAt(0);
	Clearance clientClearance = m_pDatabaseObject->getClearance(clientIdentification);

	return clientClearance >= requiredClearance;
	*/

	return true;

}

void AuthorizeRequest::Execute()
{
	InputParameter clientIdentification = m_pRequest->getParameterAt(0);
	Clearance clientClearance = m_resources.m_pDatabaseObject->getClearance(clientIdentification);
	ReplyWithRequestedClearance(clientClearance);
}

void AuthorizeRequest::ReplyWithRequestedClearance(Clearance clearance)
{
	DatabaseReply reply(clearance);
	m_resources.m_pMailbox->send(m_pRequest->getSource(), &reply);
}

void AuthorizeRequest::Log()
{
	const InputParameter& userCredentials = m_pRequest->getParameterAt(0);
	m_resources.m_pDatabaseObject->CreateLog(CommandMessage::enuCommand::AUTHENTICATE, userCredentials);
	
}


bool AddRequest::Validate()
{
	bool parameterCountCondition = hasParameterCount(m_pRequest, 2, 3);
	if (parameterCountCondition == false)
	{
		*m_pLogger << "Parameter count validation failed!";
		return false;
	}

	InputParameter param1 = m_pRequest->getParameterAt(0);
	bool param1Valid = isParameterValid(param1, { InputParameter::enuType::KeypadPIN, InputParameter::enuType::RFIDCard });

	InputParameter param2 = m_pRequest->getParameterAt(1);
	bool param2Valid = isParameterValid(param2, { InputParameter::enuType::KeypadPIN, InputParameter::enuType::RFIDCard });


	bool param3Valid = false;
	if (m_pRequest->getParameterCount() == 3)
	{
		InputParameter param3 = m_pRequest->getParameterAt(2);
		param3Valid = isParameterValid(param3, { InputParameter::enuType::PlainData });
	}
	else
	{
		param3Valid = true;
	}

	bool areParamsValid = param1Valid && param2Valid && param3Valid;
	if (areParamsValid == false)
	{
		std::stringstream logStringBuilder;
		logStringBuilder.str("");

		logStringBuilder << "Parameter validation falied:\n"
			<< "Param1 [" << param1Valid << " ]\n"
			<< "Param2 [" << param2Valid << " ]\n"
			<< "Param3 [" << param3Valid << " ]\n";

		*m_pLogger << logStringBuilder.str();
	}

	return areParamsValid;
}

bool AddRequest::Authorize()
{
	CommandMessage::enuCommand requestedCommand = m_pRequest->getCommandId();
	Clearance requiredClearance = m_resources.m_pDatabaseObject->getRequiredClearanceForCommand(requestedCommand);

	InputParameter clientIdentification = m_pRequest->getParameterAt(1);
	Clearance clientClearance = m_resources.m_pDatabaseObject->getClearance(clientIdentification);

	return clientClearance >= requiredClearance;
}

void AddRequest::Execute()
{
	Clearance newClearance = 0;

	if (m_pRequest->getParameterCount() == 3)
	{
		std::string stringClearance = m_pRequest->getParameterAt(2).getData();

		newClearance = destringifyClearance(stringClearance);
	}

	InputParameter identifierToBeAdded = m_pRequest->getParameterAt(0);

	bool success = m_resources.m_pDatabaseObject->AddIdentifier(identifierToBeAdded, newClearance);

	if (success == true)
	{
		ReplyToRequestSource(DatabaseReply::enuStatus::SUCCESS);
		return;
	}

	ReplyToRequestSource(DatabaseReply::enuStatus::ERROR);
	
}

void AddRequest::Log()
{
	const InputParameter& userCredentials = m_pRequest->getParameterAt(1);
	m_resources.m_pDatabaseObject->CreateLog(CommandMessage::enuCommand::ADD, userCredentials);
}

bool RemoveRequest::Validate()
{
	bool parameterCountCondition = hasParameterCount(m_pRequest, 2);
	if (parameterCountCondition == false)
	{
		*m_pLogger << "Parameter count validation failed!";
		return false;
	}

	InputParameter param1 = m_pRequest->getParameterAt(0);
	InputParameter param2 = m_pRequest->getParameterAt(1);

	bool param1Valid = isParameterValid(param1, { InputParameter::enuType::KeypadPIN, InputParameter::enuType::RFIDCard });
	bool param2Valid = isParameterValid(param2, { InputParameter::enuType::KeypadPIN, InputParameter::enuType::RFIDCard });

	bool parametersValidCondition = param1Valid && param2Valid;

	if (parametersValidCondition == false)
	{
		std::stringstream logStringBuilder;
		logStringBuilder.str("");

		logStringBuilder << "Parameter validation falied:\n"
			<< "Param1 [" << param1Valid << " ]\n"
			<< "Param2 [" << param2Valid << " ]\n";

		*m_pLogger << logStringBuilder.str();
	}

	return parametersValidCondition;

}

bool RemoveRequest::Authorize()
{
	CommandMessage::enuCommand requestedCommand = m_pRequest->getCommandId();
	Clearance requiredClearance = m_resources.m_pDatabaseObject->getRequiredClearanceForCommand(requestedCommand);

	InputParameter clientCredentials = m_pRequest->getParameterAt(1);
	Clearance clientClearance = m_resources.m_pDatabaseObject->getClearance(clientCredentials);

	return clientClearance >= requiredClearance;
}

void RemoveRequest::Execute()
{
	InputParameter credentialsToBeRemoved = m_pRequest->getParameterAt(0);

	bool success = m_resources.m_pDatabaseObject->RemoveIdentifier(credentialsToBeRemoved);

	if (success == false)
	{
		ReplyToRequestSource(DatabaseReply::enuStatus::ERROR);
		return;
	}

	ReplyToRequestSource(DatabaseReply::enuStatus::SUCCESS);
}

void RemoveRequest::Log()
{
	const InputParameter& userCredentials = m_pRequest->getParameterAt(1);
	m_resources.m_pDatabaseObject->CreateLog(CommandMessage::enuCommand::REMOVE, userCredentials);
}



bool SetClearanceRequest::Validate()
{
	bool parameterCountCondition = hasParameterCount(m_pRequest, 3);
	if (parameterCountCondition == false)
	{
		*m_pLogger << "Parameter count validation failed!";
		return false;
	}

	InputParameter param1 = m_pRequest->getParameterAt(0);
	InputParameter param2 = m_pRequest->getParameterAt(1);
	InputParameter param3 = m_pRequest->getParameterAt(2);

	bool param1Valid = isParameterValid(param1, { InputParameter::enuType::KeypadPIN, InputParameter::enuType::RFIDCard });
	bool param2Valid = isParameterValid(param2, { InputParameter::enuType::PlainData});
	bool param3Valid = isParameterValid(param3, { InputParameter::enuType::KeypadPIN, InputParameter::enuType::RFIDCard });

	bool parametersValidCondition = param1Valid && param2Valid && param3Valid;
	if (parametersValidCondition == false)
	{
		std::stringstream logStringBuilder;
		logStringBuilder.str("");

		logStringBuilder << "Parameter validation falied:\n"
			<< "Param1 [" << param1Valid << " ]\n"
			<< "Param2 [" << param2Valid << " ]\n"
			<< "Param3 [" << param3Valid << " ]\n";

		*m_pLogger << logStringBuilder.str();
	}

	return parametersValidCondition;
}

bool SetClearanceRequest::Authorize()
{
	CommandMessage::enuCommand requestedCommand = m_pRequest->getCommandId();
	Clearance requiredClearance = m_resources.m_pDatabaseObject->getRequiredClearanceForCommand(requestedCommand);

	InputParameter clientIdentification = m_pRequest->getParameterAt(2);
	Clearance clientClearance = m_resources.m_pDatabaseObject->getClearance(clientIdentification);

	return clientClearance >= requiredClearance;
}

void SetClearanceRequest::Execute()
{
	InputParameter credentialsToBeAltered = m_pRequest->getParameterAt(0);
	InputParameter parameter_wNewClearance = m_pRequest->getParameterAt(1);

	Clearance newClearance = destringifyClearance(parameter_wNewClearance.getData());
	if (newClearance == FAIL_SAFE_CLEARANCE)
	{
		ReplyToRequestSource(DatabaseReply::enuStatus::INVALID_PARAMETER);
		*m_pLogger << "Invalid clearance format: " + parameter_wNewClearance.getInfo();
		return;
	}

	bool success = m_resources.m_pDatabaseObject->SetClearance(credentialsToBeAltered, newClearance);
	if (success == false)
	{
		// *m_pLogger << "Failed to Set clearance to [ " + std::to_string((int)newClearance) + " ] on " + credentialsToBeAltered.getInfo();
		ReplyToRequestSource(DatabaseReply::enuStatus::ERROR);
		return;
	}

	ReplyToRequestSource(DatabaseReply::enuStatus::SUCCESS);

}

void SetClearanceRequest::Log()
{
	const InputParameter& userCredentials = m_pRequest->getParameterAt(2);
	m_resources.m_pDatabaseObject->CreateLog(CommandMessage::enuCommand::SET_CLNC, userCredentials);
}


bool GuestAccessControl::Validate()
{
	bool parameterCountCondition = hasParameterCount(m_pRequest, 1);
	if (parameterCountCondition == false)
	{
		*m_pLogger << "Parameter count validation failed!";
		return false;
	}

	InputParameter param1 = m_pRequest->getParameterAt(0);
	bool param1Valid = isParameterValid(param1, { InputParameter::enuType::KeypadPIN, InputParameter::enuType::RFIDCard });

	if (param1Valid == false)
	{
		*m_pLogger << "Parameter validation failed!";
		// return false;
	}

	return param1Valid;
}

bool GuestAccessControl::Authorize()
{
	// No permissions required to 'check' if one has permissions to alter guest access conditions.
	// Actual permission check is located in Execute()
	return true;
}

void GuestAccessControl::Execute()
{
	CommandMessage::enuCommand requestedCommand = m_pRequest->getCommandId();
	Clearance requiredClearance = m_resources.m_pDatabaseObject->getRequiredClearanceForCommand(requestedCommand);

	InputParameter clientIdentification = m_pRequest->getParameterAt(0);
	Clearance clientClearance = m_resources.m_pDatabaseObject->getClearance(clientIdentification);

	bool hasSufficientPermissions = clientClearance >= requiredClearance;

	if (hasSufficientPermissions)
	{
		ReplyToRequestSource(DatabaseReply::enuStatus::SUCCESS);
		return;
	}

	ReplyToRequestSource(DatabaseReply::enuStatus::INSUFFICIENT_PERMISSIONS);
}


void GuestAccessControl::Log()
{
	const InputParameter& userCredentials = m_pRequest->getParameterAt(0);
	const CommandMessage::enuCommand requestCommand = m_pRequest->getCommandId();

	m_resources.m_pDatabaseObject->CreateLog(requestCommand, userCredentials);
}



