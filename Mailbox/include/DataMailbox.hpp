/*****************************************************************//**
 * \file   DataMailbox.hpp
 * \brief  .
 * 
 * \author KASO
 * \date   February 2021
 *********************************************************************/

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

#ifndef DATA_MAILBOX_HPP
#define DATA_MAILBOX_HPP

#include "SimplifiedMailbox.hpp"
#include "WatchdogSettings.hpp"

#include <string>
#include <limits>

 // TODO export

using Clearance = signed char;
using byte = unsigned char;
using CardUUID = std::string;
using KeyPass = std::string;
using PIN = std::string;

const Clearance MAX_CLEARANCE = std::numeric_limits<Clearance>::max();
const Clearance FAIL_SAFE_CLEARANCE = std::numeric_limits<Clearance>::min();
const Clearance NO_CLEARANCE = -1;

// ---------------------------

class DataMailboxMessage;
class BasicDataMailboxMessage;

class MessageDataType
{
public:

	/// Defines all the types of messages that can be sent and received with DataMailbox. Always the first byte of the raw (serialized) message.
	enum class enuType
	{
		NONE = 0,
		DataMailboxErrorMessage,
		CommandMessage,
		WatchdogMessage,
		DatabaseReply,
		COUNT // get number of message types
	};

	MessageDataType() = delete;
	MessageDataType(enuType messageDataType);

	~MessageDataType() = default;



	OWNER DataMailboxMessage* createPointerToEmptyMessage();

	enuType get() const { return m_messageDataType; }
	char toChar() const { return (char)m_messageDataType; }

	/// Returns string name of the MessageDataType represented by `dataType` code. \see MessageDataType
	std::string toString() const;

	friend bool operator== (const MessageDataType& object, enuType type)
	{
		return object.m_messageDataType == type;
	}

	friend bool operator!= (const MessageDataType& object, enuType type)
	{
		return object.m_messageDataType != type;
	}

	void Decode(char dataTypeId);


private:

	enuType m_messageDataType = enuType::NONE;

};




/**
 * @brief Apstract base class for all the message classes that can be sent and received with DataMailbox.
*/
class DataMailboxMessage
{
public:
	DataMailboxMessage();

	/// Used by constructors of inherited classes
	DataMailboxMessage(MessageDataType dataType);

	DataMailboxMessage(const DataMailboxMessage&) = delete;
	DataMailboxMessage& operator=(const DataMailboxMessage&) = delete;

	DataMailboxMessage(DataMailboxMessage&& other);
	DataMailboxMessage& operator=(DataMailboxMessage&& other);


	/// Deletes (frees) serialized data when called
	virtual ~DataMailboxMessage();

	/**
	 * @brief Function which serializes object data into compact binary form and stores it to m_serialized.
	 *
	 * How to overload: \n
	 * Before data can be serialized, `deleteAndReallocateSerializedData(size_t size)` must be called. \n
	 * \see `deleteAndReallocateSerializedData(size_t size)` \n
	 * \n
	 * First byte of the serialized data must be `m_dataType` which determines the type (class) of the message object. \n
	 * m_dataType is used to determine the proper course of action during deserialization and processing. \n
	 * \n
	 * Example serialization function for `class CommandMessage`: \n
	 *
	 *		class CommandMessage
	 *		{
	 *			//(...)
	 *				typedef enum : char
	 *				{
	 *					NONE = 0,
	 *					ADD_USER,
	 *					REMOVE_USER
	 *				} enuCommand;
	 *
	 *				virtual void Serialize();
	 *
	 *				enuCommand m_command;
	 *				std::string m_parameters;
	 *
	 *				// (...) and other inherited members like `m_serialized` and `deleteAndReallocateSerializedData(size_t size)`
	 *		};
	 *
	 *
	 *		void CommandMessage::Serialize()
	 *		{
	 *
	 *			size_t sizeOfSerializedData = sizeof(MessageDataType) + sizeof(enuCommand) + m_parameters.length();
	 *
	 *			int commandIdOffset = sizeof(MessageDataType);
	 *			int parametersOffset = sizeof(enuCommand) + commandIdOffset;
	 *
	 *			int parametersLength = m_parameters.length();
	 *
	 *			deleteAndReallocateSerializedData(sizeOfSerializedData);
	 *
	 *			memcpy(reinterpret_cast<void*>(m_serialized), reinterpret_cast<const void*>(&m_dataType), sizeof(MessageDataType));
	 *			memcpy(reinterpret_cast<void*>(m_serialized + commandIdOffset), reinterpret_cast<const void*>(&m_command), sizeof(enuCommand));
	 *			memcpy(reinterpret_cast<void*>(m_serialized + parametersOffset), reinterpret_cast<const void*>(m_parameters.c_str()), parametersLength);
	 *
	 *		}
	 *
	 *
	 *
	*/
	virtual void Serialize() = 0;

	/**
	 * @brief Function which deserializes object data from compact binary form from m_serialized and initialized object fields.
	 *
	 * How to overload: \n
	 * Before data can be deserialized, `checkSerializedData()` must be called. \n
	 * \see `checkSerializedData()` \n
	 * \n
	 * First byte of the deserialized data is the `m_dataType` which determines the type (class) of the message object. \n
	 * m_dataType is used to determine the proper course of action during deserialization and processing. \n
	 * \n
	 * m_sizeOfSerializedData can be used as it is automatically calculated at the reception by DataMailbox. \n
	 * \n
	 * Example deserialization function for `class CommandMessage`: \n
	 *
	 *		class CommandMessage
	 *		{
	 *			//(...)
	 *				typedef enum : char
	 *				{
	 *					NONE = 0,
	 *					ADD_USER,
	 *					REMOVE_USER
	 *				} enuCommand;
	 *
	 *				virtual void Deserialize();
	 *
	 *				enuCommand m_command;
	 *				std::string m_parameters;
	 *
	 *				// (...) and other inherited members like `m_serialized` and `deleteAndReallocateSerializedData(size_t size)`
	 *		};
	 *
	 *
	 *		void CommandMessage::Deserialize()
	 *		{
	 *			checkSerializedData();
	 *
	 *			int commandIdOffset = sizeof(MessageDataType);
	 *			int parametersOffset = sizeof(enuCommand) + commandIdOffset;
	 *
	 *			int parametersLength = m_sizeOfSerializedData - parametersOffset;
	 *
	 *			memcpy(reinterpret_cast<void*>(&m_command), reinterpret_cast<const void*>(m_serialized + commandIdOffset), sizeof(enuCommand));
	 *
	 *			m_parameters = std::string(m_serialized + parametersOffset, parametersLength);
	 *		}
	 *
	 *
	 *
	*/

	virtual void Deserialize() = 0;

	/// Dumps raw serialized data into `filepath.dump` file which can be read by any hex editor. Serializes data if necessary
	void DumpSerialData(const std::string filepath);

	/// Returns the MessageDataType which identifies the message class/object. Used when deserializing and processing data.
	MessageDataType getDataType() { return m_dataType; };

	/// Decodes MessageDataType from the first byte of the serialized raw binary data
	void decodeMessageDataType();

	/// Function which returns string with log info of the current message object.
	virtual std::string getInfo() = 0;

	/// Set serialized data as `rawData` and size as `dataSize`. Used internally. TODO private?
	void setSerializedData(char* rawData, size_t dataSize) { m_serialized = rawData; m_sizeOfSerializedData = dataSize; }

	/// Returns MailboxReference of source of this message (object)
	MailboxReference& getSource() { return m_source; };

	// DEBUG
	char* getSerializedData() const { return m_serialized; }
	size_t getSerializedDataSize() const { return m_sizeOfSerializedData; }

protected:
	MessageDataType m_dataType;

	char* m_serialized;
	size_t m_sizeOfSerializedData;

	MailboxReference m_source;

	/// Checks validity of serialized data. Exits on failure.
	void checkSerializedData();

	/// Clears and frees current serialized data and allocates memory for new serialized data. Also sets the `m_sizeOfSerializedData` to `size`
	void deleteAndReallocateSerializedData(size_t size);

	/// Clears and frees current serialized data
	void deleteSerializedData();

	friend class DataMailbox;
};

/// Abstract virtual class used as a parent class for all other user-defined message classes.
class ExtendedDataMailboxMessage : public DataMailboxMessage
{
public:
	ExtendedDataMailboxMessage() = default;
	ExtendedDataMailboxMessage(MessageDataType dataType);
	virtual ~ExtendedDataMailboxMessage() = default;

	ExtendedDataMailboxMessage(const ExtendedDataMailboxMessage&) = delete;
	ExtendedDataMailboxMessage& operator=(const ExtendedDataMailboxMessage&) = delete;

	ExtendedDataMailboxMessage(ExtendedDataMailboxMessage&& other) : DataMailboxMessage(std::move(other)) {}
	ExtendedDataMailboxMessage& operator= (ExtendedDataMailboxMessage&& other);

	/**
	 * Takes the `BasicDataMailboxMessage message` which represents serialized message received by DataMailbox, \n
	 * creates new Message object (specific message decoded from `MessageDataType m_dataType` field of `message`) \n
	 * takes ownership of the serialized data and deserialized it.
	 *
	 * @param message Represents serialized message received by DataMailbox
	*/
	void Unpack(BasicDataMailboxMessage& message);

	virtual void Deserialize() = 0;
	virtual void Serialize() = 0;
	virtual std::string getInfo() = 0;


};

class DataMailboxErrorMessage : public DataMailboxMessage
{
public:
	typedef enum : char
	{
		GenericError = 0,
		QueueEmpty,
		TimedOut,
		SyscallInterrupted
	} enuErrorStatus;

	DataMailboxErrorMessage() : DataMailboxMessage(MessageDataType::enuType::DataMailboxErrorMessage), m_errorStatus(GenericError) {}
	DataMailboxErrorMessage(enuErrorStatus errorStatus) : DataMailboxMessage(MessageDataType::enuType::DataMailboxErrorMessage), m_errorStatus(errorStatus) {}

	virtual ~DataMailboxErrorMessage() = default;

	// DataMailboxErrorMessage does not carry any data
	// only error described by m_errorStatus
	void Serialize() {}
	void Deserialize() {}

	std::string getInfo()
	{
		return "DataMailboxErrorMessage - errorCode: " + std::to_string((int)m_errorStatus);
	}

	enuErrorStatus getErrorStatus() const { return m_errorStatus; }

private:
	enuErrorStatus m_errorStatus;
};

class DataMailbox
{
public:
	/**
	 * @brief Creates new DataMailbox object
	 * @param name Globally unique DataMailbox name.
	 * @param pLogger Pointer to a ILogger* inherited class to log information.
	 * @param mailboxAttributes DataMailbox attributes e.g. max message size and max message length. \see MailboxReference
	*/
	DataMailbox(const std::string name, ILogger* pLogger = NulLogger::getInstance(), const mq_attr& mailboxAttributes = MailboxReference::messageAttributes);
	~DataMailbox();

	// DataMailboxMessage* getReceivedMessagePointer() { return m_receivedMessage; }

	/**
	 * @brief Send `message` to `destination` DataMailbox. Guarantees that message will be delivered or it will block, possibly crash
	 * @param destination MailboxReference to another DataMailbox
	 * @param message Pointer to a class derived from DataMailboxMessage which represents the message.
	*/
	void send(MailboxReference& destination, DataMailboxMessage* message);

	/**
	 * @brief Send `message` to `destination` DataMailbox. Does not guarantee that message will be delivered.
	 * @param destination MailboxReference to another DataMailbox
	 * @param message Pointer to a class derived from DataMailboxMessage which represents the message.
	*/
	void sendConnectionless(MailboxReference& destination, DataMailboxMessage* message);

	/** TODO DOCS
	 * @brief Listens for messages until one is received. Does not discriminate between normal (`send()`) and connectionless (`sendConnectionless()`) messages.
	 * @param options enuReceiveOptions flags which determine how the message will be received (`NORMAL, TIMED, NONBLOCKING`). Specify multiple flags using | operator. (flag NORMAL has precedence)
	 * @return BasicDataMailboxMessage object which holds the serialized message and the message dataType. Unpacks to more specific message class.
	*/
	// BasicDataMailboxMessage receive(enuReceiveOptions options = enuReceiveOptions::NORMAL);
	DataMailboxMessage* receive(enuReceiveOptions options = enuReceiveOptions::NORMAL);

	// DataMailboxMessage* receiveAndDecode(enuReceiveOptions options = enuReceiveOptions::NORMAL);

	/**
	 * @brief Set the RTO of current mailbox (in seconds)
	 *
	 * @param RTO Request TimeOut
	 * @return struct timespec Previous value of the RTO (defined as timespec struct)
	 */
	struct timespec setRTO_s(time_t RTOs);

	/**
	 * @brief Set the RTO of current mailbox (in nanoseconds)
	 *
	 * @param RTOns Request TimeOut
	 * @return struct timespec Previous value of the RTO (defined as timespec struct)
	 */
	struct timespec setRTO_ns(long RTOns);

	/**
	 * @brief Set the Timeout settings object.
	 *
	 * @param _timeout_settings timespec struct with new timeout settings
	 */
	void setTimeout_settings(struct timespec _timeout_settings);

	/**
	 * @brief Set the Timeout settings object.
	 *
	 * @return struct timespec Value of the RTO (defined as timespec struct)
	 */
	struct timespec getTimeout_settings();

	/**
	 * @brief Get Message Queue (Mailbox) attributes
	 *
	 * @return mq_attr copy of the mq_attr object from current mailbox / message queue
	 */
	mq_attr getMQAttributes();

	/**
	 * @brief Set Message Queue (Mailbox) attributes
	 *
	 * @param message_queue_attributes reference to the mq_attr object of curretn mailbox / message queue
	 */
	void setMQAttributes(const mq_attr& message_queue_attributes);

private:
	ILogger* m_pLogger;

	SimplifiedMailbox m_mailbox;


	void logMessage(DataMailboxMessage* pMessage);
	DataMailboxErrorMessage* checkRawMessage(SimpleMailboxMessage& rawMessage);
	DataMailboxMessage* decodeRawMessage(SimpleMailboxMessage& rawMessage);
};


class BasicDataMailboxMessage : public DataMailboxMessage
{

public:
	BasicDataMailboxMessage();
	BasicDataMailboxMessage(MessageDataType dataType, const MailboxReference& source);
	BasicDataMailboxMessage(SimpleMailboxMessage& rawMessage);

	BasicDataMailboxMessage(const BasicDataMailboxMessage&) = delete;
	BasicDataMailboxMessage& operator=(const BasicDataMailboxMessage&) = delete;

	virtual ~BasicDataMailboxMessage() = default;

	virtual void Serialize() override;
	virtual void Deserialize() override;

	[[nodiscard]]
	ExtendedDataMailboxMessage* getOwnershipOfParsedMessage();

	/// Returns pointer to `m_serialized` - serialized raw data
	// char* getRawDataPointer() const;

	[[nodiscard]]
	DataSizePair getOwnershipOfRawData();

	/// Returns size of serialized raw data in `m_serialized` in bytes
	int getRawDataSize() const;

	/**
	* Relesases ownership of `m_serialized`. \n
	* BE SURE TO RELEASE OWNERSHIP AFTER TAKING IT AS TO AVOID UNDEFINED BEHAVIOUR \n
	* WHEN IT GETS FREED IN THE DESTRUCTOR OF THE BasicDataMailboxMessage. \n
	* \n
	* TODO: SMART POINTERS
	*/
	// void releaseRawDataOwnership();

	/// Sets the message source. Used internally.
	void setSource(const MailboxReference& source) { m_source = source; };
	virtual std::string getInfo() override;

private:
};

class InputParameter
{
	public:

	typedef enum : char
	{
		Empty = 0,
		KeypadPIN,
		KeypadCommand,
		PlainData,
		Enter,
		Cancel,
		RFIDCard,
		LCD_Message_Permanent,
		LCD_Message_wTimeout,
		LCD_Clear_wDefaultMsg,
		DoorOpen_wBuzzerSuccess,
		BuzzerError,
		BuzzerSuccess,
		BuzzerPing
	} enuType;

	InputParameter()
		: m_type(Empty),
		m_data("")
	{}

	InputParameter(enuType type)
		: m_type(type),
		m_data("")
	{}

	InputParameter(enuType type, const std::string& data)
		: m_type(type),
		m_data(data)
	{}



	void writeSerializedDataToBuffer(IN OUT char* pBuffer);
	size_t DeserializeAndGetSize(char* pSerializedData);

	// unsigned char for m_data length
	size_t getSeralizedSize() const { return sizeof(m_type) + m_data.length() + sizeof(byte); }

	enuType getType() const { return m_type; }
	std::string getData() const { return m_data; }

	std::string getInfo() const;

private:
	enuType m_type;
	std::string m_data;

};



class CommandMessage : public ExtendedDataMailboxMessage
{
public:

	typedef enum : char
	{
		NONE = 0,
		AUTHENTICATE,
		ADD,
		REMOVE,
		SET_CLNC,
		GUEST_ACCESS_ENABLE,
		GUEST_ACCESS_DISABLE
	} enuCommand;

	CommandMessage(); // TODO Forbid sending with empty commandId
	CommandMessage(enuCommand commandId);
	virtual ~CommandMessage() = default;

	CommandMessage(const CommandMessage&) = delete;
	CommandMessage& operator=(const CommandMessage&) = delete;

	CommandMessage(CommandMessage&& other);
	CommandMessage& operator= (CommandMessage&& other);

	enuCommand getCommandId() const { return m_command; }
	
	void addParameter(const InputParameter& param) { m_parameters.push_back(param); }
	InputParameter getParameterAt(unsigned int index);
	int getParameterCount() const { return m_parameters.size(); }

	virtual void Serialize() override;
	virtual void Deserialize() override;
	virtual std::string getInfo() override;

private:

	enuCommand m_command;
	std::vector<InputParameter> m_parameters;

};

class WatchdogMessage : public ExtendedDataMailboxMessage
{
public:
	typedef enum : char
	{
		REGISTER_REQUEST = 0,
		REGISTER_REPLY,
		UNREGISTER_REQUEST,
		UNREGISTER_REPLY,
		UPDATE_SETTINGS,
		KICK,
		START,
		STOP,
		SYNC_REQUEST,
		SYNC_BROADCAST,
		TERMINATE_REQUEST,
		TERMINATE_BROADCAST,
		ANY,
		NONE
	} MessageClass;

	virtual void Serialize() override;
	virtual void Deserialize() override;
	virtual std::string getInfo() override;

	WatchdogMessage();

	WatchdogMessage(const std::string& name,
		const SlotSettings& settings,
		unsigned int PID,
		enuActionOnFailure onFailure = enuActionOnFailure::RESET_ONLY,
		MessageClass type = NONE);

	WatchdogMessage(const std::string& name,
		MessageClass type = NONE);

	WatchdogMessage(MessageClass type);

	WatchdogMessage(const WatchdogMessage&) = delete;
	WatchdogMessage& operator= (const WatchdogMessage&) = delete;

	WatchdogMessage(WatchdogMessage&& other);
	WatchdogMessage& operator= (WatchdogMessage&& other);

	std::string getName() const { return m_name; }

	MessageClass getMessageClass() const { return m_messageClass; }

	const SlotSettings& getSettings() const { return m_settings; }

	int getPID() const { return m_PID; }

	enuActionOnFailure getActionOnFailure() const { return m_onFailure; }

	static std::string getMessageClassName(MessageClass messageClass);
	std::string getMessageClassName() const { return getMessageClassName(m_messageClass); }

	void setOffset(unsigned int offset) { m_offset = offset; }
	int getOffset() const { return m_offset; }

private:
	enuActionOnFailure m_onFailure;

	std::string m_name;

	MessageClass m_messageClass;

	SlotSettings m_settings;

	unsigned int m_PID;

	int m_offset;
};

class DatabaseReply : public ExtendedDataMailboxMessage
{
public:
	typedef enum : char
	{
		NONE = 0,
		INSUFFICIENT_PERMISSIONS,
		SUCCESS,
		ERROR,
		INVALID_COMMAND,
		INVALID_PARAMETER,
		CLEARANCE
	} enuStatus;

	// TODO create DEFAULT_CLEARANCE
	DatabaseReply()
		: ExtendedDataMailboxMessage(MessageDataType::enuType::DatabaseReply),
		m_status(NONE),
		m_clearance(-1)
	{}

	DatabaseReply(enuStatus status)
		:
		ExtendedDataMailboxMessage(MessageDataType::enuType::DatabaseReply),
		m_status(status),
		m_clearance(-1)
	{}

	DatabaseReply(Clearance clearance)
		: ExtendedDataMailboxMessage(MessageDataType::enuType::DatabaseReply),
		m_status(CLEARANCE),
		m_clearance(clearance)
	{}

	virtual ~DatabaseReply(){}

	virtual void Serialize() override;
	virtual void Deserialize() override;
	virtual std::string getInfo() override;

	std::string getStatusName() const;

	enuStatus getReplyStatus() const { return m_status; }
	Clearance getClearance() const { return m_clearance; }
	
	

private:

	enuStatus m_status;
	Clearance m_clearance;

};


#endif