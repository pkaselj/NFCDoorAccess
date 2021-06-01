#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include<string>
#include<sqlite3.h>


#include"Kernel.hpp"
#include"NulLogger.hpp"


typedef struct statement statement;
/// wrapper struct for sqlite3_stmt object
struct statement
{
    private:
    /// Pointer to an SQLite3 statement object
    sqlite3_stmt* statement_object = nullptr;
    
    /// Pointer to an SQLite3 database handle object
    sqlite3* dbHandle = nullptr;

    /// Query string to be compiled into statement
    std::string queryString = "";

    /// Pointer to a Logger
    ILogger* p_logger = nullptr;

    /// `true` if the statement is finalized (no operations allowed after), else `false`
    bool finalized = false;
    /// `true` if the statement is prepared (no operations allowed before), else `false`
    bool prepared = false;

    void createNulLogger();

    public:
                        /**
                         * @brief Construct a new statement object without preparing it.
                         * 
                         * `statement::prepare(...)` must be used before executing the statement.
                         * 
                         * @param query_string Query string to be compiled into statement
                         * @param _p_logger Pointer to a logger object
                         */
                        statement(const std::string& query_string, ILogger* _p_logger = NulLogger::getInstance());

    /**
     * @brief Construct a new statement object and prepare it.
     * 
     * @param _dbHandle Pointer to an SQLite3 database handle object
     * @param query_string Query string to be compiled into statement
     * @param _p_logger Pointer to a logger object
     */
    statement(sqlite3* _dbHandle, const std::string& query_string, ILogger* _p_logger = nullptr);

    /**
     * @brief Destroy the statement object
     * 
     */
    ~statement();

    /**
     * @brief Prepares the statement object for execution
     * 
     * @param _dbHandle Pointer to an SQLite3 database handle object 
     */
    void prepare(sqlite3* _dbHandle);

    /// Get the next row of results. Returns `true` if there are results(rows) left.
    bool next();

    /// Resets the statement object. Clears the state and prepares it for re-execution
    void reset();

    /// Clears the argument bindings in the query. Does not reset the state.
    void clearBindings();

    /// Clears argument bindings and resets the query (prepares it for re-execution)
    void clearBindingsAndReset();

    /// Finalizes the statement. AFTER THIS NO OPERATIONS ON THE STATEMENT ARE ALLOWED !!!
    void finalize();

    
    void setLogger(ILogger* _p_logger);
    //operator sqlite3_stmt* () const;

    /// returns the pointer to the SQLite3 statement object
    sqlite3_stmt* stmt() const;

    /**
     * @brief Get the value of the `column` and store it into the `get_destination`
     * 
     * Column indexing starts at 0
     * 
     * @tparam T type of data to be fecthed
     * @param get_destination Destination container for the data
     * @param column column number: USE COLUMN() MACRO
     * 
     * Example: \n
     * @code
     * 
     * statement currentStatement("SELECT * FROM table");
     * int ID = -1;
     * 
     * currentStatement.get<int>( ID, COLUMN(0) );
     * 
     * @endcode
     */
    template<class T>
    void get(T& get_destination, int column);


    /**
     * @brief Bind `value` to the parameter with `index`
     * 
     * Argument indexing starts at 1. \n
     * ARGUMENT() Macro indexing starts at 0. \n
     * 
     * @tparam T type of data to be bound
     * @param index index of argument
     * @param value value to be bound
     * 
     * Example: \n
     * @code
     * 
     * statement currentStatement("SELECT * FROM table WHERE id = ?");
     * int ID = 5;
     * 
     * curentStatement.bind<int>( ARGUMENT(0), ID );
     * 
     * @endcode
     */
    template<class T>
    void bind(int index, const T& value);

    /**
     * @brief Bind the value of `p_value` to the parameter with `index`
     * 
     * Argument indexing starts at 1. \n
     * ARGUMENT() Macro indexing starts at 0. \n
     * 
     * @tparam T type of data to be bound
     * @param index index of argument
     * @param p_value pointer to the value to be bound
     * 
     * Example: \n
     * @code
     * 
     * statement currentStatement("SELECT * FROM table WHERE id = ?");
     * int* p_ID = 5;
     * 
     * curentStatement.bind<int>( ARGUMENT(0), p_ID );
     * 
     * @endcode
     */
    template<class T>
    void bind(int index, const T* p_value);

    /**
     * @brief Bind rvalue `value` to the parameter with `index`
     * 
     * Argument indexing starts at 1. \n
     * ARGUMENT() Macro indexing starts at 0. \n
     * 
     * @tparam T type of data to be bound
     * @param index index of argument
     * @param value value to be bound
     * 
     * Example: \n
     * @code
     * 
     * statement currentStatement("SELECT * FROM table WHERE id = ?");
     * 
     * curentStatement.bind<int>( ARGUMENT(0), ID );
     * 
     * @endcode
     */
    template<class T>
    void bind(int index, const T&& value);
};

void statement::setLogger(ILogger* _p_logger)
{
    p_logger = _p_logger;
    if(p_logger == nullptr)
    {
        p_logger = NulLogger::getInstance();
    }
}

statement::statement(sqlite3* _dbHandle, const std::string& query_string, ILogger* _p_logger)
    :   dbHandle(_dbHandle), queryString(query_string)
{
    setLogger(_p_logger);
    prepare(dbHandle);
}

void statement::prepare(sqlite3* _dbHandle)
{
    if(prepared == true)
    {
        *p_logger << "Statement \"" + queryString + "\" already prepared. Resetting and clearing the bindings!";
        clearBindingsAndReset();
        return;
    }

    if(_dbHandle == nullptr)
    {
        *p_logger << "Database Handle cannot be null!";
        Kernel::Fatal_Error("Database Handle cannot be null!");
    }

    dbHandle = _dbHandle;

    
    if(queryString == "")
    {
        *p_logger << "Query String cannot be empty!";
        Kernel::Fatal_Error("Query String cannot be empty!");
    }

    int status = sqlite3_prepare_v2(dbHandle,
                                    queryString.c_str(),
                                    -1,
                                    &statement_object,
                                    nullptr);
                                
    prepared = true;

    if(status != SQLITE_OK)
    {
        finalize();
        Kernel::Fatal_Error("Cannot prepare statement: \"" + queryString + "\"");
    }
}

statement::statement(const std::string& query_string, ILogger* _p_logger)
    :   dbHandle(nullptr), queryString(query_string)
{
    setLogger(_p_logger);
}

statement::~statement()
{
    finalize();
}

void statement::clearBindingsAndReset()
{
    clearBindings();
    reset();
}

bool statement::next()
{
    if(finalized == true)
    {
        *p_logger << "Cannot use finalized statement \"" + queryString + "\"";
        return false;
    }

    int status = sqlite3_step(statement_object);

    if(status != SQLITE_ROW && status != SQLITE_DONE)
    {
        // UNTESTED
        std::string sqlite3_internal_errmsg = sqlite3_errmsg(dbHandle);
        *p_logger << "Error while stepping through query. \"" + queryString + "\" SQLITE_errmsg: " + sqlite3_internal_errmsg;
        finalize();
        Kernel::Fatal_Error("Error while stepping through query. \"" + queryString + "\" SQLITE_status: " + std::to_string(status));
    }

    if(status == SQLITE_ROW)
    {
        *p_logger << "Row successfully fetched";
        return true;
    }


    *p_logger << "Query executed. No rows fetched.";
    return false;
}

void statement::clearBindings()
{
    if(finalized == true)
    {
        *p_logger << "Cannot use finalized statement \"" + queryString + "\"";
        return;
    }

    int status = sqlite3_clear_bindings(statement_object);
    if(status != SQLITE_OK)
    {
        *p_logger << "Error in clearing binding values from statement. \"" + queryString + "\" SQLITE_status: " + std::to_string(status);
        Kernel::Fatal_Error("Error in clearing binding values from statement. \"" + queryString + "\" SQLITE_status: " + std::to_string(status));
    }

    *p_logger << "Bindings cleared successfully";
}

void statement::reset()
{
    if(finalized == true)
    {
        *p_logger << "Cannot use finalized statement \"" + queryString + "\"";
        return;
    }

    int status = sqlite3_reset(statement_object);
    if(status != SQLITE_OK)
    {
        *p_logger << "Error in resetting statement. \"" + queryString + "\" SQLITE_ status: " + std::to_string(status);
        Kernel::Fatal_Error("Error in resetting statement. \"" + queryString + "\" SQLITE_ status: " + std::to_string(status));
    }

    *p_logger << "Statement \"" + queryString + "\" successfully reset.";
}

void statement::finalize()
{

    *p_logger << "Finalizing query statement \"" + queryString + "\"";

    if(finalized == false)
    {
        sqlite3_finalize(statement_object);
        *p_logger << "Statement finalized!";
        finalized = true;
    }
}

/*statement::operator sqlite3_stmt* () const
{
    return statement_object;
}*/

sqlite3_stmt* statement::stmt() const
{
    return statement_object;
}

template<class T>
void statement::get(T& get_destination, int column)
{
    //int columnCount = ???
    const void* pResult = sqlite3_column_blob(statement_object, column);
    if (pResult == nullptr)
    {
        get_destination = T(); // TODO ? MUST BE DEFAULT CONSTRUCTIBLE
        return;
    }
    get_destination = *( reinterpret_cast<const T*>(pResult) );
}

template<>
void statement::get(std::string& get_destination, int column)
{
    //int columnCount = ???
    const char* szResult = reinterpret_cast<const char*>(sqlite3_column_text(statement_object, column));
    if (szResult == nullptr)
    {
        get_destination = "";
        return;
    }
    get_destination = szResult;
}

template<>
void statement::get(int& get_destination, int column)
{
    //int columnCount = ???
    get_destination = sqlite3_column_int(statement_object, column);
}

// TODO !?
template<>
void statement::get(unsigned int& get_destination, int column)
{
    //int columnCount = ???
    get_destination = (unsigned int)sqlite3_column_int(statement_object, column);
}

template<>
void statement::get(double& get_destination, int column)
{
    //int columnCount = ???
    get_destination = sqlite3_column_double(statement_object, column);
}


template<class T>
void statement::bind(int index, const T& value)
{
    int status = sqlite3_bind_blob(statement_object, index, (void *) &value, sizeof(value), nullptr); // TODO sizeof value ????
    if(status != SQLITE_OK)
    {
        *p_logger << "Error in binding value to statement. \"" + queryString + "\" SQLITE_ status: " + std::to_string(status);
        Kernel::Fatal_Error("Error in binding value to statement. \"" + queryString + "\" SQLITE_ status: " + std::to_string(status));
    }
}

template<>
void statement::bind(int index, const int& value)
{
    int status = sqlite3_bind_int(statement_object, index, value);
    *p_logger << "Binding " + std::to_string(value) + " to index " + std::to_string(index) + " of \"" + queryString + "\""; 
    if(status != SQLITE_OK)
    {
        *p_logger << "Error in binding value to statement. \"" + queryString + "\" SQLITE_status: " + std::to_string(status);
        Kernel::Fatal_Error("Error in binding value to statement. \"" + queryString + "\" SQLITE_status: " + std::to_string(status));
    }
}

template<>
void statement::bind(int index, const std::string& value)
{
    int status = sqlite3_bind_text(statement_object, index, value.c_str(), -1, nullptr);
    *p_logger << "Binding " + value + " to index " + std::to_string(index) + " of \"" + queryString + "\""; 
    if(status != SQLITE_OK)
    {
        *p_logger << "Error in binding value to statement. \"" + queryString + "\" SQLITE_ status: " + std::to_string(status);
        Kernel::Fatal_Error("Error in binding value to statement. \"" + queryString + "\" SQLITE_ status: " + std::to_string(status));
    }
}

template<>
void statement::bind(int index, const double& value)
{
    int status = sqlite3_bind_double(statement_object, index, value);
    *p_logger << "Binding " + std::to_string(value) + " to index " + std::to_string(index) + " of \"" + queryString + "\""; 
    if(status != SQLITE_OK)
    {
        *p_logger << "Error in binding value to statement. \"" + queryString + "\" SQLITE_ status: " + std::to_string(status);
        Kernel::Fatal_Error("Error in binding value to statement. \"" + queryString + "\" SQLITE_ status: " + std::to_string(status));
    }
}


template<>
void statement::bind(int index, const char* p_value)
{
    int status = sqlite3_bind_text(statement_object, index, p_value, -1, nullptr);
    *p_logger << "Binding " + std::string(p_value) + " to index " + std::to_string(index) + " of \"" + queryString + "\""; 
    if(status != SQLITE_OK)
    {
        *p_logger << "Error in binding value to statement. \"" + queryString + "\" SQLITE_ status: " + std::to_string(status);
        Kernel::Fatal_Error("Error in binding value to statement. \"" + queryString + "\" SQLITE_ status: " + std::to_string(status));
    }
}

template<class T>
void statement::bind(int index, const T&& value)
{
    int status = sqlite3_bind_blob(statement_object, index, (void *) &value, sizeof(value), nullptr); // TODO sizeof value ????
    if(status != SQLITE_OK)
    {
        *p_logger << "Error in binding value to statement. \"" + queryString + "\" SQLITE_ status: " + std::to_string(status);
        Kernel::Fatal_Error("Error in binding value to statement. \"" + queryString + "\" SQLITE_ status: " + std::to_string(status));
    }
}

template<>
void statement::bind(int index, const int&& value)
{
    int status = sqlite3_bind_int(statement_object, index, value);
    *p_logger << "Binding " + std::to_string(value) + " to index " + std::to_string(index) + " of \"" + queryString + "\""; 
    if(status != SQLITE_OK)
    {
        *p_logger << "Error in binding value to statement. \"" + queryString + "\" SQLITE_status: " + std::to_string(status);
        Kernel::Fatal_Error("Error in binding value to statement. \"" + queryString + "\" SQLITE_status: " + std::to_string(status));
    }
}

template<>
void statement::bind(int index, const double&& value)
{
    int status = sqlite3_bind_double(statement_object, index, value);
    *p_logger << "Binding " + std::to_string(value) + " to index " + std::to_string(index) + " of \"" + queryString + "\""; 
    if(status != SQLITE_OK)
    {
        *p_logger << "Error in binding value to statement. \"" + queryString + "\" SQLITE_ status: " + std::to_string(status);
        Kernel::Fatal_Error("Error in binding value to statement. \"" + queryString + "\" SQLITE_ status: " + std::to_string(status));
    }
}

template<>
void statement::bind(int index, const std::string&& value)
{
    int status = sqlite3_bind_text(statement_object, index, value.c_str(), -1, nullptr);
    *p_logger << "Binding " + value + " to index " + std::to_string(index) + " of \"" + queryString + "\""; 
    if(status != SQLITE_OK)
    {
        *p_logger << "Error in binding value to statement. \"" + queryString + "\" SQLITE_ status: " + std::to_string(status);
        Kernel::Fatal_Error("Error in binding value to statement. \"" + queryString + "\" SQLITE_ status: " + std::to_string(status));
    }
}

#endif