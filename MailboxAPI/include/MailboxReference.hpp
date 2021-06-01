#ifndef MAILBOX_REFERENCE_HPP
#define MAILBOX_REFERENCE_HPP

#include<mqueue.h>

#include"ILogger.hpp"
#include"NulLogger.hpp"
#include"Kernel.hpp"
#include "propertiesclass.h"


/*#ifndef MAX_MESSAGE_LENGTH
#define MAX_MESSAGE_LENGTH 200
#endif

#ifndef MESSAGE_BUFFER_SIZE
#define MESSAGE_BUFFER_SIZE MAX_MESSAGE_LENGTH + 1
#endif

#ifndef MAX_MESSAGES_IN_QUEUE
#define MAX_MESSAGES_IN_QUEUE 10
#endif*/

/* TODO ADD NAMES TO Logger& ??? */



/*
           struct mq_attr {
               long mq_flags;       // Flags: 0 or O_NONBLOCK
               long mq_maxmsg;      // Max. # of messages on queue
               long mq_msgsize;     // Max. message size (bytes)
               long mq_curmsgs;     // # of messages currently in queue
           };
*/
typedef struct mq_attr mq_attr;

typedef struct mailbox_message mailbox_message;

/**
 * @brief Structure which contains message and sender
 * 
 * @param sender Message sender
 * @param content Message content
 * 
 */
struct mailbox_message
{
    std::string sender;
    std::string destination;
    std::string content;
    mailbox_message() : sender(""), destination(""), content(""){};
    bool isEmpty() const {return sender == "" && destination == "" && content == "";}
};


// const std::string DEFAULT_NAME = "NO_DESTINATION";
const std::string DEFAULT_NAME = GlobalProperties::Get().MAILBOX_REFRENCE_DEFAULT_NAME;

/**
 * @brief Class wrapper for mailbox ID
 * 
 * Every mailbox is characterized with unique ID number (file descriptor). \n
 * MailboxReference class wraps that ID in a class. \n
 *  \n
 * Unique ID is created from sting identifier (mailbox's name) \n
 *  \n
 * To reference a mailbox in order to send messages to it, \n
 * First MailboxReference is created with the same string identifier \n
 * passed to its constructor. It then internally translates it to the mailbox ID. \n
 *  \n
 * Mailbox class uses MailboxReference objects as destination parameter to send messages. \n
 */
class MailboxReference
{
    //////////////////////////////////////////////////////////////////
    private:
    //////////////////////////////////////////////////////////////////
          void initialize   (const std::string& identifier, const struct mq_attr& _messageAttributes, bool unlink);
          void unlink_queue ();

          void move(MailboxReference&& other);

    //////////////////////////////////////////////////////////////////
    protected:
    //////////////////////////////////////////////////////////////////
                /// Pointer to a logger
                 ILogger*    p_parentLogger     = nullptr ;
                 /// mailbox unique ID (file descriptor)
                 mqd_t       fd                 = -1      ;
                 /// Mailbox name
                 std::string name               = ""      ;
                 bool m_isValid;

    /**
     * @brief Sets the MailboxReference name (UNIQUE)
     * 
     * By POSIX standards every mailbox name starts with, and \n
     * contains exactly one, forward-slash '/'. \n
     * \n
     * This method erases all forward-slashes except the one \n
     * in the first place and/or adds it if it's not present. \n
     * 
     * @param identifier String identifier (mailbox's name) - GLOBALLY UNIQUE
     */
    void setName (const std::string& identifier);

    //////////////////////////////////////////////////////////////////
    public:
    //////////////////////////////////////////////////////////////////

    const static mq_attr messageAttributes;

    /// Returns unique ID of current MailboxReference object
    const mqd_t getFileDescriptor   (void) const;

    std::string getName () const;

    /**
     * @brief Construct a new Mailbox Reference object with logger attached.
     * 
     * @param identifier String identifier (mailbox's name) - GLOBALLY UNIQUE
     * @param p_logger Pointer to a parent ILogger which is used as a logger. - NULL SAFE
     */
                MailboxReference(const std::string& identifier, ILogger* p_logger = NulLogger::getInstance(), const struct mq_attr& _messageAttributes = messageAttributes, bool unlink = false);

                MailboxReference():MailboxReference(DEFAULT_NAME, nullptr){}

                MailboxReference(const MailboxReference& other) = default;
                MailboxReference& operator=(const MailboxReference& other) = default;

                MailboxReference(MailboxReference&& other);
                MailboxReference& operator=(MailboxReference&& other);
    /**
     * @brief Destroy the Mailbox Reference object
     * 
     */
    virtual     ~MailboxReference   (void);

    /**
     * @brief Returns name of a Mailbox referenced by this object
     * 
     * @return std::string Mailbox's name
     */
    std::string getName             (void);

    /// Returns true if MailboxReference is valid (not named DEFAULT_NAME)
    bool isValid() const { return m_isValid; }

    friend bool operator== (const MailboxReference& mbr_a, const MailboxReference& mbr_b);
    friend bool operator!= (const MailboxReference& mbr_a, const MailboxReference& mbr_b);
};

/**
 * @brief Mailbox properties
 * 
 * Static - common to all mailboxes
 * 
 * @param mq_maxmsg Max number of messages in queue
 * @param mq_msgsize Max message length
 */
// const mq_attr MailboxReference::messageAttributes = { mq_maxmsg: MAX_MESSAGES_IN_QUEUE, mq_msgsize: MAX_MESSAGE_LENGTH};
const mq_attr MailboxReference::messageAttributes = { mq_maxmsg: GlobalProperties::Get().QUEUE_SIZE, mq_msgsize : GlobalProperties::Get().MAX_MSG_SIZE };

//const mq_attr MailboxReference::messageAttributes = { mq_maxmsg: MAX_MESSAGES_IN_QUEUE, mq_msgsize: 404};

// const MailboxReference NO_DESTINATION("NO_DESTINATION"); // TODO block sending to NO_DESTINATION

#endif