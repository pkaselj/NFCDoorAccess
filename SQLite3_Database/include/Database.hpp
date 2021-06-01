#ifndef DATABASE_HPP
#define DATABASE_HPP

#include<memory>

#include"ILogger.hpp"

#include"Statement.hpp"

/// callback function to be called on every row of results when using `Database::Execute(...)`. \see SQLITE3 C/C++ INTERFACE - OFFICIAL DOCUMENTATION
typedef int (*callback)(void*, int, char **, char **);

/// Macro for readability
#define COLUMN(X) (X)
/// Makes arguments start at index 0
#define ARGUMENT(X) (X) + 1 

/// SQLite3 Database wrapper class
class Database
{
    private:
    /// SQLite3 database handle pointer
    sqlite3* dbHandle = nullptr;

    /// ILogger pointer
    ILogger* p_logger = nullptr;

    /// Pointer to the current statement (query) to be executed
    statement* p_currentQueryStatement = nullptr;

    /// If true then the current query was dynamically allocated by this object and needs to be freed before changing the query / statement.
    bool queryOwnership = false;

    /// Finalizes and deletes previous query
    void deletePreviousQuery();

    public:
    /// Constructor without database name doesn't have sense.
    Database() = delete;

    /**
     * @brief Construct a new Database object.
     * 
     * @param database_name Name of the database file
     * @param _p_logger Pointer to a logger object
     */
    Database(const std::string& database_name, ILogger* _p_logger = NulLogger::getInstance());

    /**
     * @brief Executes a query. Calls `query_callback` function for each row in the results.
     * 
     * @param query_string SQLite query
     * @param query_callback callback function to call on each row `typedef int (*callback)(void*, int, char **, char **)`
     */
    void Execute(const std::string& query_string, callback query_callback);

    /**
     * @brief Points `p_currentQueryStatement` to `newQueryStatement` which MUST NOT BE DESTROYED DURING OPERATION.
     * 
     * @param newQueryStatement `statement` object
     */
    void NewQuery(statement& newQueryStatement);

    /**
     * @brief Create new statement object from `newQueryStatement` and points `p_currentQueryStatement` to it.
     * 
     * @param query_string SQLite query
     */
    void NewQuery(const std::string& query_string);

    /**
     * @brief Returns `p_currentQueryStatement`
     * 
     * @return statement* 
     */
    statement* CurrentQuery() const;

    /**
     * @brief Destroy the Database object
     * 
     */
    ~Database();
};

#endif