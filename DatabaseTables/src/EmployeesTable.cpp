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

#include <sstream>

EmployeesTable::~EmployeesTable()
{
	delete m_pSelectClearanceWhereName;
	delete m_pSelectClearanceWhereId;
	delete m_pSelectNameWhereId;
	delete m_pSelectIdWhereName;
	delete m_pAdd;
	delete m_pDeleteWhereName;
	delete m_pDeleteWhereId;
	delete m_pUpdateNameWhereName;
	delete m_pUpdateNameWhereId;
	delete m_pUpdateClearanceWhereName;
	delete m_pUpdateClearanceWhereId;
	delete m_pExistsName;
	delete m_pExistsId;
}

void EmployeesTable::initialize()
{
	prepareSelectClearanceWhereName();
	prepareSelectClearanceWhereId();
	prepareSelectNameWhereId();
	prepareSelectIdWhereName();
	prepareAdd();
	prepareDeleteWhereName();
	prepareDeleteWhereId();
	prepareUpdateNameWhereName();
	prepareUpdateNameWhereId();
	prepareUpdateClearanceWhereName();
	prepareUpdateClearanceWhereId();
	prepareExistsName();
	prepareExistsId();
}

void EmployeesTable::prepareSelectClearanceWhereName()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";

	/*======================================================
		
		SELECT Clearance
		FROM Employees
		WHERE Name == ?;
	
	  ======================================================*/

	queryStringBuilder
		<< "SELECT " << m_clearanceColName << " "
		<< "FROM "	 << m_tableName		<< " "
		<< "WHERE "  << m_nameColName		<< " == " << "?" << ";";

	std::string queryString = queryStringBuilder.str();

	m_pSelectClearanceWhereName = new statement(queryString);
}

void EmployeesTable::prepareSelectClearanceWhereId()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================
		
		
		SELECT Clearence
		FROM Employees
		WHERE EmployeeId == ?;


	  ======================================================*/

	queryStringBuilder
		<< "SELECT " << m_clearanceColName << " "
		<< "FROM "   << m_tableName << " "
		<< "WHERE "  << m_idColName << " == " << "?" << ";";


	std::string queryString = queryStringBuilder.str();
	m_pSelectClearanceWhereId = new statement(queryString);
}

void EmployeesTable::prepareSelectNameWhereId()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		SELECT Name
		FROM Employees
		WHERE EmployeeId == ?;

	  ======================================================*/

	queryStringBuilder
		<< "SELECT " << m_nameColName << " "
		<< "FROM "   << m_tableName << " "
		<< "WHERE "  << m_idColName << " == " << "?" << ";";


	std::string queryString = queryStringBuilder.str();
	m_pSelectNameWhereId = new statement(queryString);
}

void EmployeesTable::prepareSelectIdWhereName()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		SELECT EmployeeId
		FROM Employees
		WHERE Name == ?;

	  ======================================================*/

	queryStringBuilder
		<< "SELECT " << m_idColName << " "
		<< "FROM "   << m_tableName << " "
		<< "WHERE "  << m_nameColName << " == " << "?" << ";";


	std::string queryString = queryStringBuilder.str();
	m_pSelectIdWhereName = new statement(queryString);
}

void EmployeesTable::prepareAdd()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		INSERT INTO Employees(Name, Clearance)
		VALUES (?, ?);
		
		-- TODO Multiple inserts one transaction
	  ======================================================*/

	queryStringBuilder
		<< "INSERT INTO " << m_tableName << "(" << m_nameColName << "," << m_clearanceColName << ")" << " "
		<< "VALUES " << "(?, ?)";

	std::string queryString = queryStringBuilder.str();
	m_pAdd = new statement(queryString);
}

void EmployeesTable::prepareDeleteWhereName()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		DELETE FROM Employees
		WHERE Name == ?;

	  ======================================================*/

	queryStringBuilder
		<< "DELETE FROM " << m_tableName << " "
		<< "WHERE " << m_nameColName << " == " << "?" << ";";


	std::string queryString = queryStringBuilder.str();
	m_pDeleteWhereName = new statement(queryString);
}

void EmployeesTable::prepareDeleteWhereId()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		DELETE FROM Employees
		WHERE EmployeeId == ?;

	  ======================================================*/

	queryStringBuilder
		<< "DELETE FROM " << m_tableName << " "
		<< "WHERE " << m_idColName << " == " << "?" << ";";


	std::string queryString = queryStringBuilder.str();
	m_pDeleteWhereId = new statement(queryString);
}

void EmployeesTable::prepareUpdateNameWhereName()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		UPDATE Employees
		SET Name = ?
		WHERE Name == ?;


	  ======================================================*/

	queryStringBuilder
		<< "UPDATE " << m_tableName << " "
		<< "SET " << m_nameColName << " = " << "?" << " "
		<< "WHERE " << m_nameColName << " == " << "?" << ";";


	std::string queryString = queryStringBuilder.str();
	m_pUpdateNameWhereName = new statement(queryString);
}

void EmployeesTable::prepareUpdateNameWhereId()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		UPDATE Employees
		SET Name = ?
		WHERE EmployeeId == ?;

	  ======================================================*/

	queryStringBuilder
		<< "UPDATE " << m_tableName << " "
		<< "SET " << m_nameColName << " = " << "?" << " "
		<< "WHERE " << m_idColName << " == " << "?" << ";";


	std::string queryString = queryStringBuilder.str();
	m_pUpdateNameWhereId = new statement(queryString);
}

void EmployeesTable::prepareUpdateClearanceWhereName()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		UPDATE Employees
		SET Clearance = ?
		WHERE Name == ?;

	  ======================================================*/

	queryStringBuilder
		<< "UPDATE " << m_tableName << " "
		<< "SET " << m_clearanceColName << " = " << "?" << " "
		<< "WHERE " << m_nameColName << " == " << "?" << ";";


	std::string queryString = queryStringBuilder.str();
	m_pUpdateClearanceWhereName = new statement(queryString);
}

void EmployeesTable::prepareUpdateClearanceWhereId()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		UPDATE Employees
		SET Clearance = ?
		WHERE EmployeeId == ?;

	  ======================================================*/

	queryStringBuilder
		<< "UPDATE " << m_tableName << " "
		<< "SET " << m_clearanceColName << " = " << "?" << " "
		<< "WHERE " << m_idColName << " == " << "?" << ";";


	std::string queryString = queryStringBuilder.str();
	m_pUpdateClearanceWhereId = new statement(queryString);
}

void EmployeesTable::prepareExistsName()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		SELECT EXISTS
		(
			SELECT Name
			FROM Employees
			WHERE Name == ?
		);

	  ======================================================*/

	queryStringBuilder
		<< "SELECT EXISTS " << "("
				<< "SELECT " << m_nameColName << " "
				<< "FROM " << m_tableName << " "
				<< "WHERE " << m_nameColName << " == " << "?" << " "
		<< ")" << ";";


	std::string queryString = queryStringBuilder.str();
	m_pExistsName = new statement(queryString);
}

void EmployeesTable::prepareExistsId()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		SELECT EXISTS
		(
			SELECT EmployeeId
			FROM Employees
			WHERE EmployeeId == ?
		);

	  ======================================================*/

	queryStringBuilder
		<< "SELECT EXISTS " << "("
			<< "SELECT " << m_idColName << " "
			<< "FROM " << m_tableName << " "
			<< "WHERE " << m_idColName << " == " << "?" << " "
		<< ")" << ";";


	std::string queryString = queryStringBuilder.str();
	m_pExistsId = new statement(queryString);
}

Clearance EmployeesTable::SelectClearanceWhereName(const std::string& name)
{
	checkIfDatabaseIsInitialized();

	m_pDatabase->NewQuery(*m_pSelectClearanceWhereName);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();
	
	pCurrentQuery->bind(ARGUMENT(0), name);

	int clearance = -1;

	if (pCurrentQuery->next() == true)
	{
		pCurrentQuery->get(clearance, COLUMN(0));
	}

	return clearance;
}

Clearance EmployeesTable::SelectClearanceWhereId(UID id)
{
	checkIfDatabaseIsInitialized();

	m_pDatabase->NewQuery(*m_pSelectClearanceWhereId);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), (int)id);

	int clearance = -1;

	if (pCurrentQuery->next() == true)
	{
		pCurrentQuery->get(clearance, COLUMN(0));
	}

	return clearance;
}

std::string EmployeesTable::SelectNameWhereId(UID id)
{
	checkIfDatabaseIsInitialized();

	m_pDatabase->NewQuery(*m_pSelectNameWhereId);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), (int)id);

	std::string result = "";

	if (pCurrentQuery->next() == true)
	{
		pCurrentQuery->get(result, COLUMN(0));
	}

	return result;
}

UID EmployeesTable::SelectIdWhereName(const std::string& name)
{
	checkIfDatabaseIsInitialized();

	m_pDatabase->NewQuery(*m_pSelectIdWhereName);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), name);

	int id = 0;

	if (pCurrentQuery->next() == true)
	{
		pCurrentQuery->get(id, COLUMN(0));
	}

	return id;


}

void EmployeesTable::Add(const std::string& name, Clearance clearance)
{
	checkIfDatabaseIsInitialized();

	m_pDatabase->NewQuery(*m_pAdd);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), name);
	pCurrentQuery->bind(ARGUMENT(1), (int)clearance);

	pCurrentQuery->next(); // execute
}

void EmployeesTable::DeleteWhereName(const std::string& name)
{
	checkIfDatabaseIsInitialized();

	m_pDatabase->NewQuery(*m_pDeleteWhereName);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), name);
	pCurrentQuery->next();

}

void EmployeesTable::DeleteWhereId(UID id)
{
	checkIfDatabaseIsInitialized();

	m_pDatabase->NewQuery(*m_pDeleteWhereId);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), (int)id);
	
	pCurrentQuery->next();
}

void EmployeesTable::UpdateNameWhereName(const std::string& name, const std::string& newName)
{
	checkIfDatabaseIsInitialized();

	m_pDatabase->NewQuery(*m_pUpdateNameWhereName);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), newName);
	pCurrentQuery->bind(ARGUMENT(1), name);

	pCurrentQuery->next();

}

void EmployeesTable::UpdateNameWhereId(UID id, const std::string& newName)
{
	checkIfDatabaseIsInitialized();

	m_pDatabase->NewQuery(*m_pUpdateNameWhereId);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), newName);
	pCurrentQuery->bind(ARGUMENT(1), (int)id);

	pCurrentQuery->next();
}

void EmployeesTable::UpdateClearanceWhereName(const std::string& name, Clearance newClearance)
{
	checkIfDatabaseIsInitialized();

	m_pDatabase->NewQuery(*m_pUpdateClearanceWhereName);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), (int)newClearance);
	pCurrentQuery->bind(ARGUMENT(1), name);

	pCurrentQuery->next();

}

void EmployeesTable::UpdateClearanceWhereId(UID id, Clearance newClearance)
{
	checkIfDatabaseIsInitialized();

	m_pDatabase->NewQuery(*m_pUpdateClearanceWhereId);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), (int)newClearance);
	pCurrentQuery->bind(ARGUMENT(1), (int)id);

	pCurrentQuery->next();
}

bool EmployeesTable::ExistsName(const std::string& name)
{
	checkIfDatabaseIsInitialized();

	m_pDatabase->NewQuery(*m_pExistsName);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), name);

	int exists = 0;
	if (pCurrentQuery->next() == true)
	{
		pCurrentQuery->get(exists, COLUMN(0));
	};

	return (bool)exists;
}

bool EmployeesTable::ExistsId(UID id)
{
	checkIfDatabaseIsInitialized();

	m_pDatabase->NewQuery(*m_pExistsId);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), (int)id);

	int exists = 0;
	if (pCurrentQuery->next() == true)
	{
		pCurrentQuery->get(exists, COLUMN(0));
	};

	return (bool)exists;
}
