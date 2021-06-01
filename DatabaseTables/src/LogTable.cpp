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

#include "Tables.hpp"
#include "Time.hpp"

#include <sstream>

LogTable::~LogTable()
{
	delete m_pCreateLog;
	delete m_pSelectLogs;
}

void LogTable::initialize()
{
	prepareCreateLog();
	prepareSelectLogs();
}

void LogTable::prepareCreateLog()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		INSERT INTO LogTable(Timestamp, UserId, AuthMethod, CommandId)
		VALUES (?, ?, ?, ?);

	  ======================================================*/

	queryStringBuilder
		<< "INSERT INTO " << m_tableName << "(" << m_TimestampColName << "," << m_UserIdColName << "," << m_AuthMethodColName << "," << m_CommandIdColName << ")" << " "
		<< "VALUES " << "(?, ?, ?, ?)" << ";";


	std::string queryString = queryStringBuilder.str();
	m_pCreateLog = new statement(queryString);
}

void LogTable::prepareSelectLogs()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		SELECT Timestamp, UserId, AuthMethod, CommandId
		FROM LogTable
		--LIMIT ?;

	  ======================================================*/

	queryStringBuilder
		<< "SELECT " << m_TimestampColName << ", " << m_UserIdColName << ", " << m_AuthMethodColName << ", " << m_CommandIdColName << " "
		<< "FROM " << m_tableName << " "
		<< "--LIMIT " << "?" << ";";


	std::string queryString = queryStringBuilder.str();
	m_pSelectLogs = new statement(queryString);
}

void LogTable::CreateLog(const LogEntry& logEntry)
{
	checkIfDatabaseIsInitialized();

	m_pDatabase->NewQuery(*m_pCreateLog);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), logEntry.m_timestamp );
	pCurrentQuery->bind(ARGUMENT(1), (int)logEntry.m_userId);
	pCurrentQuery->bind(ARGUMENT(2), logEntry.m_authMethod);
	pCurrentQuery->bind(ARGUMENT(3), (int)logEntry.m_commandId);

	pCurrentQuery->next();
}

std::vector<LogEntry> LogTable::SelectLogs(unsigned int limit)
{
	if (limit > MAX_LOG_ROWS_LIMIT)
	{
		*m_pLogger << "SelectLogs(...) limit exceeds MAX_LOG_ROWS_LIMIT";
		return {};
	}

	std::vector<LogEntry> results = {};

	m_pDatabase->NewQuery(*m_pSelectLogs);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	// pCurrentQuery->bind(ARGUMENT(0), limit);

	int i = 0;
	while (i < limit && pCurrentQuery->next())
	{
		LogEntry logEntry;

		pCurrentQuery->get(logEntry.m_timestamp, COLUMN(0));
		pCurrentQuery->get(logEntry.m_userId, COLUMN(1));
		pCurrentQuery->get(logEntry.m_authMethod, COLUMN(2));
		pCurrentQuery->get(logEntry.m_commandId, COLUMN(3));

		results.push_back(logEntry);

		++i;
	}


	return results;
}