#ifndef DATABASE_REQUEST_HPP
#define DATABASE_REQUEST_HPP

#include "DatabaseObject.hpp"

class IDatabaseRequest;

/// Class used to create IDatabaseRequest-family objects
class DatabaseRequestFactory
{
public:
	DatabaseRequestFactory(DatabaseResources& resources, ILogger* pLogger = NulLogger::getInstance());
	~DatabaseRequestFactory() {}

	// TODO CommandMessage** to CommandMessage*, but check if possible

	/**
	 * @brief Parses CommandMessage to IDatabaseRequest* object
	 * @param ppRequestMessage Pointer to pointer of CommandMessage.
	 * (Most command messages are dynamically allocated - first star)
	 * (It takes the ownership of `ppRequestMessage` - second star)
	 * @return IDatabaseRequest* object based on `ppRequestMessage`
	*/
	IDatabaseRequest* createRequestObjectFrom(CommandMessage** ppRequestMessage);

private:
	DatabaseResources& m_resources;

	ILogger* m_pLogger;

};

/// Virual base class used as a base for all other DatabaseRequest classes
class IDatabaseRequest
{
public:
	// Takes ownership of *ppMessage (sets ppMessage to nullptr)
	IDatabaseRequest(CommandMessage** ppRequestMessage, DatabaseResources& resources, ILogger* pLogger = NulLogger::getInstance());

	virtual ~IDatabaseRequest();

	void Process();

	Clearance getRequiredClearance() const { return m_requiredClearance; }

protected:

	CommandMessage* m_pRequest;

	DatabaseResources& m_resources;

	Clearance m_requiredClearance;

	ILogger* m_pLogger;

	/// Sends DatabaseReply with `errorStatus` to request source
	void ReplyToRequestSource(DatabaseReply::enuStatus errorStatus);


	//***** FUNCTIONS WHICH MUST BE OVERLOADED IN CHILD CLASSES ******

	/// Validate request parameters. Returns true if all parameters are valid.
	virtual bool Validate() = 0;
	/// Check if requesting employee is authorized to execute request/command. Returns true if employee is authorized to execute request/command.
	virtual bool Authorize() = 0;
	/// Executes the request/command
	virtual void Execute() = 0;
	/// Logs the action. (Execution of request)
	virtual void Log() = 0;

};

class AuthorizeRequest : public IDatabaseRequest
{
public:
	AuthorizeRequest(CommandMessage** ppRequestMessage, DatabaseResources& resources, ILogger* pLogger = NulLogger::getInstance())
		: IDatabaseRequest(ppRequestMessage, resources, pLogger)
	{}

	virtual ~AuthorizeRequest() {}

private:

	void ReplyWithRequestedClearance(Clearance clearance);

	virtual bool Validate() override;
	virtual bool Authorize() override;
	virtual void Execute() override;
	virtual void Log() override;

};

class GuestAccessControl : public IDatabaseRequest
{
public:
	GuestAccessControl(CommandMessage** ppRequestMessage, DatabaseResources& resources, ILogger* pLogger = NulLogger::getInstance())
		: IDatabaseRequest(ppRequestMessage, resources, pLogger)
	{}

	virtual ~GuestAccessControl() {}

private:

	virtual bool Validate() override;
	virtual bool Authorize() override;
	virtual void Execute() override;
	virtual void Log() override;

};


class AddRequest : public IDatabaseRequest
{
public:
	AddRequest(CommandMessage** ppRequestMessage, DatabaseResources& resources, ILogger* pLogger = NulLogger::getInstance())
		: IDatabaseRequest(ppRequestMessage, resources, pLogger)
	{}

	virtual ~AddRequest() {}

private:

	virtual bool Validate() override;
	virtual bool Authorize() override;
	virtual void Execute() override;
	virtual void Log() override;

};

class RemoveRequest : public IDatabaseRequest
{
public:
	RemoveRequest(CommandMessage** ppRequestMessage, DatabaseResources& resources, ILogger* pLogger = NulLogger::getInstance())
		: IDatabaseRequest(ppRequestMessage, resources, pLogger)
	{}

	virtual ~RemoveRequest() {}

private:

	virtual bool Validate() override;
	virtual bool Authorize() override;
	virtual void Execute() override;
	virtual void Log() override;

};

class SetClearanceRequest : public IDatabaseRequest
{
public:
	SetClearanceRequest(CommandMessage** ppRequestMessage, DatabaseResources& resources, ILogger* pLogger = NulLogger::getInstance())
		: IDatabaseRequest(ppRequestMessage, resources, pLogger)
	{}

	virtual ~SetClearanceRequest() {}

private:

	virtual bool Validate() override;
	virtual bool Authorize() override;
	virtual void Execute() override;
	virtual void Log() override;

};



#endif