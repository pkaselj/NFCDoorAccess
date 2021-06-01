#include"Database.hpp"


Database::Database(const std::string& pathname, ILogger* _p_logger)
{
    p_logger = _p_logger;
    if(p_logger == nullptr)
        p_logger = NulLogger::getInstance();
    
    if(pathname == "")
    {
        *p_logger << "Database " + pathname + " cannot be empty!";
        Kernel::Fatal_Error("Database " + pathname + " cannot be empty!");
    }

    int status = sqlite3_open(pathname.c_str(), &dbHandle);
    if(status != SQLITE_OK)
    {
        *p_logger << "Error while trying to open database file " + pathname;
        Kernel::Fatal_Error("Error while trying to open database file " + pathname);
    }

    if(dbHandle == nullptr)
    {
        *p_logger << "Database handler is a null pointer! Error: " + pathname;
        Kernel::Fatal_Error("Database handler is a null pointer! Error: " + pathname);
    }

    // UNTESTED
    sqlite3_extended_result_codes(dbHandle, true);

    *p_logger << "Database " + pathname + " succesfully opened!"; 

}

Database::~Database()
{

    deletePreviousQuery(); // Will finalize only owned queries TODO

    int status = sqlite3_close(dbHandle);
    if(status != SQLITE_OK)
    {
        *p_logger << "Cannot close the database! SQLITE_ status: " + std::to_string(status);
        Kernel::Fatal_Error("Cannot close the database! SQLITE_ status: " + std::to_string(status));
    }

    *p_logger << "Database closed";

}

void Database::Execute(const std::string& query_string, callback query_callback)
{
    char *error_message = nullptr;

    int status = sqlite3_exec(dbHandle, query_string.c_str(), query_callback, nullptr, &error_message);

    *p_logger << "Executed \"" + query_string + "\" query";

    if(error_message != nullptr)
    {
        *p_logger << "Error message: " + std::string(error_message);
        Kernel::Fatal_Error("Error message: " + std::string(error_message));
        sqlite3_free(error_message); // ---
    }

    if(status != SQLITE_OK)
    {
        *p_logger << "Error in executing query!";
        Kernel::Fatal_Error("Error in executing query!");
    }
}

void Database::NewQuery(statement& newQueryStatement)
{
    statement* p_newQueryStatement = &newQueryStatement;
    if(p_newQueryStatement == nullptr)
    {
        *p_logger << "New query cannot be null";
        Kernel::Fatal_Error("New query cannot be null");
    }

    deletePreviousQuery();
    p_currentQueryStatement = p_newQueryStatement;
    p_currentQueryStatement->setLogger(p_logger);

    p_currentQueryStatement->prepare(dbHandle);
}

void Database::NewQuery(const std::string& query_string)
{
    deletePreviousQuery();
    p_currentQueryStatement = new statement (dbHandle, query_string, p_logger);
    queryOwnership = true;
}

void Database::deletePreviousQuery()
{
    *p_logger << "Current query statement pointer address: " + std::to_string((int)p_currentQueryStatement);
    
    if(queryOwnership == true && p_currentQueryStatement != nullptr)
    {
        p_currentQueryStatement->finalize();
        delete p_currentQueryStatement;
        p_currentQueryStatement = nullptr; // NOTEST
        *p_logger << "Previous query object deleted!";
    }
    queryOwnership = false;
}

statement* Database::CurrentQuery() const
{
    return p_currentQueryStatement;
}