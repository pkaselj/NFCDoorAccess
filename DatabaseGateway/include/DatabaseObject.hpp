#ifndef DATABASE_OBJECT_HPP
#define DATABASE_OBJECT_HPP

#include"Tables.hpp"

#include"Logger.hpp"
#include"DataMailbox.hpp"
#include"Time.hpp"

class DatabaseObject;

/// Contains all the resources used by DatabaseRequests to execute requests
struct DatabaseResources
{
	/// Database object which wraps database requests
	DatabaseObject* m_pDatabaseObject;
	/// Outgoing Mailbox pointer
	DataMailbox* m_pMailbox;
	/// Pointer to SimplifiedMailbox used to send Request log data to logging thread
	SimplifiedMailbox* m_pLogMailbox;
	/// MailboxReference to logging thread mailbox
	MailboxReference& m_refLogThread;
};

/// Class which bundles SQL statments (SQL statement wrapper methods specifically) into coherent methods
class DatabaseObject
{
public:
	/**
	 * @brief Construct database object
	 * @param DB_path Path to dabase file
	 * @param resources DatabaseResources object
	 * @param pLogger ILogger* derived class object used to write log messages to files
	*/
	DatabaseObject(const std::string& DB_path, DatabaseResources& resources, ILogger* pLogger = NulLogger::getInstance());
	~DatabaseObject(); // TODO DELETE ALL

	/**
	 * @brief Returns required clearance to execute given command
	 * @param command CommandMessage::enuCommand type
	 * @return clearance required to execute given command. Default MAX_CLEARANCE.
	*/
	Clearance getRequiredClearanceForCommand(CommandMessage::enuCommand command);

	/**
	 * @brief Get clearance associated with given employee
	 * @param authorizationParameter InputParameter which contains authorization type (Card, PIN, ...) and authorization data
	 * @return Returns clearance associated with given employee. Default NO_CLEARANCE.
	*/
	Clearance getClearance(const InputParameter& authorizationParameter);

	/**
	 * @brief Adds authorization identifier `parameterToAdd` (Card, PIN, ...) as a guest employee
	 * @param parameterToAdd InputParameter which contains authorization type (Card, PIN, ...) and authorization data to be added
	 * @param clearance [optional] clearance associated with new identifier (Default 0)
	 * @param ownerId [optional] ownerId with which new identifier will be associated (0 for new guest employee)
	 * @return true on success, fale otherwise
	*/
	bool AddIdentifier(const InputParameter& parameterToAdd, Clearance clearance = 0, UID ownerId = 0);

	/**
	 * @brief Removes authorization identifier `parameterToRemove` from DB
	 * @param parameterToRemove InputParameter which contains authorization type (Card, PIN, ...) and authorization data to be removed
	 * @return true on success, fale otherwise
	*/
	bool RemoveIdentifier(const InputParameter& parameterToRemove);

	/**
	 * @brief Changes clearance of employee identified by `targetCredentials` to `newClearance`
	 * @param targetCredentials InputParameter which contains authorization type (Card, PIN, ...) and authorization data of an employee
	 * @param newClearance New clearance between (including) -1 and MAX_CLEARANCE
	 * @return true on success, fale otherwise
	*/
	bool SetClearance(const InputParameter& targetCredentials, Clearance newClearance);

	/**
	 * @brief Creates Log which includes: timestamp, executed `command` by employee identified with `userCredentials`
	 * 
	 * It creates the log object and passes it to the logging thread (connectionless), \n
	 * then the logging thread takes the log object, whenever it does, and inserts it into database log table.
	 * 
	 * @param command requested CommandMessage::enuCommand to be executed
	 * @param userCredentials InputParameter which contains authorization type (Card, PIN, ...) and authorization data of an employee who requested to execute `command`
	*/
	void CreateLog(CommandMessage::enuCommand command, const InputParameter& userCredentials);

	/// Creates database log table entry based on `logEntry` object. Used by logging thread to write logs to database log table
	void WriteLogToLogTable(LogEntry& logEntry);

	/**
	 * @brief Returns user ID associated with `param`
	 * @param param InputParameter which contains authorization type (Card, PIN, ...) and authorization data of an employee
	 * @return Returns user ID associated with `param`. Default 0 (meaning no employee exists with given `param`)
	*/
	unsigned int getUserId(const InputParameter& param);

	/// Get the reference to the locking object used to synchronize the database access
	std::mutex& getWriteLock() { return m_writeLock; }

private:

	//************* PRIVATE DATA

	std::string m_path;
	ILogger* m_pLogger;
	DatabaseResources& m_resources;

	std::mutex m_writeLock;

	Database m_database;

	bool m_initialized = false;

	//************* DATABASE TABLES

	EmployeesTable* m_pEmployeesTable = nullptr;
	KeypadPassTable* m_pKeypadPassTable = nullptr;
	CommandsTable* m_pCommandsTable = nullptr;
	RFIDCardTable* m_pRFIDCardTable = nullptr;
	WebAPITable* m_pWebAPITable = nullptr;
	LogTable* m_pLogTable = nullptr;

	//************* INIT METHODS
	
	void initialize();

	void initializeTables();
	void prepareStatements();
	
	void checkIfDatabaseIsInitialized();
	bool test();

	//************* PRIVATE HELPER FUNCTIONS

	// Used to implement associated public functions

	std::string parseCommand(CommandMessage::enuCommand command);

	LogEntry parseInputParameterToLogEntry(const InputParameter& param);

	Clearance getClearanceFromPassword(const std::string& password);
	Clearance getClearanceFromName(const std::string& name);
	Clearance getClearanceFromRFIDCard(const std::string& uuid);

	unsigned int getUserIdFromPassword(const std::string& password);
	unsigned int getUserIdFromRFIDCard(const std::string& uuid);

	bool AddCard(const std::string& uuid, Clearance clearance = 0, UID ownerId = 0);
	bool AddPassword(const std::string& password, Clearance clearance = 0, UID ownerId = 0);

	bool RemoveCard(const std::string& uuid);
	bool RemovePassword(const std::string& password);

	UID createNewUniqueGuest(Clearance clearance = 0);

	bool SetClearanceForCard(const std::string& cardUUID, Clearance newClearance);
	bool SetClearanceForPassword(const std::string& password, Clearance newClearance);

};


#endif