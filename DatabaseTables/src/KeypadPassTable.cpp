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

#include<sstream>


KeypadPassTable::~KeypadPassTable()
{
	delete m_pAdd;
	delete m_pDeleteWhereOwnerId;
	delete m_pDeleteWherePassword;
	delete m_pSelectOwnerId;
	delete m_pExistsPassword;
}

void KeypadPassTable::initialize()
{
	prepareAdd();
	prepareDeleteWhereOwnerId();
	prepareDeleteWherePassword();
	prepareSelectOwnerId();
	prepareExistsPassword();
}

void KeypadPassTable::prepareAdd()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		INSERT INTO KeypadPasswordTable(PasswordHash, PassOwnerId)
		VALUES (?, ?);

	  ======================================================*/

	queryStringBuilder
		<< "INSERT INTO " << m_tableName << "(" << m_passwordColName << "," << m_ownerColName << ")" << " "
		<< "VALUES " << "(?, ?)" << ";";


	std::string queryString = queryStringBuilder.str();
	m_pAdd = new statement(queryString);
}


void KeypadPassTable::prepareDeleteWhereOwnerId()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		DELETE FROM KeypadPasswordTable
		WHERE PassOwnerId == ?;

	======================================================*/

	queryStringBuilder
		<< "DELETE FROM " << m_tableName << " "
		<< "WHERE " << m_ownerColName << " == " << "?" << ";";

	std::string queryString = queryStringBuilder.str();
	m_pDeleteWhereOwnerId = new statement(queryString);

}

void KeypadPassTable::prepareDeleteWherePassword()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		DELETE FROM KeypadPasswordTable
		WHERE PasswordHash == ?;

	======================================================*/

	queryStringBuilder
		<< "DELETE FROM " << m_tableName << " "
		<< "WHERE " << m_passwordColName << " == " << "?" << ";";

	std::string queryString = queryStringBuilder.str();
	m_pDeleteWherePassword = new statement(queryString);
}

void KeypadPassTable::prepareSelectOwnerId()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		SELECT PassOwnerId
		FROM KeypadPasswordTable
		WHERE PasswordHash == ?;

	======================================================*/

	queryStringBuilder
		<< "SELECT " << m_ownerColName << " "
		<< "FROM " << m_tableName << " "
		<< "WHERE " << m_passwordColName << " == " << "?" << ";";

	std::string queryString = queryStringBuilder.str();
	m_pSelectOwnerId = new statement(queryString);
}

void KeypadPassTable::prepareExistsPassword()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		SELECT EXISTS
		(
			SELECT PasswordHash
			FROM KeypadPasswordTable
			WHERE PasswordHash == ?
		);

	  ======================================================*/

	queryStringBuilder
		<< "SELECT EXISTS " << "("
		<< "SELECT " << m_passwordColName << " "
		<< "FROM " << m_tableName << " "
		<< "WHERE " << m_passwordColName << " == " << "?" << " "
		<< ")" << ";";


	std::string queryString = queryStringBuilder.str();
	m_pExistsPassword = new statement(queryString);
}

void KeypadPassTable::Add(const std::string& password, UID ownerId)
{
	checkIfDatabaseIsInitialized();

	m_pDatabase->NewQuery(*m_pAdd);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), password);

	if (ownerId != 0)
	{
		pCurrentQuery->bind(ARGUMENT(1), (int)ownerId);
	}

	pCurrentQuery->next();

}
void KeypadPassTable::DeleteWhereOwnerId(UID ownerId)
{
	checkIfDatabaseIsInitialized();

	m_pDatabase->NewQuery(*m_pDeleteWhereOwnerId);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), (int)ownerId);

	pCurrentQuery->next();

}

void KeypadPassTable::DeleteWherePassword(const std::string& password)
{
	checkIfDatabaseIsInitialized();

	m_pDatabase->NewQuery(*m_pDeleteWherePassword);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), password);

	pCurrentQuery->next();

}

UID KeypadPassTable::SelectOwnerId(const std::string& password)
{
	checkIfDatabaseIsInitialized();

	m_pDatabase->NewQuery(*m_pSelectOwnerId);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), password);

	int ownerId = 0;

	if (pCurrentQuery->next() == true)
	{
		pCurrentQuery->get(ownerId, COLUMN(0));
	}

	return ownerId;

}


bool KeypadPassTable::ExistsPassword(const std::string& password)
{
	checkIfDatabaseIsInitialized();

	m_pDatabase->NewQuery(*m_pExistsPassword);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), password);

	int exists = 0;
	if (pCurrentQuery->next() == true)
	{
		pCurrentQuery->get(exists, COLUMN(0));
	};

	return (bool)exists;
}