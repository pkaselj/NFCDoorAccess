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

#ifndef TABLES_HPP
#define TABLES_HPP

#include<string> // ?
#include<vector>
#include "Database.hpp"
#include "propertiesclass.h"

using Clearance = signed char;
using UID = unsigned int;

// TODO make static?

class ITable
{
public:
	ITable(const std::string& name, Database* pDatabase, ILogger* pLogger = NulLogger::getInstance());

	virtual ~ITable() {}

	virtual void initialize() = 0;

	std::string getName() const { return m_tableName; }

	void checkIfDatabaseIsInitialized();

protected:
	std::string m_tableName;
	Database* m_pDatabase;
	ILogger* m_pLogger;
};

ITable::ITable(const std::string& name, Database* pDatabase, ILogger* pLogger)
	: m_tableName(name), m_pDatabase(pDatabase), m_pLogger(pLogger)
{
	if (m_pLogger == nullptr)
	{
		m_pLogger = NulLogger::getInstance();
	}

	if (m_pDatabase == nullptr)
	{
		*m_pLogger << "Database pointer invalid - " + m_tableName;
		Kernel::Fatal_Error("Database pointer invalid - " + m_tableName);
	}
}

void ITable::checkIfDatabaseIsInitialized()
{
	if (m_pDatabase == nullptr)
	{
		*m_pLogger << m_tableName + " - Database not properly initialized() (nullptr)!";
		Kernel::Fatal_Error(m_tableName + " - Database not properly initialized() (nullptr)!");
	}
}

class EmployeesTable : public ITable
{
public:
	EmployeesTable(Database* pDatabase, ILogger* pLogger = NulLogger::getInstance())
		: ITable(GlobalProperties::Get().EMPLOYEES_TABLE_NAME, pDatabase, pLogger),
		m_idColName(GlobalProperties::Get().EMPLOYEES_TABLE_ID_COLUMN_NAME),
		m_nameColName(GlobalProperties::Get().EMPLOYEES_TABLE_NAME_COLUMN_NAME),
		m_clearanceColName(GlobalProperties::Get().EMPLOYEES_TABLE_CLEARANCE_COLUMN_NAME)
	{ }
	
	~EmployeesTable();

	std::string getIDColName() const { return m_idColName; }
	std::string getNameColName() const { return m_nameColName; }
	std::string getClearanceColName() const { return m_clearanceColName; }

	void initialize();

	Clearance SelectClearanceWhereName(const std::string& name);
	Clearance SelectClearanceWhereId(UID id);
	std::string SelectNameWhereId(UID id);
	UID SelectIdWhereName(const std::string& name);
	void Add(const std::string& name, Clearance clearance);
	void DeleteWhereName(const std::string& name);
	void DeleteWhereId(UID id);
	void UpdateNameWhereName(const std::string& name, const std::string& newName);
	void UpdateNameWhereId(UID id, const std::string& newName);
	void UpdateClearanceWhereName(const std::string& name, Clearance newClearance);
	void UpdateClearanceWhereId(UID id, Clearance newClearance);
	bool ExistsName(const std::string& name);
	bool ExistsId(UID id);

private:

	void prepareSelectClearanceWhereName();
	void prepareSelectClearanceWhereId();
	void prepareSelectNameWhereId();
	void prepareSelectIdWhereName();
	void prepareAdd();
	void prepareDeleteWhereName();
	void prepareDeleteWhereId();
	void prepareUpdateNameWhereName();
	void prepareUpdateNameWhereId();
	void prepareUpdateClearanceWhereName();
	void prepareUpdateClearanceWhereId();
	void prepareExistsName();
	void prepareExistsId();


	const std::string m_idColName;
	const std::string m_nameColName;
	const std::string m_clearanceColName;

	statement* m_pSelectClearanceWhereName;
	statement* m_pSelectClearanceWhereId;
	statement* m_pSelectNameWhereId;
	statement* m_pSelectIdWhereName;
	statement* m_pAdd;
	statement* m_pDeleteWhereName;
	statement* m_pDeleteWhereId;
	statement* m_pUpdateNameWhereName;
	statement* m_pUpdateNameWhereId;
	statement* m_pUpdateClearanceWhereName;
	statement* m_pUpdateClearanceWhereId;
	statement* m_pExistsName;
	statement* m_pExistsId;

};

class KeypadPassTable : public ITable
{
public:
	KeypadPassTable(Database* pDatabase, ILogger* pLogger = NulLogger::getInstance())
		: ITable(GlobalProperties::Get().KEYPAD_PASS_TABLE_NAME, pDatabase, pLogger),
		m_idColName(GlobalProperties::Get().KEYPAD_PASS_TABLE_ID_COLUMN_NAME),
		m_passwordColName(GlobalProperties::Get().KEYPAD_PASS_TABLE_PASSWORD_COLUMN_NAME),
		m_ownerColName(GlobalProperties::Get().KEYPAD_PASS_TABLE_OWNER_COLUMN_NAME)
	{}
	~KeypadPassTable();

	std::string getIDColName() const { return m_idColName; }
	std::string getPasswordColName() const { return m_passwordColName; }
	std::string getOwnerColName() const { return m_ownerColName; }

	void initialize();

	void Add(const std::string& password, UID ownerId = 0);
	void DeleteWhereOwnerId(UID ownerId);
	void DeleteWherePassword(const std::string& password);
	UID SelectOwnerId(const std::string& password);
	bool ExistsPassword(const std::string& password);

private:

	void prepareAdd();
	void prepareDeleteWhereOwnerId();
	void prepareDeleteWherePassword();
	void prepareSelectOwnerId();
	void prepareExistsPassword();

	const std::string m_idColName;
	const std::string m_passwordColName;
	const std::string m_ownerColName;

	statement* m_pAdd;
	statement* m_pDeleteWhereOwnerId;
	statement* m_pDeleteWherePassword;
	statement* m_pSelectOwnerId;
	statement* m_pExistsPassword;
};

class CommandsTable : public ITable
{
public:
	CommandsTable(Database* pDatabase, ILogger* pLogger = NulLogger::getInstance())
		: ITable(GlobalProperties::Get().COMMANDS_TABLE_NAME, pDatabase, pLogger),
		m_IDColName(GlobalProperties::Get().COMMANDS_TABLE_ID_COLUMN_NAME),
		m_CommandColName(GlobalProperties::Get().COMMANDS_TABLE_COMMAND_COLUMN_NAME),
		m_ClearanceColName(GlobalProperties::Get().COMMANDS_TABLE_CLEARANCE_COLUMN_NAME)
	{}
	~CommandsTable();

	std::string getIDColName() const { return m_IDColName; }
	std::string getCommandColName() const { return m_CommandColName; }
	std::string getClearanceColName() const { return m_ClearanceColName; }

	void initialize();

	Clearance SelectClearance(const std::string& command);
	unsigned int SelectId(const std::string& command);

private:

	void prepareSelectClearance();
	void prepareSelectId();

	const std::string m_IDColName;
	const std::string m_CommandColName;
	const std::string m_ClearanceColName;

	statement* m_pSelectClearance;
	statement* m_pSelectId;
};

class RFIDCardTable : public ITable
{
public:
	RFIDCardTable(Database* pDatabase, ILogger* pLogger = NulLogger::getInstance())
		: ITable(GlobalProperties::Get().RFID_CARD_TABLE_NAME, pDatabase, pLogger),
		m_IDColName(GlobalProperties::Get().RFID_CARD_TABLE_ID_COLUMN_NAME),
		m_CardUUIDColName(GlobalProperties::Get().RFID_CARD_TABLE_CARD_UUID_COLUMN_NAME),
		m_OwnerColName(GlobalProperties::Get().RFID_CARD_TABLE_OWNER_COLUMN_NAME)
	{}
	~RFIDCardTable();

	std::string getIDColName() const { return m_IDColName; }
	std::string getCardUUIDColName() const { return m_CardUUIDColName; }
	std::string getOwnerColName() const { return m_OwnerColName; }

	void initialize();

	void Add(const std::string& cardUUID, UID ownerId = 0);
	void DeleteWhereOwnerId(UID ownerId);
	void DeleteWhereCardUUID(const std::string& cardUUID);
	UID SelectWhereCardUUID(const std::string& cardUUID);
	bool ExistsCardUUID(const std::string& cardUUID);

private:

	void prepareAdd();
	void prepareDeleteWhereOwnerId();
	void prepareDeleteWhereCardUUID();
	void prepareSelectWhereCardUUID();
	void prepareExistsCardUUID();


	const std::string m_IDColName;
	const std::string m_CardUUIDColName;
	const std::string m_OwnerColName;

	statement* m_pAdd;
	statement* m_pDeleteWhereOwnerId;
	statement* m_pDeleteWhereCardUUID;
	statement* m_pSelectWhereCardUUID;
	statement* m_pExistsCardUUID;

};


// =========================================== UNUSED
class WebAPITable : public ITable
{
public:
	WebAPITable(Database* pDatabase, ILogger* pLogger = NulLogger::getInstance()) : ITable("WebPassTable", pDatabase, pLogger) {}
	~WebAPITable();

	std::string getIDColName() const { return m_IDColName; }
	std::string getWebPassColName() const { return m_WebPassColName; }
	std::string getUserIdColName() const { return m_UserIdColName; }

	void initialize();

	void Add(UID userId, const std::string& password);
	UID SelectUserId(const std::string& password);
	void UpdateWebPass(UID userId, const std::string& newPassword);
	void DeleteWhereUserId(UID userId);
	bool ExistsPassword(const std::string& password);

private:

	void prepareAdd();
	void prepareSelectUserId();
	void prepareUpdateWebPass();
	void prepareDeleteWhereUserId();
	void prepareExistsPassword();

	const std::string m_IDColName = "Id";
	const std::string m_WebPassColName = "WebPassHash";
	const std::string m_UserIdColName = "UserId";

	statement* m_pAdd;
	statement* m_pSelectUserId;
	statement* m_pUpdateWebPass;
	statement* m_pDeleteWhereUserId;
	statement* m_pExistsPassword;
};

// =============================================================================


struct LogEntry
{
	std::string m_timestamp = "";
	unsigned int m_userId = 0;
	std::string m_authMethod = "";
	unsigned int m_commandId = 0;
};

// CONSTANTS ===================================== TODO
const unsigned int DEFAULT_LOG_ROWS_LIMIT = 50;
const unsigned int MAX_LOG_ROWS_LIMIT = 500;
// =====================================================


class LogTable : public ITable
{
public:
	LogTable(Database* pDatabase, ILogger* pLogger = NulLogger::getInstance())
		: ITable(GlobalProperties::Get().LOG_TABLE_NAME, pDatabase, pLogger),
		m_IdColName(GlobalProperties::Get().LOG_TABLE_ID_COLUMN_NAME),
		m_TimestampColName(GlobalProperties::Get().LOG_TABLE_TIMESTAMP_COLUMN_NAME),
		m_UserIdColName(GlobalProperties::Get().LOG_TABLE_USER_ID_COLUMN_NAME),
		m_AuthMethodColName(GlobalProperties::Get().LOG_TABLE_AUTH_METHOD_COLUMN_NAME),
		m_CommandIdColName(GlobalProperties::Get().LOG_TABLE_COMMAND_ID_COLUMN_NAME)
	{}
	~LogTable();

	std::string getIdColName() const { return m_IdColName; }
	std::string getTimestampColName() const { return m_TimestampColName; }
	std::string getUserIdColName() const { return m_UserIdColName; }
	std::string getAuthMethodColName() const { return m_AuthMethodColName; }
	std::string getCommandIdColName() const { return m_CommandIdColName; }

	void initialize();

	void CreateLog(const LogEntry& logEntry);
	std::vector<LogEntry> SelectLogs(unsigned int limit = DEFAULT_LOG_ROWS_LIMIT);

private:

	void prepareCreateLog();
	void prepareSelectLogs();

	const std::string m_IdColName;
	const std::string m_TimestampColName;
	const std::string m_UserIdColName;
	const std::string m_AuthMethodColName;
	const std::string m_CommandIdColName;

	statement* m_pCreateLog;
	statement* m_pSelectLogs;
};

#endif