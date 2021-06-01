#include "Tables.hpp"

#include<sstream>

RFIDCardTable::~RFIDCardTable()
{
	delete m_pAdd;
	delete m_pDeleteWhereOwnerId;
	delete m_pDeleteWhereCardUUID;
	delete m_pSelectWhereCardUUID;
	delete m_pExistsCardUUID;
}

void RFIDCardTable::initialize()
{
	prepareAdd();
	prepareDeleteWhereOwnerId();
	prepareDeleteWhereCardUUID();
	prepareSelectWhereCardUUID();
	prepareExistsCardUUID();
}

void RFIDCardTable::prepareAdd()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		INSERT INTO RFIDCardTable(CardUUID, CardOwnerId)
		VALUES (?, ?);

	  ======================================================*/

	queryStringBuilder
		<< "INSERT INTO " << m_tableName << "(" << m_CardUUIDColName << "," << m_OwnerColName << ")" << " "
		<< "VALUES " << "(?, ?)" << ";";

	std::string queryString = queryStringBuilder.str();
	m_pAdd = new statement(queryString);
}

void RFIDCardTable::prepareDeleteWhereOwnerId()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		DELETE
		FROM RFIDCardTable
		WHERE CardOwnerId == ?;

	  ======================================================*/

	queryStringBuilder
		<< "DELETE "
		<< "FROM " << m_tableName << " "
		<< "WHERE " << m_OwnerColName << " == " << "?" << ";";



	std::string queryString = queryStringBuilder.str();
	m_pDeleteWhereOwnerId = new statement(queryString);
}

void RFIDCardTable::prepareDeleteWhereCardUUID()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		DELETE
		FROM RFIDCardTable
		WHERE CardUUID == ?;

	  ======================================================*/

	queryStringBuilder
		<< "DELETE "
		<< "FROM " << m_tableName << " "
		<< "WHERE " << m_CardUUIDColName << " == " << "?" << ";";



	std::string queryString = queryStringBuilder.str();
	m_pDeleteWhereCardUUID = new statement(queryString);
}

void RFIDCardTable::prepareSelectWhereCardUUID()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		SELECT CardOwnerId
		FROM RFIDCardTable
		WHERE CardUUID == ?;

	  ======================================================*/

	queryStringBuilder
		<< "SELECT " << m_OwnerColName << " "
		<< "FROM " << m_tableName << " "
		<< "WHERE " << m_CardUUIDColName << " == " << "?" << ";";


	std::string queryString = queryStringBuilder.str();
	m_pSelectWhereCardUUID = new statement(queryString);
}

void RFIDCardTable::prepareExistsCardUUID()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		SELECT EXISTS
		(
			SELECT CardUUID
			FROM RFIDCardTable
			WHERE CardUUID == ?
		);

	  ======================================================*/

	queryStringBuilder
		<< "SELECT EXISTS " << "("
		<< "SELECT " << m_CardUUIDColName << " "
		<< "FROM " << m_tableName << " "
		<< "WHERE " << m_CardUUIDColName << " == " << "?" << " "
		<< ")" << ";";


	std::string queryString = queryStringBuilder.str();
	m_pExistsCardUUID = new statement(queryString);
}


void RFIDCardTable::Add(const std::string& cardUUID, UID ownerId)
{
	checkIfDatabaseIsInitialized();
	m_pDatabase->NewQuery(*m_pAdd);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), cardUUID);
	if (ownerId != 0)
	{
		pCurrentQuery->bind(ARGUMENT(1), (int)ownerId);
	}

	pCurrentQuery->next();
}

void RFIDCardTable::DeleteWhereOwnerId(UID ownerId)
{
	checkIfDatabaseIsInitialized();
	m_pDatabase->NewQuery(*m_pDeleteWhereOwnerId);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), (int)ownerId);

	pCurrentQuery->next();
}

void RFIDCardTable::DeleteWhereCardUUID(const std::string& cardUUID)
{
	checkIfDatabaseIsInitialized();
	m_pDatabase->NewQuery(*m_pDeleteWhereCardUUID);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), cardUUID);

	pCurrentQuery->next();
}

UID RFIDCardTable::SelectWhereCardUUID(const std::string& cardUUID)
{
	checkIfDatabaseIsInitialized();
	m_pDatabase->NewQuery(*m_pSelectWhereCardUUID);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), cardUUID);

	int ownerId = 0;

	if (pCurrentQuery->next() == true)
	{
		pCurrentQuery->get(ownerId, COLUMN(0));
	}

	return ownerId;
}

bool RFIDCardTable::ExistsCardUUID(const std::string& cardUUID)
{
	checkIfDatabaseIsInitialized();

	m_pDatabase->NewQuery(*m_pExistsCardUUID);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), cardUUID);

	int exists = 0;
	if (pCurrentQuery->next() == true)
	{
		pCurrentQuery->get(exists, COLUMN(0));
	};

	return (bool)exists;
}
