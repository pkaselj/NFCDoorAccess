#ifndef SIMPLIFIED_MAILBOX_HPP
#define SIMPLIFIED_MAILBOX_HPP

#include"MailboxReference.hpp"
#include"SimplifiedMailboxEnums.hpp"

#include<memory>
#include<queue>

#define OWNER
#define IN
#define OUT

struct DataSizePair
{
    DataSizePair() = default;
    ~DataSizePair() = default;

    DataSizePair(char* pData, size_t dataSize)
        : m_pData(pData), m_dataSize(dataSize)
    {}

    DataSizePair(const DataSizePair&) = default;
    DataSizePair& operator=(const DataSizePair&) = default;

    DataSizePair(DataSizePair&&) = default;
    DataSizePair& operator=(DataSizePair&&) = default;

    void Clear() { m_pData = nullptr; m_dataSize = 0; }

    char* m_pData = nullptr;
    size_t m_dataSize = 0;
};
 
/**
 * @brief Defines header which is prepended to the raw message (data) being sent.
*/
struct SimpleMailboxMessageHeader
{
    SimpleMailboxMessageHeader() : m_sourceNameLength(0), m_destinationNameLength(0), m_type(enuMessageType::ERROR), m_payloadSize(0){}

    char m_sourceNameLength;
    char m_destinationNameLength;
    enuMessageType m_type;
    size_t m_payloadSize;
};

/**
 * @brief Nonserialized packet which holds raw message and other information like header
 * m_pData MUST BE FREED MANUALLY ()
*/
struct SimpleMailboxMessage
{
    // TODO hide data?

    SimpleMailboxMessage() : m_header(), m_sourceName(""), m_destinationName(""), m_pData(nullptr) {};

    ~SimpleMailboxMessage();

    SimpleMailboxMessage(const SimpleMailboxMessage& other);
    SimpleMailboxMessage& operator=(const SimpleMailboxMessage& other);

    SimpleMailboxMessage(SimpleMailboxMessage&& other);
    SimpleMailboxMessage& operator=(SimpleMailboxMessage&& other);

    void releaseAndResetResources();

    [[nodiscard]]
    DataSizePair getOwnershipOfData();

    size_t getDataSize() const { return m_header.m_payloadSize; }

    // void setHeader(SimpleMailboxMessageHeader* p_header) { m_pHeader = p_header; }

    bool isTimedOut() const { return m_header.m_type == enuMessageType::TIMED_OUT; }
    bool isSyscallInterrupted() const { return m_header.m_type == enuMessageType::SYSCALL_INTERRUPTED; }

    SimpleMailboxMessageHeader m_header;
    std::string m_sourceName;
    std::string m_destinationName;
    char* m_pData;

private:
    void swap(SimpleMailboxMessage& other);
    void copy(const SimpleMailboxMessage& other);
};

class MailboxAutomaton;
class MailboxAutomatonEvent_wMessage;

class SimplifiedMailbox : public MailboxReference
{
    private:

    friend class MailboxAutomaton;
    MailboxAutomaton* m_pAutomaton = nullptr;

    //void* serializeMessage(const SimpleMailboxMessageHeader& message);

    /**
     * @brief Deserialized raw packet (serialized form of `struct SimpleMailboxMessage`)
     * @param p_rawMessage serialized form of `struct SimpleMailboxMessage`
     * @return SimpleMailboxMessage deserialized from p_rawMessage
    */
    SimpleMailboxMessage deserializeMessage(char* p_rawMessage);

    //std::queue<MailboxReference> m_qWaitingList;
    std::queue<std::string> m_qWaitingList; // set to MailboxReference ???
    SimpleMailboxMessage m_messageBuffer;

    /// Structure representing send/receive timeout in both `s` and `ns`
    struct timespec timeout_settings = {1, 0};

    /// Used by the automaton
    enuReceiveOptions m_timedReceiveOverride;

    /// INTERNAL USE. Takes `char* p_rawData` and receives message (TIMED) and writes it to `p_rawData` TODO
    SimpleMailboxMessage receiveImmediateTimed(IN OUT char* p_rawData);

    /// INTERNAL USE. Takes `char* p_rawData` and receives message (NORMAL) and writes it to `p_rawData` TODO
    SimpleMailboxMessage receiveImmediateNormal(IN OUT char* p_rawData);

    /// INTERNAL USE. Takes `char* p_rawData` and receives message (NONBLOCKING) and writes it to `p_rawData` TODO
    SimpleMailboxMessage receiveImmediateNonblocking(IN OUT char* p_rawData);

    public:

    /**
     * @brief Create a new SimplifiedMailbox object
     * @param identifier Mailbox name. - GLOBALLY UNIQUE
     * @param p_logger Pointer to an ILogger derived class to log messages to.
     * @param _messageAttributes Mailbox attributes
    */
    SimplifiedMailbox (const std::string& identifier,
        ILogger* p_logger = NulLogger::getInstance(),
        const struct mq_attr& _messageAttributes = MailboxReference::messageAttributes);

    ~SimplifiedMailbox ();

    /// Checks if `message` is valid for sending
    bool messageIsValid(const std::string& message);

    /// Checks if raw message (`message`) is valid and sets `delimiter` to delimiter (`|`) position in raw message (`SENDER|MESSAGE` format)
    bool receivedMessageIsValid(const std::string& message, int& delimiter);

    /// INTERNAL USE. Clears all the messages in the queue. TODO
    void clearAllMessages();

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

    /**
    * @brief Blocking receive message immediately
    * 
    * @param timed Receive::NORMAL for non-timed and Receive::TIMED for timed
    * @return mailbox_message Received message
    */
    SimpleMailboxMessage receiveImmediate(enuReceiveOptions timed = enuReceiveOptions::NORMAL);
    
    /**
    * @brief Send message immediately INTERNAL USE TODO
    * 
    * @param message SimpleMailboxMessage struct to be sent (destination taken from `message` struct)
    */
    void sendImmediate(SimpleMailboxMessage& message);

    /**
     * @brief Send signal immediately (RTS, CTS, ACK, HOLD, ...) INTERNAL USE TODO
     * @param destination Mailbox destination
     * @param type (RTS, CTS, ACK, HOLD, ...)
    */
    void sendImmediate(const MailboxReference& destination, const enuMessageType type);

    /**
     * @brief Send signal immediately (RTS, CTS, ACK, HOLD, ...) INTERNAL USE TODO
     * @param destination Mailbox destination (mailbox identifier - process local)
     * @param type (RTS, CTS, ACK, HOLD, ...)
    */
    void sendImmediate(const mqd_t destination, const enuMessageType type);

    /**
     * @brief Takes SimpleMailboxMessage `raw_message` and returns the MailboxAutomatonEvent_wMessage based on the message INTERNAL USE TODO
     * @param raw_message SimpleMailboxMessage `raw_message`
     * @return MailboxAutomatonEvent_wMessage containing pointer to the raw_message TODO UNSTABLE?
    */
    OWNER MailboxAutomatonEvent_wMessage* parseMessage(SimpleMailboxMessage& raw_message);

    /**
     * @brief Send message contained in `p_data` with size of `data_size` to `destination` DataMailbox. Guarantees that message will be delivered or it will block, possibly crash
     * @param destination MailboxReference to another DataMailbox
     * @param p_data Pointer to a raw message.
     * @param data_size Size of data pointed to by `p_data`
    */
    void send(MailboxReference& destination, char* p_data, size_t data_size);

    /**
     * @brief Send message contained in `p_data` with size of `data_size` to `destination` DataMailbox. Does not guarantee that message will be delivered.
     * @param destination MailboxReference to another DataMailbox
     * @param p_data Pointer to a raw message.
     * @param data_size Size of data pointed to by `p_data`
    */
    void sendConnectionless(MailboxReference& destination, char* p_data, size_t data_size);

    /**
     * @brief Listens for messages until one is received. Does not discriminate between normal (`send()`) and connectionless (`sendConnectionless()`) messages.
     * @param options enuReceiveOptions flags which determine how the message will be received (`NORMAL, TIMED, NONBLOCKING`). Specify multiple flags using | operator. (flag NORMAL has precedence)
     * @return SimpleMailboxMessage deserialized struct which holds the raw message and other information.
    */
    SimpleMailboxMessage receive(enuReceiveOptions options = enuReceiveOptions::NORMAL);
    
    
};

#endif // !