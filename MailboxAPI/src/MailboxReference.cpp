#include"MailboxReference.hpp"

#include<string>
#include<fcntl.h>
#include<sys/stat.h>
#include<time.h>
#include<errno.h>

#include<sstream>

MailboxReference::MailboxReference(MailboxReference&& other)
{
    move(std::move(other));
}

MailboxReference& MailboxReference::operator=(MailboxReference&& other)
{
    if (this == &other)
    {
        return *this;
    }

    move(std::move(other));

    return *this;
}

void MailboxReference::initialize(const std::string& identifier, const struct mq_attr& _messageAttributes, bool unlink)
{

    if (p_parentLogger == nullptr)
        p_parentLogger = NulLogger::getInstance();

    if (identifier == DEFAULT_NAME)
    {
        // INFO: DEFAULT_NAME represents void MailboxReference (e.g. to indicate that message timed out)
        // So as to avoid opening and closing invalid/unused/void MailboxReference (which is a 'file' on *nix)
        // return skips that part. Be sure to check if MailboxReference is valid before sending message.
        m_isValid = false;
        name = identifier;

        *p_parentLogger << "Void mailbox opened: " + identifier;

        return;
    }


    setName(identifier);

    if(name == "")
    {
       *p_parentLogger << "Could not create a mailbox reference: " + identifier;
       Kernel::Fatal_Error("Could not create a mailbox reference: " + identifier);
    }

    // debug

    //mq_unlink(("/" + name).c_str());

    if (unlink == true)
    {
        unlink_queue();
    }

    // debug

    fd = mq_open(
                 ("/" + name).c_str(),
                 Kernel::IOMode::RW,
                 Kernel::Permission::OWNER_RW,
                 &_messageAttributes
                 );
    int _errno = errno;

    if(fd < 0)
    {
        std::stringstream error_message_ss;

        error_message_ss << "Could not open the mailbox reference: " << identifier << "\n"
            << "Errno code: " << _errno << "\n"
            << "messageAttributes: " << "\n"
            << "\tCurrent number of msg in queue: " << messageAttributes.mq_curmsgs << "\n"
            << "\tMQ flags: " << messageAttributes.mq_flags << "\n"
            << "\tMax no. of messages in queue: " << messageAttributes.mq_maxmsg << "\n"
            << "\tMax message size: " << messageAttributes.mq_msgsize << "\n";

        *p_parentLogger << error_message_ss.str();
        Kernel::Fatal_Error(error_message_ss.str());
    }

    m_isValid = true;

    *p_parentLogger << "Successfully opened the mailbox reference: " + identifier;
}

void MailboxReference::unlink_queue()
{
    if (fd == -1)
    {
        *p_parentLogger << name + " already unlinked";
        return;
    }

    int status_close = mq_close(fd);
    int _errno = errno;
    if (status_close != 0)
    {
        std::string error_message = name + " - could not be closed! Errno: " + std::to_string(_errno) + " FD: " + std::to_string(fd);
        Kernel::Warning(error_message);
        return;
    }

    fd = -1;

    // int status = mq_unlink(("/" + name).c_str());
    // int _errno = errno;
    // 
    // if (status != 0)
    // {
    //     std::string error_message = name + " - could not be unlinked!";
    //     if (_errno == EACCES)
    //     {
    //         *p_parentLogger << error_message + " Unsufficient permissions!";
    //         Kernel::Warning(error_message + " Unsufficient permissions!");
    //     }
    //     else if (_errno == ENOENT)
    //     {
    //         *p_parentLogger << error_message + " No message queue with specified name!";
    //     }
    //     else
    //     {
    //         *p_parentLogger << error_message + " Errno: " + std::to_string(_errno);
    //         Kernel::Warning(error_message + " Errno: " + std::to_string(_errno));
    //     }
    //     return;
    // }


    *p_parentLogger << name + " - unlinked!";
    // DEBUG
    // Kernel::Trace(name + " - unlinked!");
}

/*
*   Every POSIX message_queue identifier/name must begin with a forward slash == '/'
*   setName() takes that name, verifies it, modifies it if needed and assigns it to
*   class member std::string name.
*/
void MailboxReference::setName(const std::string& identifier)
{
    name = identifier;

    /* erase all forward slashes in name */
    size_t position = name.find('/', 0);
    while(position >= 0 && position < name.length())
    {
        name.erase(position, 1);
        position = name.find('/', 0);
    }
    

    return;
}


void MailboxReference::move(MailboxReference&& other)
{
    fd = other.fd;
    name = other.name;
    p_parentLogger = other.p_parentLogger;
    m_isValid = other.m_isValid;

    other.fd = -1;
    other.name = "NO_DESTINATION";
}

const mqd_t MailboxReference::getFileDescriptor(void) const
{
    return fd;
}

std::string MailboxReference::getName() const
{
    return name;
}

MailboxReference::MailboxReference(const std::string& identifier, ILogger* p_logger, const struct mq_attr& _messageAttributes, bool unlink)
{
    m_isValid = false;
    p_parentLogger = p_logger;
    initialize(identifier, _messageAttributes, unlink);
}

MailboxReference::~MailboxReference(void)
{
    //*p_parentLogger << "Closing MailboxReference " + name + "!" << std::endl;

    //DEBUG
    unlink_queue();

    //*p_parentLogger << "MailboxReference " + name + " closed!" << std::endl; 
}


std::string MailboxReference::getName()
{
    return name;
}

bool operator== (const MailboxReference& mbr_a, const MailboxReference& mbr_b)
{
    return mbr_a.name == mbr_b.name;
}

bool operator!= (const MailboxReference& mbr_a, const MailboxReference& mbr_b)
{
    return !(mbr_a == mbr_b);
}