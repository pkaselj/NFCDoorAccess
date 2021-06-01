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

#include "DatabaseObject.hpp"

#include "ValidationUtils.hpp"

#include <sstream>

std::string decodeParamType(InputParameter::enuType paramType);
std::string getTimeSeed(unsigned int accuracy);

DatabaseObject::DatabaseObject(const std::string& DB_path, DatabaseResources& resources, ILogger* pLogger)
	:	m_path(DB_path), m_writeLock(), m_pLogger(pLogger), m_resources(resources), m_database(DB_path, pLogger) // TODO what if path and pLogger are invalid
{
	if (m_pLogger == nullptr)
	{
		m_pLogger = NulLogger::getInstance();
	}

	if (resources.m_pMailbox == nullptr)
	{
		*m_pLogger << "DatabaseObject - mailbox cannot be nullptr!";
		Kernel::Fatal_Error("DatabaseObject - mailbox cannot be nullptr!");
	}

	if (resources.m_pLogMailbox == nullptr)
	{
		*m_pLogger << "DatabaseObject -- pointer to pLogMailbox cannot be null!";
		Kernel::Fatal_Error("DatabaseRequestFactory -- pointer to pLogMailbox cannot be null!");
	}

	initialize();
}

DatabaseObject::~DatabaseObject()
{
	// TODO change dynamic allocation to static allocation

	delete m_pEmployeesTable;
	delete m_pKeypadPassTable;
	delete m_pCommandsTable;
	delete m_pRFIDCardTable;
	delete m_pWebAPITable;
	delete m_pLogTable;
}


void DatabaseObject::initialize()
{
	initializeTables();
	prepareStatements();

	m_initialized = test();
	if (m_initialized == false)
	{
		*m_pLogger << "Database initialization test failed!";
		Kernel::Fatal_Error("Database initialization test failed!"); // TODO more descriptive log
	}
}

bool DatabaseObject::test()
{
	/************************************************************
	*   Xn(var) = (var == nullptr)						        *
	*															*
	*   anyNullptrPresent = !X1 && !X2 && !X3 && ... && !Xn	    *
	*					  = (deMorgan)                          *
	*					  = !(X1 || X2 || X3 || ... || Xn)	    *
	*************************************************************/

	bool anyNullptrPresent = m_pEmployeesTable == nullptr ||
		m_pKeypadPassTable == nullptr ||
		m_pCommandsTable == nullptr ||
		m_pRFIDCardTable == nullptr ||
		m_pWebAPITable == nullptr ||
		m_pLogTable == nullptr;

	return !anyNullptrPresent;
							
}

void DatabaseObject::checkIfDatabaseIsInitialized()
{
	if (m_initialized == false)
	{
		*m_pLogger << "Trying to use uninitialized database object!";
		Kernel::Fatal_Error("Trying to use uninitialized database object!");
	}
}


void DatabaseObject::prepareStatements()
{

}


void DatabaseObject::initializeTables()
{
	m_pEmployeesTable = new EmployeesTable(&m_database, m_pLogger);
	m_pEmployeesTable->initialize();

	m_pKeypadPassTable = new KeypadPassTable(&m_database, m_pLogger);
	m_pKeypadPassTable->initialize();

	m_pCommandsTable = new CommandsTable(&m_database, m_pLogger);
	m_pCommandsTable->initialize();

	m_pRFIDCardTable = new RFIDCardTable(&m_database, m_pLogger);
	m_pRFIDCardTable->initialize();

	m_pWebAPITable = new WebAPITable(&m_database, m_pLogger);
	m_pWebAPITable->initialize();

	m_pLogTable = new LogTable(&m_database, m_pLogger);
	m_pLogTable->initialize();

}


void DatabaseObject::CreateLog(CommandMessage::enuCommand command, const InputParameter& userCredentials)
{
	// std::string s contained in LogEntry contain garbage value
	// when sent via a message queue even inside the same process
	// (probably uses stack local variables)
	// Make sure that pLogEntry is free d on receiving side!
	LogEntry* pLogEntry = new LogEntry;

	*pLogEntry = parseInputParameterToLogEntry(userCredentials);
	std::string commandName = parseCommand(command);

	pLogEntry->m_timestamp = Time::getDateTime_ISO8601();
	pLogEntry->m_commandId = m_pCommandsTable->SelectId(commandName);


	m_resources.m_pLogMailbox->sendConnectionless(m_resources.m_refLogThread, (char*)&pLogEntry, sizeof(LogEntry*));

	return;
	
}

void DatabaseObject::WriteLogToLogTable(LogEntry& logEntry)
{
	std::unique_lock<std::mutex> writeLock(m_writeLock);
	m_pLogTable->CreateLog(logEntry);
}

LogEntry DatabaseObject::parseInputParameterToLogEntry(const InputParameter& param)
{
	LogEntry logEntry;

	InputParameter::enuType paramType = param.getType();
	logEntry.m_authMethod = decodeParamType(paramType);
	logEntry.m_userId = getUserId(param);

	return logEntry;
}

// TODO new name
std::string decodeParamType(InputParameter::enuType paramType)
{
	switch (paramType)
	{
	case InputParameter::enuType::KeypadPIN:
		return "PIN";

	case InputParameter::enuType::RFIDCard:
		return "Card";

	// case InputParameter::enuWebPass: // TODO ADD
	}

	Kernel::Fatal_Error("decodeParamType(...) from DatabaseObject -> invalid parameter type [ " + std::to_string((int)paramType) + " ]!");
	return "";
}


Clearance DatabaseObject::getClearanceFromPassword(const std::string& password)
{
	unsigned int ownerId = m_pKeypadPassTable->SelectOwnerId(password);
	return m_pEmployeesTable->SelectClearanceWhereId(ownerId);
}

Clearance DatabaseObject::getClearanceFromName(const std::string& name)
{
	return m_pEmployeesTable->SelectClearanceWhereName(name);
}

Clearance DatabaseObject::getClearanceFromRFIDCard(const std::string& uuid)
{
	unsigned int ownerId = m_pRFIDCardTable->SelectWhereCardUUID(uuid);
	return m_pEmployeesTable->SelectClearanceWhereId(ownerId);
}

unsigned int DatabaseObject::getUserIdFromPassword(const std::string& password)
{
	return m_pKeypadPassTable->SelectOwnerId(password);
}

unsigned int DatabaseObject::getUserIdFromRFIDCard(const std::string& uuid)
{
	return m_pRFIDCardTable->SelectWhereCardUUID(uuid);
}

Clearance DatabaseObject::getRequiredClearanceForCommand(CommandMessage::enuCommand command)
{
	std::string commandName = parseCommand(command);
	if (commandName == "")
	{
		return MAX_CLEARANCE;
	}

	Clearance requiredClearance = m_pCommandsTable->SelectClearance(commandName);

	return requiredClearance;
}

std::string DatabaseObject::parseCommand(CommandMessage::enuCommand command)
{
	switch (command)
	{
	case CommandMessage::enuCommand::AUTHENTICATE:
		return "AUTHENTICATE";

	case CommandMessage::enuCommand::ADD:
		return "ADD";

	case CommandMessage::enuCommand::REMOVE:
		return "REMOVE";

	case CommandMessage::enuCommand::SET_CLNC:
		return "SET_CLEARANCE";

	case CommandMessage::enuCommand::GUEST_ACCESS_ENABLE:
	case CommandMessage::enuCommand::GUEST_ACCESS_DISABLE:
		return "GUEST_ACCESS_CTL";
	}

	*m_pLogger << "Invalid command type for authorization: (CommandMessage::enuCommand int)" + (int)command;

	return "";
}

Clearance DatabaseObject::getClearance(const InputParameter& authorizationParameter)
{
	InputParameter::enuType parameterType = authorizationParameter.getType();
	const std::string parameterData = authorizationParameter.getData();

	switch (parameterType)
	{
	case InputParameter::enuType::KeypadPIN:
		return getClearanceFromPassword(parameterData);

	case InputParameter::enuType::RFIDCard:
		return getClearanceFromRFIDCard(parameterData);

	}

	*m_pLogger << "Invalid parameter type for authorization: " + authorizationParameter.getInfo();

	// TODO default clearance
	return NO_CLEARANCE;

}

unsigned int DatabaseObject::getUserId(const InputParameter& param)
{
	InputParameter::enuType paramType = param.getType();
	const std::string& paramData = param.getData();

	switch (paramType)
	{
	case InputParameter::enuType::KeypadPIN:
		return getUserIdFromPassword(paramData);

	case InputParameter::enuType::RFIDCard:
		return getUserIdFromRFIDCard(paramData);

	}

	Kernel::Fatal_Error("Database Object - getUserId(...) -> invalid parameter type [ " + std::to_string((int)paramType) + " ]");
	return 0;
}

bool DatabaseObject::AddIdentifier(const InputParameter& parameterToAdd, Clearance clearance, UID ownerId)
{
	InputParameter::enuType parameterType = parameterToAdd.getType();
	std::string parameterData = parameterToAdd.getData();

	std::unique_lock<std::mutex> writeLock(m_writeLock);

	switch (parameterType)
	{
	case InputParameter::enuType::KeypadPIN:
		return AddPassword(parameterData, clearance, ownerId);

	case InputParameter::enuType::RFIDCard:
		return AddCard(parameterData, clearance, ownerId);
	}

	writeLock.unlock();

	*m_pLogger << "Invalid input parameter type tp ADD!";

	return false;
}

bool DatabaseObject::AddCard(const std::string& uuid, Clearance clearance, UID ownerId)
{
	bool isUUIDValid = isValidCardUUID(uuid);
	if (isUUIDValid == false)
	{
		*m_pLogger << "CardUUID is invalid. Cannot preform ADD.";
		return false;
	}

	bool Card_AlreadyExists = m_pRFIDCardTable->ExistsCardUUID(uuid);
	if (Card_AlreadyExists == true)
	{
		*m_pLogger << "Card already exists. Cannot preform ADD.";
		return false;
	}

	if (ownerId == 0)
	{
		ownerId = createNewUniqueGuest(clearance);
	}
	else
	{

		// Duplication to skip unnecessary SQL queries
		bool ownerExists = m_pEmployeesTable->ExistsId(ownerId);
		if (ownerExists == true)
		{
			*m_pLogger << "Owner already exists. Cannot preform ADD.";
			return false;
		}
		else
		{
			ownerId = createNewUniqueGuest(clearance);
		}
	}

	m_pRFIDCardTable->Add(uuid, ownerId);

	// TODO add check if card was really added

	return true;

}

UID DatabaseObject::createNewUniqueGuest(Clearance clearance)
{
	bool ownerExists = true;
	std::string newOwnerName = "";
	do
	{
		newOwnerName = "GUEST_" + getTimeSeed(5);
		ownerExists = m_pEmployeesTable->ExistsName(newOwnerName);
	} while (ownerExists);

	m_pEmployeesTable->Add(newOwnerName, clearance);

	UID newOwnerId = m_pEmployeesTable->SelectIdWhereName(newOwnerName);
	return newOwnerId;
}

std::string getTimeSeed(unsigned int accuracy)
{
	int timeSeed = Time::getRawTime().tv_nsec % accuracy;
	return std::to_string(timeSeed);
}

bool DatabaseObject::AddPassword(const std::string& password, Clearance clearance, UID ownerId)
{
	bool isPINValid = isValidKeypadPassword(password);
	if (isPINValid == false)
	{
		*m_pLogger << "PIN is invalid. Cannot preform ADD.";
		return false;
	}

	bool PIN_AlreadyExists = m_pKeypadPassTable->ExistsPassword(password);
	if (PIN_AlreadyExists == true)
	{
		*m_pLogger << "PIN already exists. Cannot preform ADD.";
		return false;
	}


	if (ownerId == 0)
	{
		ownerId = createNewUniqueGuest(clearance);
	}
	else
	{

		// Duplication to skip unnecessary SQL queries
		bool ownerExists = m_pEmployeesTable->ExistsId(ownerId);
		if (ownerExists == true)
		{
			*m_pLogger << "Owner already exists. Cannot preform ADD.";
			return false;
		}
		else
		{
			ownerId = createNewUniqueGuest(clearance);
		}
	}

	m_pKeypadPassTable->Add(password, ownerId);

	// TODO add check if PIN was really added

	return true;
}

bool DatabaseObject::RemoveIdentifier(const InputParameter& parameterToRemove)
{
	InputParameter::enuType paramType = parameterToRemove.getType();
	std::string paramData = parameterToRemove.getData();

	std::unique_lock<std::mutex> writeLock(m_writeLock);

	switch (paramType)
	{
	case InputParameter::enuType::KeypadPIN:
		return RemovePassword(paramData);

	case InputParameter::enuType::RFIDCard:
		return RemoveCard(paramData);
	}

	writeLock.unlock();

	*m_pLogger << "Invalid input type to REMOVE request!";

	return false;
}

bool DatabaseObject::RemoveCard(const std::string& uuid)
{
	bool bCardValid = isValidCardUUID(uuid);
	if (bCardValid == false)
	{
		return false;
	}

	m_pRFIDCardTable->DeleteWhereCardUUID(uuid);

	// TODO add check if card was really deleted

	return true;

}

bool DatabaseObject::RemovePassword(const std::string& password)
{
	bool bPINValid = isValidKeypadPassword(password);
	if (bPINValid == false)
	{
		return false;
	}

	m_pKeypadPassTable->DeleteWherePassword(password);

	// TODO add check if pass was really deleted

	return true;
}

bool DatabaseObject::SetClearance(const InputParameter& targetCredentials, Clearance newClearance)
{
	InputParameter::enuType paramType = targetCredentials.getType();
	std::string paramData = targetCredentials.getData();

	std::unique_lock<std::mutex> writeLock(m_writeLock);

	switch (paramType)
	{
	case InputParameter::enuType::KeypadPIN:
		return SetClearanceForPassword(paramData, newClearance);

	case InputParameter::enuType::RFIDCard:
		return SetClearanceForCard(paramData, newClearance);
	}

	writeLock.unlock();

	*m_pLogger << "Invalid input type to SET_CLEARANCE request!";

	return false;
}

bool DatabaseObject::SetClearanceForCard(const std::string& cardUUID, Clearance newClearance)
{
	if (isValidClearance(newClearance) == false)
	{
		*m_pLogger << "Clearance not from valid interval [-1, " + std::to_string((int)MAX_CLEARANCE) + "], value: " + std::to_string((int)newClearance);
		return false;
	}

	/*
	
	// Redundant because m_pRFIDCardTable->SelectWhereCardUUID() returns 0 if card does not exist
	
		bool cardExists = m_pRFIDCardTable->ExistsCardUUID(cardUUID);
	if (cardExists == false)
	{
		*m_pLogger << "Specified card does not exist in the database!";
		return false;
	}

	*/

	UID ownerId = m_pRFIDCardTable->SelectWhereCardUUID(cardUUID);
	if (ownerId == 0)
	{
		*m_pLogger << "Specified card does not exist in the database!";
		return false;
	}

	m_pEmployeesTable->UpdateClearanceWhereId(ownerId, newClearance);

	return true;

}

bool DatabaseObject::SetClearanceForPassword(const std::string& password, Clearance newClearance)
{
	if (isValidClearance(newClearance) == false)
	{
		*m_pLogger << "Clearance not from valid interval [-1, " + std::to_string((int)MAX_CLEARANCE) + "], value: " + std::to_string((int)newClearance);
		return false;
	}

	UID ownerId = m_pKeypadPassTable->SelectOwnerId(password);
	if (ownerId == 0)
	{
		*m_pLogger << "Specified card does not exist in the database!";
		return false;
	}

	m_pEmployeesTable->UpdateClearanceWhereId(ownerId, newClearance);

	return true;
}