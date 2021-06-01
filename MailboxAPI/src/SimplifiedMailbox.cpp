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

#include "SimplifiedMailbox.hpp"

#include"Time.hpp"
#include"MailboxAutomaton.hpp"

#include<cstring>
#include<sstream>

#include<iostream>

#define TRACE(X) std::cout<<"Marker:"<<X<<std::endl;

timespec operator+(const timespec& t1, const timespec& t2);

void SimpleMailboxMessage::releaseAndResetResources()
{
    delete[] m_pData;
    m_pData = nullptr;

    m_header.m_payloadSize = 0;
}

SimpleMailboxMessage::~SimpleMailboxMessage()
{
    releaseAndResetResources();
}

void SimpleMailboxMessage::swap(SimpleMailboxMessage& other)
{
    releaseAndResetResources();

    m_header = other.m_header;
    m_destinationName = other.m_destinationName;
    m_sourceName = other.m_sourceName;

    DataSizePair temp = other.getOwnershipOfData();

    m_pData = temp.m_pData;
    m_header.m_payloadSize = temp.m_dataSize;
}

SimpleMailboxMessage::SimpleMailboxMessage(SimpleMailboxMessage&& other)
    : m_header(), m_sourceName(""), m_destinationName(""), m_pData(nullptr)
{
    swap(other);
}

SimpleMailboxMessage& SimpleMailboxMessage::operator=(SimpleMailboxMessage&& other)
{
    if (&other == this) return *this;

    swap(other);
    return *this;
}

void SimpleMailboxMessage::copy(const SimpleMailboxMessage& other)
{
    // Free currently held resources to avoid memory leaks
    releaseAndResetResources();

    // Copy data from 'other'
    m_header = other.m_header;
    m_destinationName = other.m_destinationName;
    m_sourceName = other.m_sourceName;
    m_pData = nullptr;

    // Deep copy of m_pData
    size_t dataSize = m_header.m_payloadSize;
    if (dataSize != 0)
    {
        m_pData = new char[dataSize];
        memcpy(m_pData, other.m_pData, dataSize);
    }
}

SimpleMailboxMessage::SimpleMailboxMessage(const SimpleMailboxMessage& other)
    : m_header(), m_sourceName(""), m_destinationName(""), m_pData(nullptr)
{
    copy(other);
}

SimpleMailboxMessage& SimpleMailboxMessage::operator=(const SimpleMailboxMessage& other)
{
    if (&other == this) return *this;

    copy(other);
    return *this;
}


DataSizePair SimpleMailboxMessage::getOwnershipOfData()
{
    char* pTemp = m_pData;
    size_t tempSize = m_header.m_payloadSize;

    m_pData = nullptr;
    m_header.m_payloadSize = 0;

    return DataSizePair(pTemp, tempSize);
}

SimplifiedMailbox::SimplifiedMailbox (const std::string& identifier, ILogger* p_logger, const struct mq_attr& _mailboxAttributes)
    :   MailboxReference(identifier, p_logger, _mailboxAttributes), m_messageBuffer(SimpleMailboxMessage{}), m_timedReceiveOverride(enuReceiveOptions::DONT_OVERRIDE)
{
    if (p_logger == nullptr)
        p_parentLogger = NulLogger::getInstance();

    m_pAutomaton = new MailboxAutomaton(0, this, p_parentLogger);
    if(m_pAutomaton->initialize() == false)
    {
        Kernel::Fatal_Error(name + ", m_siInstID: " + std::to_string(0) +" - Automaton cannot be initialized! Check the log files!");
    }

    setRTO_s(2);

    //struct mq_attr oldAttributes = getMQAttributes();
    //oldAttributes.mq_msgsize = MESSAGE_BUFFER_SIZE;
    //oldAttributes.mq_msgsize = 1000;
    //setMQAttributes(oldAttributes);

    setMQAttributes(_mailboxAttributes);

    clearAllMessages();
}

SimplifiedMailbox::~SimplifiedMailbox()
{
    *p_parentLogger << "Closing Mailbox " + name + "!";

    mq_close(fd);

    *p_parentLogger << "Mailbox " + name + " closed!"; 

    delete m_pAutomaton;
}


void SimplifiedMailbox::clearAllMessages()
{
    mq_attr previous_attributes = getMQAttributes();

    mq_attr new_attributes = previous_attributes;
    new_attributes.mq_flags |= O_NONBLOCK;

    setMQAttributes(new_attributes);

    int n = previous_attributes.mq_maxmsg;
    ssize_t sizeOfReceivedData = -1;
    char p_message[getMQAttributes().mq_msgsize + 1];

    do
    {
        sizeOfReceivedData = mq_timedreceive(fd, p_message, getMQAttributes().mq_msgsize, 0, &timeout_settings);
        *p_parentLogger << name + " - clearing messages..."; 
        --n;
    }
    while(sizeOfReceivedData > 0 && n > 0);

    *p_parentLogger << name + " - messages cleared";

    setMQAttributes(previous_attributes);
}



/*std::unique_ptr<SerializedMessage> SimplifiedMailbox::serializeMessage(const SimpleMailboxMessage& message)
{

    std::unique_ptr<SerializedMessage> serializedMessage = std::make_unique<SerializedMessage>();


    bool copyError = false;

    char* result = strncpy(serializedMessage->destination,
                   message.destination.getName().c_str(),
                   MAX_MESSAGE_LENGTH);

    serializedMessage->destination[MAX_MESSAGE_LENGTH] = '\0';
    copyError |= (result == nullptr);


  result = strncpy(serializedMessage->source,
                   message.source.getName().c_str(),
                   MAX_MESSAGE_LENGTH);

    serializedMessage->source[MAX_MESSAGE_LENGTH] = '\0';
    copyError |= (result == nullptr);


   
  result = strncpy(serializedMessage->type,
                   message.type.c_str(),
                   MAX_MESSAGE_LENGTH);

    serializedMessage->type[MAX_MESSAGE_LENGTH] = '\0';
    copyError |= (result == nullptr);


  result = strncpy(serializedMessage->content,
                   message.content.c_str(),
                   MAX_MESSAGE_LENGTH);

    serializedMessage->content[MAX_MESSAGE_LENGTH] = '\0';
    copyError |= (result == nullptr);



    if(copyError == true)
    {
        *p_parentLogger << name + "- Error while preparing the message for serialization";
        Kernel::Fatal_Error(name + "- Error while preparing the message for serialization");
        exit(-1);
    }

    *p_parentLogger << name + " - Message successfully prepared for serialization";

    return std::move( serializedMessage );
}*/

SimpleMailboxMessage SimplifiedMailbox::deserializeMessage(char* p_rawMessage)
{

    SimpleMailboxMessage message;
    memcpy((void*) &message.m_header, p_rawMessage, sizeof(SimpleMailboxMessageHeader));

    size_t sourceNameLength = message.m_header.m_sourceNameLength;
    size_t destinationNameLength = message.m_header.m_destinationNameLength;
    size_t payloadSize = message.m_header.m_payloadSize;

    int source_name_offset = sizeof(SimpleMailboxMessageHeader);
    int destination_name_offset = source_name_offset + sourceNameLength;
    int payload_offset = destination_name_offset + destinationNameLength;

    if (payloadSize > 0)
    {
        OWNER message.m_pData = new char[payloadSize];
        memcpy(message.m_pData, p_rawMessage + payload_offset, payloadSize);
    }
    else
    {
        message.m_pData = nullptr;
    }


    message.m_sourceName = std::string((char*) (p_rawMessage + source_name_offset), sourceNameLength);
    message.m_destinationName = std::string((char*) (p_rawMessage + destination_name_offset), destinationNameLength);

    return message;
    
}

bool SimplifiedMailbox::messageIsValid(const std::string& message)
{
    return true;
}

bool SimplifiedMailbox::receivedMessageIsValid(const std::string& message, int& delimiter)
{
    return true;
}

struct timespec SimplifiedMailbox::setRTO_s(time_t RTOs)
{
    struct timespec old_timeout_settings = timeout_settings;

    if(RTOs <= 0)
        Kernel::Fatal_Error(name + " - Timeout cannot be negative!");

    timeout_settings = Time::getRawTime();

    timeout_settings.tv_nsec = 0;
    timeout_settings.tv_sec = RTOs;

    return old_timeout_settings;
}

struct timespec SimplifiedMailbox::setRTO_ns(long RTOns)
{
    struct timespec old_timeout_settings = timeout_settings;

    if(RTOns <= 0)
        Kernel::Fatal_Error(name + " - Timeout cannot be negative!");

    RTOns %= 1000 * Time::ms_to_ns; // 1 second max value overflow protection

    timeout_settings = Time::getRawTime();

    timeout_settings.tv_nsec = RTOns;
    timeout_settings.tv_sec = 0;

    return old_timeout_settings;
}

void SimplifiedMailbox::setTimeout_settings(struct timespec _timeout_settings)
{
    /*
    if(_timeout_settings.tv_sec <= 0 != _timeout_settings.tv_nsec <= 0) // LOGICAL XOR
        timeout_settings = _timeout_settings;
    else
        Kernel::Fatal_Error(name + " - Timeout settings not valid. Mutually exclusive timespec fields are specified at the same time!");
     */

    timeout_settings = _timeout_settings;
}

struct timespec SimplifiedMailbox::getTimeout_settings()
{
    return timeout_settings;
}

mq_attr SimplifiedMailbox::getMQAttributes()
{
    mq_attr message_queue_attributes {
        .mq_flags = -1,
        .mq_maxmsg = -1,
        .mq_msgsize = -1,
        .mq_curmsgs = -1 
    };

    int status = mq_getattr(fd, &message_queue_attributes);
    if(status < 0)
    {
        Kernel::Warning("Cannot get mailbox attributes. MB: " + name);
    }

    return message_queue_attributes;
}

void SimplifiedMailbox::setMQAttributes(const mq_attr& message_queue_attributes)
{
    int status = -1;

    const struct mq_attr old_attributes = getMQAttributes();
    if(message_queue_attributes.mq_msgsize != old_attributes.mq_msgsize ||
       message_queue_attributes.mq_maxmsg  != old_attributes.mq_maxmsg) // Needs to be reset to change these parameters
    {
        mq_unlink(("/" + name).c_str());
        status = fd = mq_open(
                              ("/" + name).c_str(),
                              Kernel::IOMode::RW,
                              Kernel::Permission::OWNER_RW,
                              &message_queue_attributes
                             );
    }
    else
    {
        status = mq_setattr(fd, &message_queue_attributes, nullptr);
    }
    
    if(status < 0)
    {
        *p_parentLogger << "Cannot set mailbox attributes. MB: " + name;
        Kernel::Warning("Cannot set mailbox attributes. MB: " + name);
    }

}

SimpleMailboxMessage SimplifiedMailbox::receiveImmediate(enuReceiveOptions timed)
{
    char p_rawData[getMQAttributes().mq_msgsize + 1];
    memset(p_rawData, 0, sizeof(p_rawData));

    SimpleMailboxMessage deserializedMessage;

    if(timed % enuReceiveOptions::TIMED)
    {
        *p_parentLogger << name + " TIMED receive with parameters: { s: " + std::to_string(timeout_settings.tv_sec)
            +   ", ns: " + std::to_string(timeout_settings.tv_nsec) + " }";
        deserializedMessage = receiveImmediateTimed(p_rawData);
    }
    else if (timed % enuReceiveOptions::NONBLOCKING)
    {
        *p_parentLogger << name + " NONBLOCKING receive";
        deserializedMessage = receiveImmediateNonblocking(p_rawData);
    }
    else
    {
        *p_parentLogger << name + " NORMAL receive";
        deserializedMessage = receiveImmediateNormal(p_rawData);
    }
    if (deserializedMessage.m_header.m_type == enuMessageType::ERROR) // ******************
    {
        *p_parentLogger << name + " - received message is invalid!";
        Kernel::Fatal_Error(name + " - received message is invalid!");
    }

    *p_parentLogger << name + " - Message successfully received: \n\t\t"
                        + "  source: " + deserializedMessage.m_sourceName + "\n\t\t"
                        + ", destination: " + deserializedMessage.m_destinationName + "\n\t\t"
                        + ", type: " + toString(deserializedMessage.m_header.m_type) + "\n\t\t"
                        + ", length: " + std::to_string(deserializedMessage.m_header.m_payloadSize);

    return deserializedMessage;
}

SimpleMailboxMessage SimplifiedMailbox::receiveImmediateTimed(IN OUT char* p_rawData)
{
    if (p_rawData == nullptr)
    {
        *p_parentLogger << name + " - SimplfiedMailbox - receiveImmediate p_rawData cannot be nullptr";
        Kernel::Fatal_Error(name + " - SimplfiedMailbox - receiveImmediate p_rawData cannot be nullptr");
    }

    timespec current_time = Time::getRawTime();

    *p_parentLogger << "Current time: { s: " + std::to_string(current_time.tv_sec)
        + " , ns: " + std::to_string(current_time.tv_sec) + " }";

    timespec absolute_timeout_settings = current_time + timeout_settings;

    *p_parentLogger << "absoulte timeout settings time: { s: " + std::to_string(absolute_timeout_settings.tv_sec)
        + " , ns: " + std::to_string(absolute_timeout_settings.tv_sec) + " }";

    [[maybe_unused]]
    ssize_t sizeOfreceivedData = mq_timedreceive(fd,
        p_rawData,
        getMQAttributes().mq_msgsize,
        0,
        &absolute_timeout_settings);

    *p_parentLogger << "Size of received data: " + std::to_string(sizeOfreceivedData);


    int _errno = errno;
    if (sizeOfreceivedData <= 0 && _errno == ETIMEDOUT)
    {
        std::stringstream stringBuilder;

        stringBuilder << name + " receive timed out - No response!"
            << "\nTimeout info: "
            << "\n\ttv_sec = " << timeout_settings.tv_sec
            << "\n\ttv_nsec = " << timeout_settings.tv_nsec;

        *p_parentLogger << stringBuilder.str();


        SimpleMailboxMessageHeader timed_out_message_header;
        timed_out_message_header.m_type = enuMessageType::TIMED_OUT;
        SimpleMailboxMessage timed_out_message;
        timed_out_message.m_header = timed_out_message_header;

        return timed_out_message;
    }
    else if (sizeOfreceivedData <= 0 && _errno == EINTR)
    {
        *p_parentLogger << " receive syscall was interrupted by a signal!";


        SimpleMailboxMessageHeader error_message_header;
        error_message_header.m_type = enuMessageType::SYSCALL_INTERRUPTED;
        SimpleMailboxMessage error_message;
        error_message.m_header = error_message_header;

        return error_message;
    }
    else if (sizeOfreceivedData <= 0)
    {
        Kernel::Warning("MB: " + name + " receive error. Size of received data: " + std::to_string(sizeOfreceivedData) + " - errno: " + std::to_string(_errno));
    }

    SimpleMailboxMessage deserializedMessage = deserializeMessage(p_rawData);
    return deserializedMessage;
}

SimpleMailboxMessage SimplifiedMailbox::receiveImmediateNormal(IN OUT char* p_rawData)
{
    if (p_rawData == nullptr)
    {
        *p_parentLogger << name + " - SimplfiedMailbox - receiveImmediate p_rawData cannot be nullptr";
        Kernel::Fatal_Error(name + " - SimplfiedMailbox - receiveImmediate p_rawData cannot be nullptr");
    }

    [[maybe_unused]]
    ssize_t sizeOfreceivedData = mq_timedreceive(fd,
        p_rawData,
        getMQAttributes().mq_msgsize,
        0,
        nullptr);

    int _errno = errno;

    if (sizeOfreceivedData <= 0 && _errno == EINTR)
    {
        *p_parentLogger << " receive syscall was interrupted by a signal!";


        SimpleMailboxMessageHeader error_message_header;
        error_message_header.m_type = enuMessageType::SYSCALL_INTERRUPTED;
        SimpleMailboxMessage error_message;
        error_message.m_header = error_message_header;

        return error_message;
    }
    else if (sizeOfreceivedData <= 0)
    {
        Kernel::Warning("MB: " + name + " receive error. Size of received data: " + std::to_string(sizeOfreceivedData) + " - errno: " + std::to_string(_errno));
    }

    SimpleMailboxMessage deserializedMessage = deserializeMessage(p_rawData);
    return deserializedMessage;
}

SimpleMailboxMessage SimplifiedMailbox::receiveImmediateNonblocking(IN OUT char* p_rawData)
{
    if (p_rawData == nullptr)
    {
        *p_parentLogger << name + " - SimplfiedMailbox - receiveImmediate p_rawData cannot be nullptr";
        Kernel::Fatal_Error(name + " - SimplfiedMailbox - receiveImmediate p_rawData cannot be nullptr");
    }

    mq_attr oldAttributes = getMQAttributes();
    mq_attr newAttributes = oldAttributes;
    newAttributes.mq_flags |= O_NONBLOCK;
    setMQAttributes(newAttributes);

    [[maybe_unused]]
    ssize_t sizeOfreceivedData = mq_receive(fd,
        p_rawData,
        getMQAttributes().mq_msgsize,
        0);

    *p_parentLogger << "Size of received data: " + std::to_string(sizeOfreceivedData);

    setMQAttributes(oldAttributes);

    int _errno = errno;
    if (sizeOfreceivedData <= 0 && _errno == EAGAIN)
    {
        std::stringstream stringBuilder;

        stringBuilder << name + " receive timed out - No response!"
            << "\nTimeout info: "
            << "\n\ttv_sec = " << timeout_settings.tv_sec
            << "\n\ttv_nsec = " << timeout_settings.tv_nsec;

        *p_parentLogger << stringBuilder.str();


        SimpleMailboxMessageHeader nonblocking_empty_queue_message_header;
        nonblocking_empty_queue_message_header.m_type = enuMessageType::EMPTY;
        SimpleMailboxMessage nonblocking_empty_queue_message;
        nonblocking_empty_queue_message.m_header = nonblocking_empty_queue_message_header;

        return nonblocking_empty_queue_message;
    }
    else if (sizeOfreceivedData <= 0 && _errno == EINTR)
    {
        *p_parentLogger << " receive syscall was interrupted by a signal!";


        SimpleMailboxMessageHeader error_message_header;
        error_message_header.m_type = enuMessageType::SYSCALL_INTERRUPTED;
        SimpleMailboxMessage error_message;
        error_message.m_header = error_message_header;

        return error_message;
    }
    else if (sizeOfreceivedData <= 0)
    {
        Kernel::Warning("MB: " + name + " receive error. Size of received data: " + std::to_string(sizeOfreceivedData) + " - errno: " + std::to_string(_errno));
    }

    SimpleMailboxMessage deserializedMessage = deserializeMessage(p_rawData);
    return deserializedMessage;
}

void SimplifiedMailbox::sendImmediate(const MailboxReference& destination, const enuMessageType type)
{

    if (destination.isValid() == false)
    {
        // TODO softer measures?
        Kernel::Fatal_Error("Trying to send to void (DEFAULT / NON-POINTING) destination.");
    }

    SimpleMailboxMessage message;

    message.m_sourceName = this->getName();
    message.m_destinationName = destination.getName();
    message.m_pData = nullptr;

    message.m_header.m_sourceNameLength = message.m_sourceName.length();
    message.m_header.m_destinationNameLength = message.m_destinationName.length();
    message.m_header.m_payloadSize = 0;

    message.m_header.m_type = type;

    size_t sourceNameLength = message.m_header.m_sourceNameLength;
    size_t destinationNameLength = message.m_header.m_destinationNameLength;

    int source_name_offset = sizeof(SimpleMailboxMessageHeader);
    int destination_name_offset = source_name_offset + sourceNameLength;

    size_t serializedMessageLength = sizeof(SimpleMailboxMessageHeader) + sourceNameLength + destinationNameLength;
    char serializedMessage[serializedMessageLength];
    memset(serializedMessage, 0, serializedMessageLength);

    memcpy((void*) serializedMessage, (void*) &message.m_header, sizeof(SimpleMailboxMessageHeader));
    memcpy((void*) (serializedMessage + source_name_offset) , (const void*) message.m_sourceName.c_str(), sourceNameLength);
    memcpy((void*) (serializedMessage + destination_name_offset), (const void*) message.m_destinationName.c_str(), destinationNameLength);

    int result = mq_send(destination.getFileDescriptor(),
        serializedMessage,
        serializedMessageLength,
        0);

    int _errno = errno;
    if (result < 0)
    {
        Kernel::Fatal_Error(name + " Could not send the message. Errno code: " + std::to_string(_errno));

        return;
    }

    *p_parentLogger << name + " - Message successfully sent: \n\t\t"
        + "  source: " + message.m_sourceName + "\n\t\t"
        + ", destination: " + message.m_destinationName + "\n\t\t"
        + ", type: " + toString(message.m_header.m_type) + "\n\t\t"
        + ", length: " + std::to_string(message.m_header.m_payloadSize) + "\n\t\t"
        + ", raw packet length: " + std::to_string(serializedMessageLength);

    return;

}

void SimplifiedMailbox::sendImmediate(const mqd_t destination, const enuMessageType type)
{
    /*SimpleMailboxMessageHeader message_header;

    message_header.source = this->getFileDescriptor();
    message_header.destination = destination;
    message_header.type = type;
    message_header.p_data = nullptr;

    sendImmediate(message_header);*/

    Kernel::Warning(name + ": Trying to use deprecated function!");
}

void SimplifiedMailbox::sendImmediate(SimpleMailboxMessage& message)
{

    // Recalculate just to be sure...
    message.m_header.m_sourceNameLength = message.m_sourceName.length();
    message.m_header.m_destinationNameLength = message.m_destinationName.length();

    size_t sourceNameLength = message.m_header.m_sourceNameLength;
    size_t destinationNameLength = message.m_header.m_destinationNameLength;
    size_t payloadSize = message.m_header.m_payloadSize;

    int source_name_offset = sizeof(SimpleMailboxMessageHeader);
    int destination_name_offset = source_name_offset + sourceNameLength;
    int payload_offset = destination_name_offset + destinationNameLength;

    size_t serializedMessageLength = sizeof(SimpleMailboxMessageHeader) + sourceNameLength + destinationNameLength + payloadSize;
    char serializedMessage[serializedMessageLength]; // TODO member variable - reduces # of allocations
    memset(serializedMessage, 0, serializedMessageLength);

    memcpy((void*) serializedMessage, (const void*) &message.m_header, sizeof(SimpleMailboxMessageHeader));
    memcpy((void*) (serializedMessage + source_name_offset) , (const void*) message.m_sourceName.c_str(), sourceNameLength);
    memcpy((void*) (serializedMessage + destination_name_offset), (const void*) message.m_destinationName.c_str(), destinationNameLength);
    memcpy((void*) (serializedMessage + payload_offset), (const void*) message.m_pData, payloadSize);

    MailboxReference destination(message.m_destinationName);
    if (destination.isValid() == false)
    {
        // TODO softer measures?
        Kernel::Fatal_Error("Trying to send to void (DEFAULT / NON-POINTING) destination.");
    }

    int result = mq_send(destination.getFileDescriptor(),
                         serializedMessage,
                         serializedMessageLength,
                         0);

    int _errno = errno; 
    if(result < 0)
    {
       Kernel::Fatal_Error(name + " Could not send the message. Errno code: " + std::to_string(_errno));
        return;
    }

     *p_parentLogger << name + " - Message successfully sent: \n\t\t"
                        + "  source: " + message.m_sourceName + "\n\t\t"
                        + ", destination: " + message.m_destinationName + "\n\t\t"
                        + ", type: " + toString(message.m_header.m_type) + "\n\t\t"
                        + ", length: " + std::to_string(message.m_header.m_payloadSize) + "\n\t\t"
                        + ", raw packet length: " + std::to_string(serializedMessageLength);


    return;
}


OWNER MailboxAutomatonEvent_wMessage* SimplifiedMailbox::parseMessage(SimpleMailboxMessage& message)
{

    if(message.m_header.m_type == TIMED_OUT)
    {
        return new MailboxAutomatonEvent_wMessage(MailboxAutomaton::enuEvtTimedOut, &message);
    }

    else if(message.m_header.m_type == RTS && message.m_sourceName != m_messageBuffer.m_destinationName)
    {
       return new MailboxAutomatonEvent_wMessage(MailboxAutomaton::enuEvtRTS_Received, &message);
    }
    else if(message.m_header.m_type == CTS && message.m_sourceName == m_messageBuffer.m_destinationName)
    {
        return new MailboxAutomatonEvent_wMessage(MailboxAutomaton::enuEvtCTS_Received, &message);
    }
    else if(message.m_header.m_type == HOLD && message.m_sourceName == m_messageBuffer.m_destinationName)
    {
        return new MailboxAutomatonEvent_wMessage(MailboxAutomaton::enuEvtHoldReceived, &message);
    }
    else if(message.m_header.m_type == ACK && message.m_sourceName == m_messageBuffer.m_destinationName)
    {
        return new MailboxAutomatonEvent_wMessage(MailboxAutomaton::enuEvtACK_Received, &message);
    }
    else if (message.m_header.m_type == MESSAGE_CONNECTIONLESS)
    {
        return new MailboxAutomatonEvent_wMessage(MailboxAutomaton::enuEvtValidMsgConnectionless, &message);
    }
    else if (message.m_header.m_type == EMPTY)
    {
        return new MailboxAutomatonEvent_wMessage(MailboxAutomaton::enuEvtTimedOut, &message);
    }
    /*else
    {
        *p_parentLogger << name + " INVALID CTL SIGNAL RECEIVED: " + std::to_string(message.m_pHeader->m_type) + ", FROM: " + std::to_string(message.source); 
        Kernel::Fatal_Error(name + " - invalid CTL message received");
        return nullptr; // ERROR TODO ERROR STATE?
    }*/

    else if(message.m_sourceName == m_messageBuffer.m_destinationName)
    {
        return new MailboxAutomatonEvent_wMessage(MailboxAutomaton::enuEvtValidMsg, &message);
    }

    // else
    *p_parentLogger << name + " INVALID MESSAGE RECEIVED FROM: " + message.m_sourceName + ", TYPE: " + std::to_string(message.m_header.m_type);
    Kernel::Fatal_Error(name + " - invalid CTL message received");
    // m_pAutomaton->reset();
    return nullptr; // ERROR TODO ERROR STATE?

}

void SimplifiedMailbox::sendConnectionless(MailboxReference& destination, char* p_data, size_t data_size)
{

    SimpleMailboxMessage messageToBeSent;
    messageToBeSent.m_sourceName = this->getName();
    messageToBeSent.m_destinationName = destination.getName();

    // TODO
    // TEMPORARY POINTER TO DATA !!!
    // OWNERSHIP OF SOME OBJECT ON HIGHER LAYER !!!
    // MUST BE SET TO NULL AFTER USE
    // SO THAT ~SimpleMailboxMessage()
    // DOESN'T DELETE IT !!!!
    // ******************************
    messageToBeSent.m_pData = p_data;
    // ****************************** 

    messageToBeSent.m_header.m_type = MESSAGE_CONNECTIONLESS;
    messageToBeSent.m_header.m_payloadSize = data_size;

    messageToBeSent.m_header.m_sourceNameLength = messageToBeSent.m_sourceName.length();
    messageToBeSent.m_header.m_destinationNameLength = messageToBeSent.m_destinationName.length();


    m_messageBuffer = messageToBeSent; // ????

    sendImmediate(messageToBeSent);

    // TEMPORARY POINTER TO DATA !!!
    // OWNERSHIP OF SOME OBJECT ON HIGHER LAYER !!!
    // MUST BE SET TO NULL AFTER USE
    // SO THAT ~SimpleMailboxMessage()
    // DOESN'T DELETE IT !!!!
    // ******************************
    messageToBeSent.m_pData = nullptr;
    // ******************************
}

void SimplifiedMailbox::send(MailboxReference& destination, char* p_data, size_t data_size)
{
    SimpleMailboxMessage messageToBeSent;
    messageToBeSent.m_sourceName = this->getName();
    messageToBeSent.m_destinationName = destination.getName();


    // TEMPORARY POINTER TO DATA !!!
    // OWNERSHIP OF SOME OBJECT ON HIGHER LAYER !!!
    // MUST BE SET TO NULL AFTER USE
    // SO THAT ~SimpleMailboxMessage()
    // DOESN'T DELETE IT !!!!
    // ******************************
    messageToBeSent.m_pData = p_data; 
    // ****************************** 
                                             

    messageToBeSent.m_header.m_type = MESSAGE;
    messageToBeSent.m_header.m_payloadSize = data_size;

    messageToBeSent.m_header.m_sourceNameLength = messageToBeSent.m_sourceName.length();
    messageToBeSent.m_header.m_destinationNameLength = messageToBeSent.m_destinationName.length();


    m_messageBuffer = messageToBeSent; // ????
    
    // m_pAutomaton->clearErrorStatus();

    OWNER MailboxAutomatonEvent_wMessage* p_event = new MailboxAutomatonEvent_wMessage(MailboxAutomaton::enuEvtInitSending, &messageToBeSent);
    m_pAutomaton->processEvent(p_event);

    while(m_pAutomaton->taskCompleted() == false)
    {
        SimpleMailboxMessage msg = receiveImmediate(); // TODO timed
        p_event = parseMessage(msg);
        m_pAutomaton->processEvent(p_event);
    }

    if(m_qWaitingList.empty() == true)
        m_pAutomaton->processEvent(OWNER new MailboxAutomatonEvent_wMessage(MailboxAutomaton::enuEvtResetQueueEmpty));
    else
        m_pAutomaton->processEvent(OWNER new MailboxAutomatonEvent_wMessage(MailboxAutomaton::enuEvtResetQueueNotEmpty));

    // TEMPORARY POINTER TO DATA !!!
    // OWNERSHIP OF SOME OBJECT ON HIGHER LAYER !!!
    // MUST BE SET TO NULL AFTER USE
    // SO THAT ~SimpleMailboxMessage()
    // DOESN'T DELETE IT !!!!
    // ******************************
    messageToBeSent.m_pData = nullptr;
    // ******************************
}

SimpleMailboxMessage SimplifiedMailbox::receive(enuReceiveOptions options)
{
    // m_pAutomaton->clearErrorStatus();

    // DEBUG UNTESTED
    m_timedReceiveOverride = DONT_OVERRIDE;

    MailboxAutomatonEvent_wMessage* p_event = nullptr;

    
    if(options % enuReceiveOptions::IGNORE_QUEUE)
        p_event = new MailboxAutomatonEvent_wMessage(MailboxAutomaton::enuEvtStartReceivingIgnoreQueue);
    else
        p_event = new MailboxAutomatonEvent_wMessage(MailboxAutomaton::enuEvtStartReceiving);

    m_pAutomaton->processEvent(p_event);

    while(m_pAutomaton->taskCompleted() == false)
    {

        // SimpleMailboxMessage msg = receiveImmediate(options | m_timedReceiveOverride); // TIMED ONLY WHILE WAITING FOR RTS ELSE IT WILL TIMEOUT DURING RECEPTION PROTOCOL
        enuReceiveOptions receiveOptions = options;
        if (m_timedReceiveOverride == OVERRIDE_NORMAL)
        {
            receiveOptions = NORMAL;
        }

        SimpleMailboxMessage msg = receiveImmediate(receiveOptions);
        p_event = parseMessage(msg);
        m_pAutomaton->processEvent(p_event);
    }

    SimpleMailboxMessage messageCopy = m_messageBuffer;
    m_messageBuffer.releaseAndResetResources();

    if(m_qWaitingList.empty() == true)
        m_pAutomaton->processEvent(new MailboxAutomatonEvent_wMessage(MailboxAutomaton::enuEvtResetQueueEmpty));
    else
        m_pAutomaton->processEvent(new MailboxAutomatonEvent_wMessage(MailboxAutomaton::enuEvtResetQueueNotEmpty));

    // DEBUG UNTESTED
    m_timedReceiveOverride = DONT_OVERRIDE;

    return messageCopy;
}

timespec operator+(const timespec& t1, const timespec& t2)
{
    timespec temp = {0, 0};

    temp.tv_sec = t1.tv_sec + t2.tv_sec;
    temp.tv_nsec = t1.tv_nsec + t2.tv_nsec; // TODO possible overflow

    int modulo = temp.tv_nsec % (1000 * Time::ms_to_ns);
    int carry = temp.tv_nsec / (1000 * Time::ms_to_ns);

    temp.tv_sec += carry;
    temp.tv_nsec = modulo;

    return temp;
}
