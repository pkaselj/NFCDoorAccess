#include "Tables.hpp"
#include <sstream>

WebAPITable::~WebAPITable()
{
	delete m_pAdd;
	delete m_pSelectUserId;
	delete m_pUpdateWebPass;
	delete m_pDeleteWhereUserId;
	delete m_pExistsPassword;
}

void WebAPITable::initialize()
{
	prepareAdd();
	prepareSelectUserId();
	prepareUpdateWebPass();
	prepareDeleteWhereUserId();
	prepareExistsPassword();
}

void WebAPITable::prepareAdd()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		INSERT INTO WebPassTable(UserId, WebPassHash)
		VALUES (?, ?);


	  ======================================================*/

	queryStringBuilder
		<< "INSERT INTO " << m_tableName << "(" << m_UserIdColName << "," << m_WebPassColName << ")" << " "
		<< "VALUES " << "(?, ?)" << ";";


	std::string queryString = queryStringBuilder.str();
	m_pAdd = new statement(queryString);
}

void WebAPITable::prepareSelectUserId()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		SELECT UserId
		FROM WebPassTable
		WHERE WebPassHash == ?;

	  ======================================================*/

	queryStringBuilder
		<< "SELECT " << m_UserIdColName << " "
		<< "FROM " << m_tableName << " "
		<< "WHERE " << m_WebPassColName << " == " << "?" << ";";



	std::string queryString = queryStringBuilder.str();
	m_pSelectUserId = new statement(queryString);
}

void WebAPITable::prepareUpdateWebPass()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		UPDATE WebPassTable
		SET WebPassHash = ?
		WHERE UserId == ?;

	  ======================================================*/

	queryStringBuilder
		<< "UPDATE " << m_tableName << " "
		<< "SET " << m_WebPassColName << " = " << "?" << " "
		<< "WHERE " << m_UserIdColName << " == " << "?" << ";";


	std::string queryString = queryStringBuilder.str();
	m_pUpdateWebPass = new statement(queryString);
}

void WebAPITable::prepareDeleteWhereUserId()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		DELETE FROM WebPassTable
		WHERE UserId == ?;

	  ======================================================*/

	queryStringBuilder
		<< "DELETE FROM " << m_tableName << " "
		<< "WHERE " << m_UserIdColName << " == " << "?" << ";";


	std::string queryString = queryStringBuilder.str();
	m_pDeleteWhereUserId = new statement(queryString);
}

void WebAPITable::prepareExistsPassword()
{
	std::stringstream queryStringBuilder;
	queryStringBuilder.str() = "";
	/*======================================================

		SELECT EXISTS
		(
			SELECT WebPassHash
			FROM WebPassTable
			WHERE WebPassHash == ?
		);

	  ======================================================*/

	queryStringBuilder
		<< "SELECT EXISTS " << "("
		<< "SELECT " << m_WebPassColName << " "
		<< "FROM " << m_tableName << " "
		<< "WHERE " << m_WebPassColName << " == " << "?" << " "
		<< ")" << ";";


	std::string queryString = queryStringBuilder.str();
	m_pExistsPassword = new statement(queryString);
}

void WebAPITable::Add(UID userId, const std::string& password)
{
	checkIfDatabaseIsInitialized();
	m_pDatabase->NewQuery(*m_pAdd);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), (int)userId);
	pCurrentQuery->bind(ARGUMENT(1), password);

	pCurrentQuery->next();

}

UID WebAPITable::SelectUserId(const std::string& password)
{
	checkIfDatabaseIsInitialized();
	m_pDatabase->NewQuery(*m_pSelectUserId);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), password);
	int userId = 0;

	if (pCurrentQuery->next() == true)
	{
		pCurrentQuery->get(userId, COLUMN(0));
	}

	return userId;

}

void WebAPITable::UpdateWebPass(UID userId, const std::string& newPassword)
{
	checkIfDatabaseIsInitialized();
	m_pDatabase->NewQuery(*m_pUpdateWebPass);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), newPassword);
	pCurrentQuery->bind(ARGUMENT(1), (int)userId);

	pCurrentQuery->next();

}

void WebAPITable::DeleteWhereUserId(UID userId)
{
	checkIfDatabaseIsInitialized();
	m_pDatabase->NewQuery(*m_pDeleteWhereUserId);
	statement* pCurrentQuery = m_pDatabase->CurrentQuery();

	pCurrentQuery->clearBindingsAndReset();

	pCurrentQuery->bind(ARGUMENT(0), (int)userId);

	pCurrentQuery->next();

}

bool WebAPITable::ExistsPassword(const std::string& password)
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
