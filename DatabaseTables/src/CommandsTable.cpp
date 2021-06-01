#include "Tables.hpp"

#include <sstream>

CommandsTable::~CommandsTable()
{
	delete m_pSelectClearance;
	delete m_pSelectId;
}

void CommandsTable::initialize()
{
	prepareSelectClearance();
	prepareSelectId();
}

void CommandsTable::prepareSelectClearance()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		SELECT CommandClearance
		FROM Commands
		WHERE CommandName == ?;


	  ======================================================*/

	queryStringBuilder
		<< "SELECT " << m_ClearanceColName << " "
		<< "FROM " << m_tableName << " "
		<< "WHERE " << m_CommandColName << " == " << "?" << ";";

	std::string queryString = queryStringBuilder.str();
	m_pSelectClearance = new statement(queryString);
}

void CommandsTable::prepareSelectId()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		SELECT Id
		FROM Commands
		WHERE CommandName == "?";


	  ======================================================*/

	queryStringBuilder
		<< "SELECT " << m_IDColName << " "
		<< "FROM " << m_tableName << " "
		<< "WHERE " << m_CommandColName << " == " << "?" << ";";

	std::string queryString = queryStringBuilder.str();
	m_pSelectId = new statement(queryString);
}

Clearance CommandsTable::SelectClearance(const std::string& command)
{
	checkIfDatabaseIsInitialized();
	m_pDatabase->NewQuery(*m_pSelectClearance);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), command);
	int clearance = 255; // TODO max clearance

	if (pCurrentQuery->next() == true)
	{
		pCurrentQuery->get(clearance, COLUMN(0));
	}

	return clearance;

}

unsigned int CommandsTable::SelectId(const std::string& command)
{
	checkIfDatabaseIsInitialized();

	m_pDatabase->NewQuery(*m_pSelectId);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), command);
	int commandId = 0;

	if (pCurrentQuery->next() == true)
	{
		pCurrentQuery->get(commandId, COLUMN(0));
	}

	return commandId;
}