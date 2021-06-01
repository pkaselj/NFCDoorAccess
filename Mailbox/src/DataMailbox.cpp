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

#include "DataMailbox.hpp"

#include "Kernel.hpp"

#include "Time.hpp"

#include <cstring>
#include <sstream>
#include <fstream>
#include <numeric>


MessageDataType::MessageDataType(enuType messageDataType) : m_messageDataType(messageDataType)
{
	if (messageDataType == enuType::COUNT)
	{
		Kernel::Fatal_Error("Invalid message datatype!");
	}
};

// Possible name clash TODO?
std::string MessageDataType::toString() const
{
	unsigned int dataTypeId = (unsigned int)m_messageDataType;

	if (dataTypeId >= (unsigned int)enuType::COUNT)
	{
		return "ErrorType";
	}

	static const size_t numberOfDataTypes = (int)enuType::COUNT + 1;

	static std::array<std::string, numberOfDataTypes> names = {
		"NONE",
		"DataMailboxErrorMessage",
		"CommandMessage",
		"WatchdogMessage",
		"DatabaseReply",
		"COUNT"
	};

	return names[dataTypeId]; // Potential indexOutOfBounds exception
}

OWNER DataMailboxMessage* MessageDataType::createPointerToEmptyMessage()
{

	switch (m_messageDataType)
	{
	case enuType::CommandMessage:
		return new CommandMessage;

	case enuType::WatchdogMessage:
		return new WatchdogMessage;

	case enuType::DatabaseReply:
		return new DatabaseReply;

	}

	Kernel::Fatal_Error("MessageDataType::createPointerToEmptyMessage() invalid datatype: " + std::to_string((int)m_messageDataType));

	return nullptr;
}

void MessageDataType::Decode(char dataTypeId)
{
	if (dataTypeId < 0 || dataTypeId >= (char) enuType::COUNT)
	{

		// DEBUG - Create core dump
		abort();
		Kernel::Fatal_Error("Invalid data type ID!");
	}

	m_messageDataType = static_cast<enuType>(dataTypeId);

}



DataMailboxMessage::DataMailboxMessage()
	: m_dataType(MessageDataType::enuType::NONE),
	m_serialized(nullptr),
	m_sizeOfSerializedData(0)
{

}

DataMailboxMessage::DataMailboxMessage(MessageDataType dataType)
	: m_dataType(dataType),
	m_serialized(nullptr),
	m_sizeOfSerializedData(0)
{

}

DataMailboxMessage::DataMailboxMessage(DataMailboxMessage&& other)
	: m_dataType(other.m_dataType),
	m_serialized(other.m_serialized),
	m_sizeOfSerializedData(other.m_sizeOfSerializedData),
	m_source(other.m_source)
{
	other.m_serialized = nullptr;
	other.m_sizeOfSerializedData = 0;
}

DataMailboxMessage& DataMailboxMessage::operator=(DataMailboxMessage&& other)
{ // TODO swap function
	m_dataType = other.m_dataType;
	m_serialized = other.m_serialized;
	m_sizeOfSerializedData = other.m_sizeOfSerializedData;
	m_source = other.m_source;

	other.m_serialized = nullptr;
	other.m_sizeOfSerializedData = 0;

	return *this;
}

DataMailboxMessage::~DataMailboxMessage()
{
	deleteSerializedData();
}

void DataMailboxMessage::DumpSerialData(const std::string filepath)
{
	if (m_serialized == nullptr)
	{
		Kernel::Warning("Trying to dump message with no serialized data! ");
		Serialize();
	}

	std::ofstream dump(filepath + ".dump", std::ios::out | std::ios::binary);
	if (!dump)
	{
		Kernel::Warning("Cannot dump message: " + getInfo());
		return;
	}

	dump.write(m_serialized, m_sizeOfSerializedData);

	Kernel::Trace("Serial data dumped!");

	// deleteSerializedData();

	dump.close();
}

void DataMailboxMessage::decodeMessageDataType()
{
	// DataMailboxErrorMessages do not carry data (m_serialized),
	// they carry all information in their DataMailboxErrorMessage::m_errorStatus
	// which is set at construction time to indicate e.g. TIMEOUT, EMPTY_MESSAGE,
	// SYSCALL_INTERRUPTED, so their m_serialized is set to nullptr,
	// their m_dataType is set to DataMailboxErrorMessage at construction
	// and doesn't need (cannot) to be decoded from the first byte of m_serialized.
	if (m_dataType == MessageDataType::enuType::DataMailboxErrorMessage)
	{
		return;
	}
	else if (m_serialized == nullptr)
	{
		Kernel::Warning("m_serialized == nullptr in decodeMessageDataType(), MessageDataType: " + m_dataType.toString());
		Kernel::DumpRawData(this, sizeof(*this), "cannot_decode_nullptr_" + std::to_string(std::rand() % 1000));

		Kernel::Fatal_Error("Cannot decode message datatype from null pointer!");
	}

	char serializedDataType = 0;

	memcpy(&serializedDataType, m_serialized, sizeof(serializedDataType));

	m_dataType.Decode(serializedDataType);

	if (m_dataType.toChar() < 0 || m_dataType.toChar() >= (int)MessageDataType::enuType::COUNT)
	{
		Kernel::DumpRawData(m_serialized, m_sizeOfSerializedData, "invalid_message_datatype_pid_" + std::to_string( getpid() ) );
		Kernel::Fatal_Error("Message has invalid datatype: " + m_dataType.toString());
	}
		
}

void DataMailboxMessage::checkSerializedData()
{
	if (m_serialized == nullptr)
		Kernel::Fatal_Error("Cannot deserialize data from nullptr!");
	else if (m_sizeOfSerializedData == 0)
		Kernel::Fatal_Error("Cannot deserialize data from message with length 0!");
}

void DataMailboxMessage::deleteSerializedData()
{
	// TODO DEBUG Possible source of errors
	/*
	Thread 10:
		==14100== Invalid free() / delete / delete[] / realloc()
		==14100==    at 0x4849C38: operator delete[](void*) (vg_replace_malloc.c:649)
		==14100==    by 0x4A8BD27: DataMailboxMessage::deleteSerializedData() (DataMailbox.cpp:165)
		==14100==    by 0x4A8C60F: DataMailbox::sendConnectionless(MailboxReference&, DataMailboxMessage*) (DataMailbox.cpp:276)
		==14100==    by 0x489EA4B: WatchdogClient::sendSignal(WatchdogMessage::MessageClass, enuReceiveOptions) (WatchdogClient.cpp:179)
		==14100==    by 0x489F70F: WatchdogClient::sendAliveMessageToServer_TimeoutCallback(void*) (WatchdogClient.cpp:294)
		==14100==    by 0x48A180F: TimerCallbackFunctor<WatchdogClient>::operator()(void*) (Timer.hpp:121)
		==14100==    by 0x4B52C73: Timer::executeCallback(sigval) (Timer.cpp:94)
		==14100==    by 0x4BB2EAF: timer_sigev_thread (timer_routines.c:62)
		==14100==    by 0x48FA493: start_thread (pthread_create.c:486)
		==14100==    by 0x4EED577: ??? (clone.S:73)
		==14100==  Address 0x5293590 is 0 bytes inside a block of size 43 free'd
		==14100==    at 0x4849C38: operator delete[](void*) (vg_replace_malloc.c:649)
		==14100==    by 0x4A8BD27: DataMailboxMessage::deleteSerializedData() (DataMailbox.cpp:165)
		==14100==    by 0x4A8BEAF: ExtendedDataMailboxMessage::Unpack(BasicDataMailboxMessage&) (DataMailbox.cpp:196)
		==14100==    by 0x4A8CD23: DataMailbox::decodeRawMessage(SimpleMailboxMessage&) (DataMailbox.cpp:380)
		==14100==    by 0x4A8CEB3: DataMailbox::receive(enuReceiveOptions) (DataMailbox.cpp:392)
		==14100==    by 0x489EF67: WatchdogClient::Kick() (WatchdogClient.cpp:220)
		==14100==    by 0x14307: InputLogicThreadFunction() (Hardwared.cpp:118)
		==14100==    by 0x163AB: void std::__invoke_impl<void, void (*)()>(std::__invoke_other, void (*&&)()) (invoke.h:60)
		==14100==    by 0x15FDF: std::__invoke_result<void (*)()>::type std::__invoke<void (*)()>(void (*&&)()) (invoke.h:95)
		==14100==    by 0x16C4B: decltype (__invoke((_S_declval<0u>)())) std::thread::_Invoker<std::tuple<void (*)()> >::_M_invoke<0u>(std::_Index_tuple<0u>) (thread:244)
		==14100==    by 0x16C0B: std::thread::_Invoker<std::tuple<void (*)()> >::operator()() (thread:253)
		==14100==    by 0x16BE3: std::thread::_State_impl<std::thread::_Invoker<std::tuple<void (*)()> > >::_M_run() (thread:196)
		==14100==  Block was alloc'd at
		==14100==    at 0x484876C: operator new[](unsigned int) (vg_replace_malloc.c:425)
		==14100==    by 0x4A8BD83: DataMailboxMessage::deleteAndReallocateSerializedData(unsigned int) (DataMailbox.cpp:178)
		==14100==    by 0x4A8EB83: WatchdogMessage::Serialize() (DataMailbox.cpp:694)
		==14100==    by 0x4A8C5E7: DataMailbox::sendConnectionless(MailboxReference&, DataMailboxMessage*) (DataMailbox.cpp:272)
		==14100==    by 0x489EA4B: WatchdogClient::sendSignal(WatchdogMessage::MessageClass, enuReceiveOptions) (WatchdogClient.cpp:179)
		==14100==    by 0x489F70F: WatchdogClient::sendAliveMessageToServer_TimeoutCallback(void*) (WatchdogClient.cpp:294)
		==14100==    by 0x48A180F: TimerCallbackFunctor<WatchdogClient>::operator()(void*) (Timer.hpp:121)
		==14100==    by 0x4B52C73: Timer::executeCallback(sigval) (Timer.cpp:94)
		==14100==    by 0x4BB2EAF: timer_sigev_thread (timer_routines.c:62)
		==14100==    by 0x48FA493: start_thread (pthread_create.c:486)
		==14100==    by 0x4EED577: ??? (clone.S:73)
	*/
	delete[] m_serialized;
	m_serialized = nullptr;
	m_sizeOfSerializedData = 0;
		
}

void DataMailboxMessage::deleteAndReallocateSerializedData(size_t size)
{

	deleteSerializedData();

	if (size == 0)
	{
		m_sizeOfSerializedData = 0;
		m_serialized = nullptr;

		return;
	}

	m_sizeOfSerializedData = size;
	m_serialized = new char[m_sizeOfSerializedData];

	if (m_serialized == nullptr)
	{
		Kernel::Fatal_Error("Could not allocate memory in DataMailboxMessage::deleteAndReallocateSeializedData(...) !");
	}
}

ExtendedDataMailboxMessage::ExtendedDataMailboxMessage(MessageDataType dataType)
	:	DataMailboxMessage(dataType)
{

}

ExtendedDataMailboxMessage& ExtendedDataMailboxMessage::operator= (ExtendedDataMailboxMessage&& other)
{
	DataMailboxMessage::operator=(std::move(other));
	return *this;
}

void ExtendedDataMailboxMessage::Unpack(BasicDataMailboxMessage& message)
{
	DataSizePair temp = message.getOwnershipOfRawData();
	m_sizeOfSerializedData = temp.m_dataSize;
	m_serialized = temp.m_pData;

	Deserialize();

	m_source = message.getSource();

	deleteSerializedData();
}




DataMailbox::DataMailbox(const std::string name, ILogger* pLogger, const mq_attr& mailboxAttributes)
	: m_mailbox(name, pLogger, mailboxAttributes)
{
	if (pLogger == nullptr)
		pLogger = NulLogger::getInstance();

	m_pLogger = pLogger;

	*m_pLogger << "DataMailbox opened: " + name;
}

DataMailbox::~DataMailbox()
{
	*m_pLogger << "DataMailbox closed: " + m_mailbox.getName();
}

/*
void DataMailbox::clearMessageBuffer()
{
	if (m_receivedMessage != nullptr)
	{
		delete m_receivedMessage;
		m_receivedMessage = nullptr;
	}
}*/

void DataMailbox::logMessage(DataMailboxMessage* pMessage)
{
	if (pMessage == nullptr)
	{
		*m_pLogger << "Trying to log message whose pointer is nullptr!";
		Kernel::Warning("Trying to log message whose pointer is nullptr!");
		return;
	}

	std::stringstream stringbuilder;

	stringbuilder << "\n"
		<< "==========================================" << "\n"
		<< "Message: | " << pMessage->getInfo() << "\n"
		<< "==========================================";

	*m_pLogger << stringbuilder.str();
}

void DataMailbox::send(MailboxReference& destination, DataMailboxMessage* message)
{

	*m_pLogger << m_mailbox.getName() + " - sending message to - " + destination.getName();

	logMessage(message);

	message->Serialize();

	m_mailbox.send(destination, message->m_serialized, message->m_sizeOfSerializedData);

	message->deleteSerializedData();

	*m_pLogger << m_mailbox.getName() + " - message successfully sent to - " + destination.getName();

}

void DataMailbox::sendConnectionless(MailboxReference& destination, DataMailboxMessage* message)
{
	*m_pLogger << m_mailbox.getName() + " - sending message to - " + destination.getName() + " - CONNECTIONLESS";

	logMessage(message);

	message->Serialize();

	m_mailbox.sendConnectionless(destination, message->m_serialized, message->m_sizeOfSerializedData);

	message->deleteSerializedData();

	*m_pLogger << m_mailbox.getName() + " - message successfully sent to - " + destination.getName();
}

struct timespec DataMailbox::setRTO_s(time_t RTOs)
{
	timespec oldSettings = getTimeout_settings();
	m_mailbox.setRTO_s(RTOs);
	return oldSettings;
}

struct timespec DataMailbox::setRTO_ns(long RTOns)
{
	timespec oldSettings = getTimeout_settings();
	m_mailbox.setRTO_ns(RTOns);
	return oldSettings;
}

void DataMailbox::setTimeout_settings(struct timespec _timeout_settings)
{
	m_mailbox.setTimeout_settings(_timeout_settings);
}

struct timespec DataMailbox::getTimeout_settings()
{
	return m_mailbox.getTimeout_settings();
}

mq_attr DataMailbox::getMQAttributes()
{
	return m_mailbox.getMQAttributes();
}

void DataMailbox::setMQAttributes(const mq_attr& message_queue_attributes)
{
	m_mailbox.setMQAttributes(message_queue_attributes);
}


DataMailboxErrorMessage* DataMailbox::checkRawMessage(SimpleMailboxMessage& rawMessage)
{
	enuMessageType& msgType = rawMessage.m_header.m_type;

	// Message is of valid type
	if (msgType == enuMessageType::MESSAGE || msgType == enuMessageType::MESSAGE_CONNECTIONLESS)
	{
		return nullptr;
	}

	// Specific types of invalid messages
	switch (msgType)
	{
	case enuMessageType::TIMED_OUT:

		return new DataMailboxErrorMessage(DataMailboxErrorMessage::enuErrorStatus::TimedOut);

	case enuMessageType::EMPTY:

		return new DataMailboxErrorMessage(DataMailboxErrorMessage::enuErrorStatus::QueueEmpty);

	case enuMessageType::SYSCALL_INTERRUPTED:
		return new DataMailboxErrorMessage(DataMailboxErrorMessage::enuErrorStatus::SyscallInterrupted);
	}

	// Generic invalid message
	return new DataMailboxErrorMessage(DataMailboxErrorMessage::enuErrorStatus::GenericError);
}


DataMailboxMessage* DataMailbox::decodeRawMessage(SimpleMailboxMessage& rawMessage)
{
	DataMailboxErrorMessage* pErrorMessage = checkRawMessage(rawMessage);
	if (pErrorMessage != nullptr)
	{
		return pErrorMessage;
	}

	// DEBUG
	if (rawMessage.m_pData == nullptr)
	{
		Kernel::Warning("Received message which is neither error message nor real message! Bug! Raw data dumped! Ignoring...");
		Kernel::DumpRawData(&rawMessage, sizeof(rawMessage), "invalid_msg_" + std::to_string(std::rand() % 1000));
		// DEBUG
		std::cout << getpid() << " - Caught invalid msg: neither error msg nor carrying data!" << "Message Type: " << rawMessage.m_header.m_type << std::endl;
		// DEBUG - Create core dump
		// abort();
		return new DataMailboxErrorMessage;
	}
	// DEBUG

	// TODO Extract to BasicDataMailboxMessage?
	BasicDataMailboxMessage receivedMessage(rawMessage);
	/*
	receivedMessage.setSerializedData(rawMessage.m_pData, rawMessage.m_header.m_payloadSize);

	receivedMessage.decodeMessageDataType();
	receivedMessage.setSource(MailboxReference(rawMessage.m_sourceName));

	MessageDataType dataType = receivedMessage.getDataType();
	ExtendedDataMailboxMessage* pDecodedReceivedMessage = dynamic_cast<ExtendedDataMailboxMessage*>(dataType.createPointerToEmptyMessage());
	pDecodedReceivedMessage->Unpack(receivedMessage);

	return pDecodedReceivedMessage;
	*/

	return receivedMessage.getOwnershipOfParsedMessage();
}

DataMailboxMessage* DataMailbox::receive(enuReceiveOptions options)
{

	*m_pLogger << m_mailbox.getName() + " - waiting for message!";

	SimpleMailboxMessage rawMessage = m_mailbox.receive(options);

	DataMailboxMessage* pDecodedMessage = decodeRawMessage(rawMessage);

	if (pDecodedMessage == nullptr)
	{
		*m_pLogger << m_mailbox.getName() + " - receive() mesage is nullptr!";
		Kernel::Fatal_Error(m_mailbox.getName() + " - receive() mesage is nullptr!");
	}
	
	*m_pLogger << m_mailbox.getName() + " - message successfully received";
	
	logMessage(pDecodedMessage);
	
	return pDecodedMessage;
}


BasicDataMailboxMessage::BasicDataMailboxMessage()
	:	DataMailboxMessage()
{
	setSource(MailboxReference{});
}

BasicDataMailboxMessage::BasicDataMailboxMessage(MessageDataType dataType, const MailboxReference& source)
	:	DataMailboxMessage(dataType)
{
	setSource(source);
}

BasicDataMailboxMessage::BasicDataMailboxMessage(SimpleMailboxMessage& rawMessage)
	:	DataMailboxMessage()
{
	DataSizePair temp = rawMessage.getOwnershipOfData();

	char* pData = temp.m_pData;
	size_t dataSize = temp.m_dataSize;

	setSerializedData(pData, dataSize);
	decodeMessageDataType();
	setSource(MailboxReference(rawMessage.m_sourceName));
}

void BasicDataMailboxMessage::Serialize()
{
	deleteAndReallocateSerializedData(sizeof(m_dataType.toChar()));

	memcpy(&m_serialized, &m_dataType, m_sizeOfSerializedData);
}

void BasicDataMailboxMessage::Deserialize()
{
	checkSerializedData();

	decodeMessageDataType();
}

ExtendedDataMailboxMessage* BasicDataMailboxMessage::getOwnershipOfParsedMessage()
{
	DataMailboxMessage* pMsg = m_dataType.createPointerToEmptyMessage();

	ExtendedDataMailboxMessage* pTemp = dynamic_cast<ExtendedDataMailboxMessage*>(pMsg);
	if (pTemp == nullptr)
	{
		Kernel::Fatal_Error("BasicDataMailboxMessage::getOwnershipOfParsedMessage() error!" + std::string(__FILE__) + " " + std::to_string(__LINE__));
	}

	pTemp->Unpack(*this);

	return pTemp;
}

/*char* BasicDataMailboxMessage::getRawDataPointer() const
{
	return m_serialized;
}*/

int BasicDataMailboxMessage::getRawDataSize() const
{
	return m_sizeOfSerializedData;
}

/*void BasicDataMailboxMessage::releaseRawDataOwnership()
{
	m_serialized = 0;
	m_sizeOfSerializedData = 0;
}*/

DataSizePair BasicDataMailboxMessage::getOwnershipOfRawData()
{
	char* pTemp = m_serialized;
	size_t tempSize = m_sizeOfSerializedData;

	m_serialized = nullptr;
	m_sizeOfSerializedData = 0;

	return DataSizePair(pTemp, tempSize);
}


std::string BasicDataMailboxMessage::getInfo()
{
	return "BasicDataMailboxMessage - MessageDataType: " + m_dataType.toString() + " from: " + m_source.getName();
}


void InputParameter::writeSerializedDataToBuffer(IN OUT char* pBuffer)
{
	if (pBuffer == nullptr)
	{
		Kernel::Fatal_Error("While trying to write serialized data to buffer (InputParameter). Type:" + std::to_string((int)m_type) + ", Data: " + m_data);
	}

	size_t serializedDataSize = getSeralizedSize();
	memset(pBuffer, 0, serializedDataSize);

	byte dataSize = m_data.length();
	size_t dataSizeOffset = sizeof(m_type);
	size_t dataOffset = sizeof(dataSize) + dataSizeOffset;

	memcpy(pBuffer, &m_type, sizeof(m_type));
	memcpy(pBuffer + dataSizeOffset, &dataSize, sizeof(dataSize));
	memcpy(pBuffer + dataOffset, m_data.c_str(), m_data.length());
}

size_t InputParameter::DeserializeAndGetSize(char* pSerializedData)
{
	if (pSerializedData == nullptr)
	{
		Kernel::Fatal_Error("While trying to deserialize data. Serialized data buffer cannot be nullptr.");
	}

	m_type = enuType::Empty;
	m_data = "";
	
	memcpy(&m_type, pSerializedData, sizeof(m_type));

	byte dataSize = 0;
	size_t dataSizeOffset = sizeof(m_type);
	memcpy(&dataSize, pSerializedData + dataSizeOffset, sizeof(dataSize));
	
	char pDataBuffer[dataSize + 1];
	memset(pDataBuffer, 0, dataSize + 1);

	size_t dataOffset = dataSizeOffset + sizeof(dataSize);
	memcpy(pDataBuffer, pSerializedData + dataOffset, dataSize);
	m_data = std::string(pDataBuffer, dataSize);

	size_t totalSize = getSeralizedSize();
	return totalSize;

}

std::string InputParameter::getInfo() const
{
	std::array<std::string, 14> names =
	{
		"Empty",
		"KeypadPIN",
		"KeypadCommand",
		"PlainData",
		"Enter",
		"Cancel",
		"RFIDCard",
		"LCD_Message_Permanent",
		"LCD_Message_wTimeout",
		"LCD_Clear_wDefaultMsg",
		"DoorOpen_wBuzzerSuccess",
		"BuzzerError",
		"BuzzerSuccess",
		"BuzzerPing"
	};

	if ((int)m_type >= names.size())
	{
		return "INVALID getInfo() ARGUMENT! CHECK: " + std::string(__FILE__) + " at line: " + std::to_string(__LINE__); // Fatal Error?
	}

	return "Input Parameter [ " + names[(int)m_type] + " ] : " + m_data;
}

CommandMessage::CommandMessage()
	: ExtendedDataMailboxMessage(MessageDataType::enuType::CommandMessage), m_command(enuCommand::NONE), m_parameters()
{

}

CommandMessage::CommandMessage(CommandMessage&& other)
	:	ExtendedDataMailboxMessage(std::move(other)),
	m_command(other.m_command),
	m_parameters(other.m_parameters)
{

}
CommandMessage& CommandMessage::operator= (CommandMessage&& other)
{
	ExtendedDataMailboxMessage::operator=(std::move(other));

	m_command = other.m_command;
	m_parameters = other.m_parameters;

	return *this;
}

CommandMessage::CommandMessage(enuCommand commandId)
	: ExtendedDataMailboxMessage(MessageDataType::enuType::CommandMessage), m_command(commandId), m_parameters()
{

}

InputParameter CommandMessage::getParameterAt(unsigned int index)
{
	if (index >= m_parameters.size())
	{
		return InputParameter(); // TODO Fatal?
	}

	return m_parameters[index];
}

void CommandMessage::Serialize()
{

	// size computation
	byte parameterCount = m_parameters.size();

	size_t totalParametersSerializedSize = 0;
	for (const auto& param : m_parameters)
	{
		totalParametersSerializedSize += param.getSeralizedSize();
	}

	char serializedDataType = m_dataType.toChar();

	size_t sizeOfSerializedData = sizeof(serializedDataType) + sizeof(m_command) + sizeof(parameterCount) + totalParametersSerializedSize;


	// offset computation
	size_t commandIdOffset = sizeof(serializedDataType);
	size_t parameterCountOffset = sizeof(m_command) + commandIdOffset;
	size_t parametersOffset = sizeof(parameterCount) + parameterCountOffset;


	// serialization
	deleteAndReallocateSerializedData(sizeOfSerializedData);

	memcpy(m_serialized, &serializedDataType, sizeof(serializedDataType));
	memcpy(m_serialized + commandIdOffset, &m_command, sizeof(m_command));
	memcpy(m_serialized + parameterCountOffset, &parameterCount, sizeof(parameterCount));


	size_t currentOffset = parametersOffset;
	for (auto& param : m_parameters)
	{
		param.writeSerializedDataToBuffer(m_serialized + currentOffset);
		currentOffset += param.getSeralizedSize();
	}

}

void CommandMessage::Deserialize()
{
	checkSerializedData();

	char serializedDataType = 0;

	memcpy(&serializedDataType, m_serialized, sizeof(serializedDataType));
	m_dataType.Decode(serializedDataType);

	size_t commandIdOffset = sizeof(serializedDataType);
	memcpy(&m_command, m_serialized + commandIdOffset, sizeof(m_command));

	size_t parameterCountOffset = sizeof(m_command) + commandIdOffset;
	byte parameterCount = 0;
	memcpy(&parameterCount, m_serialized + parameterCountOffset, sizeof(parameterCount));

	m_parameters.reserve(parameterCount);
	m_parameters.clear();


	size_t currentOffset = sizeof(parameterCount) + parameterCountOffset;
	for (byte i = 0; i < parameterCount; ++i)
	{
		InputParameter param;
		size_t currentParamSize = param.DeserializeAndGetSize(m_serialized + currentOffset);
		currentOffset += currentParamSize;
		
		m_parameters.push_back(param);
	}
}

std::string CommandMessage::getInfo()
{
	std::stringstream stringBuilder;

	stringBuilder
		<< "\tCommandMessage" << "\n"
		<< "\tCommand: " << (int)m_command << "\n"
		<< "\tParameter Count: " << m_parameters.size() << "\n";

		for (const auto& param : m_parameters)
		{
			stringBuilder
				<< "\tParam Type: " << param.getType() << "\n"
				<< "\tParam Data: " << param.getData() << "\n";
		}

		return stringBuilder.str();
}

WatchdogMessage::WatchdogMessage()
	:	ExtendedDataMailboxMessage(MessageDataType::enuType::WatchdogMessage),
	m_name(""), m_messageClass(MessageClass::NONE), m_settings(), m_PID(0), m_onFailure(enuActionOnFailure::RESET_ONLY), m_offset(-1)
{

}

WatchdogMessage::WatchdogMessage(const std::string& name, const SlotSettings& settings, unsigned int PID, enuActionOnFailure onFailure, MessageClass type)
	: ExtendedDataMailboxMessage(MessageDataType::enuType::WatchdogMessage),
	m_name(name), m_settings(settings), m_messageClass(type), m_PID(PID), m_onFailure(onFailure), m_offset(-1)
{

}

WatchdogMessage::WatchdogMessage(const std::string& name,
	MessageClass type)
	: ExtendedDataMailboxMessage(MessageDataType::enuType::WatchdogMessage),
	m_name(name), m_messageClass(type), m_settings(), m_PID(0), m_onFailure(enuActionOnFailure::RESET_ONLY), m_offset(-1)
{

}

WatchdogMessage::WatchdogMessage(MessageClass type)
	:	ExtendedDataMailboxMessage(MessageDataType::enuType::WatchdogMessage),
	m_name(""), m_messageClass(MessageClass::NONE), m_settings(), m_PID(0), m_onFailure(enuActionOnFailure::RESET_ONLY), m_offset(-1)
{

}

WatchdogMessage::WatchdogMessage(WatchdogMessage&& other)
	: ExtendedDataMailboxMessage(std::move(other))
{
	m_name = other.m_name;
	m_messageClass = other.m_messageClass;
	m_settings = other.m_settings;
	m_PID = other.m_PID;
	m_onFailure = other.m_onFailure;
	m_offset = other.m_offset;

}

WatchdogMessage& WatchdogMessage::operator= (WatchdogMessage&& other)
{
	ExtendedDataMailboxMessage::operator=(std::move(other));

	// TODO swap function
	m_name = other.m_name;
	m_messageClass = other.m_messageClass;
	m_settings = other.m_settings;
	m_PID = other.m_PID;
	m_onFailure = other.m_onFailure;
	m_offset = other.m_offset;

	return *this;
}

void WatchdogMessage::Serialize()
{
	char serializedDataType = m_dataType.toChar();

	size_t messageClassOffset = sizeof(serializedDataType);
	size_t settingsOffset = messageClassOffset + sizeof(m_messageClass);
	size_t PID_Offset = settingsOffset + sizeof(m_settings);
	size_t actionOnFailureOffset = PID_Offset + sizeof(m_PID);
	size_t offsetOffset = actionOnFailureOffset + sizeof(m_onFailure);
	size_t nameOffset = offsetOffset + sizeof(m_offset);
	
	size_t sizeOfSerializedData = nameOffset + m_name.length();

	deleteAndReallocateSerializedData(sizeOfSerializedData);

	memcpy(m_serialized, &serializedDataType, sizeof(serializedDataType));
	memcpy(m_serialized + messageClassOffset, &m_messageClass, sizeof(m_messageClass));
	memcpy(m_serialized + settingsOffset, &m_settings, sizeof(m_settings));
	memcpy(m_serialized + PID_Offset, &m_PID, sizeof(m_PID));
	memcpy(m_serialized + actionOnFailureOffset, &m_onFailure, sizeof(m_onFailure));
	memcpy(m_serialized + offsetOffset, &m_offset, sizeof(m_offset));
	memcpy(m_serialized + nameOffset, m_name.c_str(), m_name.length());
}

void WatchdogMessage::Deserialize()
{
	checkSerializedData();

	char serializedDataType = 0;

	size_t messageClassOffset = sizeof(serializedDataType);
	size_t settingsOffset = messageClassOffset + sizeof(m_messageClass);
	size_t PID_Offset = settingsOffset + sizeof(m_settings);
	size_t actionOnFailureOffset = PID_Offset + sizeof(m_PID);
	size_t offsetOffset = actionOnFailureOffset + sizeof(m_onFailure);
	size_t nameOffset = offsetOffset + sizeof(m_offset);


	memcpy(&serializedDataType, m_serialized, sizeof(serializedDataType));
	memcpy(&m_messageClass, m_serialized + messageClassOffset, sizeof(m_messageClass));
	memcpy(&m_settings, m_serialized + settingsOffset, sizeof(m_settings));
	memcpy(&m_PID, m_serialized + PID_Offset, sizeof(m_PID));
	memcpy(&m_onFailure, m_serialized + actionOnFailureOffset, sizeof(m_onFailure));
	memcpy(&m_offset, m_serialized + offsetOffset, sizeof(m_offset));

	// DEBUG TEST
	size_t nameLength = m_sizeOfSerializedData - nameOffset;


	char* pTempName = new char[nameLength + 1];
	memcpy(pTempName, m_serialized + nameOffset, nameLength);
	pTempName[nameLength] = 0;
	m_name = std::string(pTempName);
	delete[] pTempName;
	// DEBUG



	// m_name = std::string(m_serialized + nameOffset, m_sizeOfSerializedData - nameOffset);

	m_dataType.Decode(serializedDataType);

}

std::string WatchdogMessage::getInfo()
{
	std::array<std::string, 2> onFailureActionNamesList = { "RESET_ONLY", "KILL_ALL" };

	std::stringstream stringBuilder;

	// std::cout << "m_onFailure" << (int)m_onFailure << std::endl;

	stringBuilder << "\n"
		<< "WatchdogSlotRequestMessage - from: " << m_name << "\n"
		<< "\tPID:" << m_PID << "\n"
		<< "\tType: " << getMessageClassName(m_messageClass) << "\n"
		<< "\tOn faliure: " << onFailureActionNamesList.at((int)m_onFailure) << "\n"
		<< "\tOffset: " << m_offset << "\n"
		<< "\tSettings:" << "\n"
		<< "\t\tBaseTTL: " << m_settings.m_BaseTTL << "\n"
		<< "\t\tTimeout: " << m_settings.m_timeout_ms << " ms" << "\n";

	return stringBuilder.str();
}

std::string WatchdogMessage::getMessageClassName(MessageClass messageClass)
{
	std::array<std::string, 14> m_messageClassNames =
	{
		"REGISTER_REQUEST",
		"REGISTER_REPLY",
		"UNREGISTER_REQUEST",
		"UNREGISTER_REPLY",
		"UPDATE_SETTINGS",
		"KICK",
		"START",
		"STOP",
		"SYNC_REQUEST",
		"SYNC_BROADCAST",
		"TERMINATE_REQUEST",
		"TERMINATE_BROADCAST",
		"ANY",
		"NONE"
	};

	return m_messageClassNames.at((int)messageClass);
}

void DatabaseReply::Serialize()
{
	char serializedDataType = m_dataType.toChar();

	size_t statusOffset = sizeof(serializedDataType);
	size_t clearanceOffset = statusOffset + sizeof(m_status);

	size_t sizeOfSerializedData = clearanceOffset + sizeof(m_clearance);

	deleteAndReallocateSerializedData(sizeOfSerializedData);

	memcpy(m_serialized, &serializedDataType, sizeof(serializedDataType));
	memcpy(m_serialized + statusOffset, &m_status, sizeof(m_status));
	memcpy(m_serialized + clearanceOffset, &m_clearance, sizeof(m_clearance));
}

void DatabaseReply::Deserialize()
{
	checkSerializedData();

	char serializedDataType = 0;

	size_t statusOffset = sizeof(serializedDataType);
	size_t clearanceOffset = statusOffset + sizeof(m_status);

	memcpy(&serializedDataType, m_serialized, sizeof(serializedDataType));
	memcpy(&m_status, m_serialized + statusOffset, sizeof(m_status));
	memcpy(&m_clearance, m_serialized + clearanceOffset, sizeof(m_clearance));

	m_dataType.Decode(serializedDataType);
}

std::string DatabaseReply::getInfo()
{
	return "DatabaseReply -- Status [ " + getStatusName() + " ], Clearance [ " + std::to_string((int)m_clearance) + " ]";
}

std::string DatabaseReply::getStatusName() const
{
	int statusId = (unsigned int)m_status;

	std::array<std::string, 7> statusNames =
	{
		"NONE",
		"INSUFFICIENT_PERMISSIONS",
		"SUCCESS",
		"ERROR",
		"INVALID_COMMAND",
		"INVALID_PARAMETER",
		"CLEARANCE"
	};

	if (statusId >= statusNames.size())
	{
		return "INVALID STATUS: " + std::to_string(statusId);
	}

	return statusNames[statusId];
}
