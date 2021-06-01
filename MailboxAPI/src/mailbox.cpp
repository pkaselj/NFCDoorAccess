#include"mailbox.hpp"

#include"Time.hpp"

#include <cstring>

struct timespec Mailbox::setRTO_s(time_t RTOs)
{
    struct timespec old_timeout_settings = timeout_settings;

    if(RTOs <= 0)
        Kernel::Fatal_Error(name + " - Timeout cannot be negative!");

    timeout_settings = Time::getRawTime();

    timeout_settings.tv_nsec = 0;
    timeout_settings.tv_sec += RTOs;

    return old_timeout_settings;
}

struct timespec Mailbox::setRTO_ns(long RTOns)
{
    struct timespec old_timeout_settings = timeout_settings;

    if(RTOns <= 0)
        Kernel::Fatal_Error(name + " - Timeout cannot be negative!");

    timeout_settings = Time::getRawTime();

    timeout_settings.tv_nsec += RTOns;
    timeout_settings.tv_sec = 0;

    return old_timeout_settings;
}

void Mailbox::setTimeout_settings(struct timespec _timeout_settings)
{
    if((_timeout_settings.tv_sec) <= 0 != (_timeout_settings.tv_nsec <= 0)) // LOGICAL XOR
        timeout_settings = _timeout_settings;
    else
        Kernel::Fatal_Error(name + " - Timeout settings not valid. Mutually exclusive timespec fields are specified at the same time!");
}

struct timespec Mailbox::getTimeout_settings()
{
    return timeout_settings;
}

void Mailbox::acknowledge(const MailboxReference& destination)
{
    sendImmediate(destination, "ACK");
}

void Mailbox::clearAllMessages()
{
        mq_attr previous_attributes = getMQAttributes();
    
        mq_attr new_attributes = previous_attributes;
        new_attributes.mq_flags |= O_NONBLOCK;
    
        setMQAttributes(new_attributes);
    
        int n = previous_attributes.mq_maxmsg;
        mailbox_message message;
        do
        {
            message = receiveImmediate(Receive::TIMED);
            *p_parentLogger << name + " - clearing messages..."; 
            --n;
        }
        while(message.isEmpty() == false && n > 0);
    
        *p_parentLogger << name + " - messages cleared";
    
        setMQAttributes(previous_attributes);

}

std::string Mailbox::receiveRaw(Receive mode = Receive::NORMAL)
{
    char receivedData[getMQAttributes().mq_msgsize];
    memset(receivedData, 0, sizeof(receivedData));

    if(mode == Receive::TIMED)
    {

        [[maybe_unused]]
        ssize_t sizeOfreceivedData = mq_timedreceive(fd, receivedData, getMQAttributes().mq_msgsize, 0, &timeout_settings);
        int _errno = errno;
        if(sizeOfreceivedData <= 0)
        {
            Kernel::Warning("MB: " + name + " receive error. Size of received data: " + std::to_string(sizeOfreceivedData) + " - errno: " + std::to_string(_errno));
        }

        _errno = errno;
        if(_errno == ETIMEDOUT)
        {
            *p_parentLogger << name + " receive timed out - No response!";
            return "";
        }

    }
    else if(mode == Receive::NORMAL)
    {

        [[maybe_unused]]
        ssize_t sizeOfreceivedData = mq_receive(fd, receivedData, getMQAttributes().mq_msgsize, 0);
        int _errno = errno;
        if(sizeOfreceivedData <= 0)
        {
            Kernel::Warning("MB: " + name + " receive error. Size of received data: " + std::to_string(sizeOfreceivedData) + " - errno: " + std::to_string(_errno));
        }

    }

    std::string receivedString(receivedData);
    if(receivedString.length() == 0)
    {
       *p_parentLogger << name + " received empty message!";
        return "";
    }

   *p_parentLogger << name + " Mailbox input: \"" + receivedString + "\"";
    return receivedString;
}

bool Mailbox::messageIsValid(const std::string& message)
{
    if(message == "")
    {
       *p_parentLogger << name + " Cannot send an empty message";
        return false;
    }
    else if(message.length() + name.length() >= getMQAttributes().mq_msgsize)
    {
       *p_parentLogger << name + " Message size limit exceeded. Sending aborted";
        return false;
    }

    return true;

}

bool Mailbox::receivedMessageIsValid(const std::string& message, int& delimiter)
{
    if(message.length() < 3)
    {
       *p_parentLogger << "Received message \"" + message + "\" is invalid (less than 3 characters)";
       return false;
    }

    delimiter = message.find('|');
    if(delimiter <= 0)
    {
       *p_parentLogger << "Received message: \"" + message + "\" invalid form - missing delimiter '|'";
        return false;
    }

    return true;
}

bool Mailbox::waitForAcknowledgement(const MailboxReference& destination)
{
    std::string destinationName = destination.getName();

    mailbox_message message;
    int TTL = BaseTTL;

    do
    {
        message = timedReceiveWithoutAcknowledgement();
        if(message.sender == destinationName && message.content == "ACK")
        {
            *p_parentLogger << destinationName + " acknowledged the message!";
            return true;
        }
            
        *p_parentLogger << "Mailbox - No response from: " + destinationName;
        --TTL;
        *p_parentLogger << "TTL: " + std::to_string(TTL);
    }while (TTL > 0);
    
    *p_parentLogger << destinationName + " timed out!";
    return false;

}

/*Mailbox::Mailbox(const std::string& identifier,ILogger* p_logger, unsigned int _RTO, unsigned int _BaseTTL)
    : MailboxReference(identifier, p_logger)
{
    RTOns = _RTOns;
    BaseTTL = _BaseTTL;
}*/

Mailbox::Mailbox(const std::string& identifier,ILogger* p_logger)
    :   MailboxReference(identifier, p_logger)
{
    setRTO_s(2);
    BaseTTL = 5;

    clearAllMessages();

    //RetransmitOnTimeout = false;
}

/*Mailbox::Mailbox(const std::string& identifier)
    :   MailboxReference(identifier)
{
    RTOns = 2;
    BaseTTL = 5;
    //RetransmitOnTimeout = false;
}*/

mq_attr Mailbox::getMQAttributes()
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

void Mailbox::setMQAttributes(const mq_attr& message_queue_attributes)
{
    int status = mq_setattr(fd, &message_queue_attributes, nullptr);
    if(status < 0)
    {
        Kernel::Warning("Cannot set mailbox attributes. MB: " + name);
    }
}

void Mailbox::sendImmediate(const MailboxReference& destination, const std::string& message)
{
    if(messageIsValid(message) == false)
        return;

    std::string stringToSend = name + "|" + message;
   *p_parentLogger << name + " Mailbox output: \"" + stringToSend + "\"";

    /*int result = mq_send(destination.getFileDescriptor(),
                         stringToSend.c_str(),
                         stringToSend.length() + 1,
                         0);*/

    int result = mq_send(destination.getFileDescriptor(),
                     stringToSend.c_str(),
                     getMQAttributes().mq_msgsize,
                     0);

    int _errno = errno; 
    if(result < 0)
    {
       *p_parentLogger << name + " Could not send the message: " + stringToSend + " . Errno code: " + std::to_string(_errno);
        return;
    }

    *p_parentLogger << name + ": Message successfully sent!";

    return;
}

void Mailbox::sendWithoutAcknowledgement(const MailboxReference& destination, const std::string& message)
{
    readyToSend(destination);
    mailbox_message receivedMessage = receiveImmediate();

    if(receivedMessage.sender != destination.getName())
    {
        *p_parentLogger << name + " Received invalid message from: " + receivedMessage.sender;
        return;
    }
    else
    {
        if(receivedMessage.content == "HOLD")
        {
            *p_parentLogger << name + ": Destination " + destination.getName() + " requested HOLD!";
            waitForMessage(destination, "CTS");

            *p_parentLogger << name + ": CTS signal received from " + destination.getName() + " - sending message!";
            sendImmediate(destination, message);
        }

        if(receivedMessage.content == "CTS")
        {
            *p_parentLogger << name + ": CTS signal received from " + destination.getName() + " - sending message!";
            sendImmediate(destination, message);
        }
    }

    return;
}

void Mailbox::send(const MailboxReference& destination, const std::string& message)
{
    sendWithoutAcknowledgement(destination, message);
    waitForMessage(destination, "ACK");
}

void Mailbox::hold(const MailboxReference& destination)
{
    sendImmediate(destination, "HOLD");
    waitingList.push(destination);
}

void Mailbox::clearToSend(const MailboxReference& destination)
{
    sendImmediate(destination, "CTS");
}

void Mailbox::readyToSend(const MailboxReference& destination)
{
    sendImmediate(destination, "RTS");
    //waitForMessage(destination, "CTS");
}

std::string Mailbox::getNext()
{
    std::string destination = "";

    if(waitingList.empty())
    {
        mailbox_message message = waitForMessage("RTS");
        if(message.isEmpty() == false)
            destination = message.sender;
        
    }
    else
    {
        destination = waitingList.front().getName();
        waitingList.pop();
    }

    return destination;
}

mailbox_message Mailbox::receiveImmediate(Receive timed)
{
    std::string messageRaw = receiveRaw(timed);
    return decodeRawMessage(messageRaw);
}

mailbox_message Mailbox::receiveWithoutAcknowledgement()
{
    mailbox_message message;

    std::string next = getNext();
    if(next == "")
        return message;

    clearToSend(next);

    message = receiveImmediate();

    while(message.sender != next)
    {
        hold(message.sender);
        message = receiveImmediate();
    }
    return message;
}

mailbox_message Mailbox::receive()
{
    mailbox_message message = receiveWithoutAcknowledgement();
    acknowledge(message.sender);
    return message;
}

mailbox_message Mailbox::timedReceiveWithoutAcknowledgement()
{
    mailbox_message message;

    std::string next = getNext();
    if(next == "")
        return message;

    clearToSend(next);

    message = receiveImmediate(Receive::TIMED);

    while(message.sender != next && message.isEmpty() == false)
    {
        hold(message.sender);
        message = receiveImmediate();
    }
    return message;
}


mailbox_message Mailbox::timedReceive()
{
    mailbox_message message = timedReceiveWithoutAcknowledgement();
    acknowledge(message.sender);
    return message;
}



mailbox_message Mailbox::decodeRawMessage(const std::string& rawMessage)
{
    mailbox_message message;
    message.content = "";
    message.sender = "";

    int delimiter = -1;

    if(receivedMessageIsValid(rawMessage, delimiter) == false)
        return message;

    message.sender  = rawMessage.substr(0, delimiter);
    message.content = rawMessage.substr(delimiter + 1);

   *p_parentLogger << name + " - message decoded: Sender - \"" + message.sender
                 + "\" , Content - \"" + message.content + "\"";

    return message;
}  

Mailbox::~Mailbox()
{
    *p_parentLogger << "Closing Mailbox " + name + "!";

    mq_close(fd);

    *p_parentLogger << "Mailbox " + name + " closed!"; 
}

mailbox_message Mailbox::waitForMessage(const MailboxReference& source, const std::string& message)
{
    return waitFor(message, source.getName());
}
mailbox_message Mailbox::waitForMessage(const std::string& message)
{
    return waitFor(message);
}
mailbox_message Mailbox::timedWaitForMessage(const MailboxReference& source, const std::string& message)
{
    return waitFor(message, source.getName(), Receive::TIMED);
}
mailbox_message Mailbox::timedWaitForMessage(const std::string& message)
{
    return waitFor(message, "", Receive::TIMED);
}

mailbox_message Mailbox::waitFor(const std::string& rawMessage, const std::string& sourceName, Receive timed)
{
    //*p_parentLogger << name + "mailbox Waiting for: " + rawMessage + " from: " + sourceName;

    mailbox_message message = receiveImmediate(timed);
    if(message.isEmpty())
       return message;

    if(sourceName == "")
    {
        if(message.content == rawMessage)
        {
            *p_parentLogger << name + " mailbox - target message (\"" + message.content + "\" / \"" + rawMessage + "\") received from: \"" + message.content + "\"!";
            return message;
        }
    }
    else if(message.sender == sourceName)
    {
        if(message.content == rawMessage)
        {
            *p_parentLogger << name + " mailbox - target message (\"" + message.content + "\" / \"" + rawMessage + "\") received from: \"" + message.content + "\"!";
            return message;
        }
        else
        {
            *p_parentLogger << name + " mailbox - target message (\"" + message.content + "\" / \"" + rawMessage + "\") received from: \"" + message.content + "\"!";
            return message;
        }
        
    }

    *p_parentLogger << name + " mailbox - !!!! target message (\"" + message.content + "\" / \"" + rawMessage + "\") NOT received from: \"" + message.content + "\"!";
    hold(sourceName);
    return waitFor(sourceName, rawMessage);
}