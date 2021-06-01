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

#ifndef MAILBOX_HPP
#define MAILBOX_HPP

#include"MailboxReference.hpp"

#include<queue>






/**
 * @brief Mailbox API class
 * 
 * Class used to pass messages between processes and threads. \n
 * \n
 * Derives from MailboxReference as a base class. \n
 */
class Mailbox : public MailboxReference
{
//protected:
public:


    /// Used to select normal or timed receive/send when using mailboxes
    enum Receive {NORMAL = false, TIMED = true};
    
    /// Sends "ACK" signal to `destination`
    void acknowledge(const MailboxReference& destination);

    void clearAllMessages();

    /**
     * @brief Receive raw message (`SENDER|MESSAGE` format)
     * 
     * @param mode Receive::NORMAL for non-timed and Receive::TIMED for timed
     * @return std::string returns raw message (`SENDER|MESSAGE` format)
     */
    std::string receiveRaw(Receive mode);

    /// Checks if `message` is valid for sending
    bool messageIsValid(const std::string& message);

    /// Checks if raw message (`message`) is valid and sets `delimiter` to delimiter (`|`) position in raw message (`SENDER|MESSAGE` format)
    bool receivedMessageIsValid(const std::string& message, int& delimiter);

    /**
     * @brief Checks for "ACK" message from `destination`. Returns false on timeout.
     * 
     * If receive timeouts 'TTL' times - returns `false` \n
     * \n
     * Ignores all messages not "ACK" from `destination` - decrements TTL. \n
     * \n
     * If message id received from someone other than `destination` - send "HOLD" signal \n
     * and queue it. \n
     * 
     * @param destination MailboxReference destination mailbox 
     * @return true Received "ACK" from `destination`
     * @return false NOT Received "ACK" from `destination`
     */
    bool waitForAcknowledgement(const MailboxReference& destination);

    /// Blocks and waits indefinitely for "CTS" signal from `destination`
    void hold(const MailboxReference& destination);

    /// Sends "CTS" signal to `destination`
    void clearToSend(const MailboxReference& destination);

    /// Sends "RTS" signal to `destination`
    void readyToSend(const MailboxReference& destination);

    /// Gets name of the first MailboxReference from the waiting list queue, or blocks and waits for "RTS" signal
    std::string getNext();

    /**
     * @brief Blocks and waits for message with specified parameters. Returns that message. USE OTHER (WRAPPER) FUNCTIONS IF POSSIBLE
     * 
     * `waitFor(rawMessage)` --  blocks and waits for message with content: `rawMessage` \n
     *                      from anyone. For every other `rawMessage` received it sends HOLD signal to the message source \n
     *                      and queues it. \n
     *                      EQUIVALENT TO `waitForMessage(const std::string& message)`
     * \n
     * \n
     * 'waitFor(rawMessage, sourceName)' -- blocks and waits for message with content: `rawMessage` from `sourceName` \n
     *                                          if `sourceName == ""` -> same as `waitFor(rawMessage)`. \n
     *                                          For every other `rawMessage` received and/or from someone other than `sourceName` \n
     *                                          it sends HOLD signal to the message source and queues it. \n
     *                                          EQUIVALENT TO `waitForMessage(const MailboxReference& source, const std::string& message)` \n
     * \n
     * \n
     * `Receive timed` argument sets if the function should block and wait indefinitely or it should timeout (specified by `Mailbox::RTO`). \n
     * On timeout it returns empty message (can be verified with 'bool mailbox_message::isEmpty()' on received message). \n
     * \n
     * \n
     * All wrapper functions: waitForMessage have timed wrapper function pairs.
     * 
     * @param rawMessage Target message to wait for.
     * @param sourceName Target source to listen messages from. Empty or "" (empty string) to receive from any source.
     * @param timed `Receive::NORMAL` for indefinite blocking or `Receive::TIMED` for timed blocking.
     * @return mailbox_message Returns message with specified parameters [message, source] (if and when it is received). `Receive::TIMED` returns empty `mailbox_message` on timeout. 
     */
    mailbox_message waitFor(const std::string& rawMessage, const std::string& sourceName = "", Receive timed = Receive::NORMAL);

    /// \see mailbox_message waitFor(const std::string& rawMessage, const std::string& sourceName = "", Receive timed = Receive::NORMAL)
    mailbox_message waitForMessage(const MailboxReference& source, const std::string& message);

    /// \see mailbox_message waitFor(const std::string& rawMessage, const std::string& sourceName = "", Receive timed = Receive::NORMAL)
    mailbox_message waitForMessage(const std::string& message);

    /// \see mailbox_message waitFor(const std::string& rawMessage, const std::string& sourceName = "", Receive timed = Receive::NORMAL)
    mailbox_message timedWaitForMessage(const MailboxReference& source, const std::string& message);

    /// \see mailbox_message waitFor(const std::string& rawMessage, const std::string& sourceName = "", Receive timed = Receive::NORMAL)
    mailbox_message timedWaitForMessage(const std::string& message);

    //mailbox_message holdBuffer;

    /// waiting list of other mailboxed RTS (ready to send) messages to this mailbox.
    std::queue<MailboxReference> waitingList;

    /// Structure representing send/receive timeout in both `s` and `ns`
    struct timespec timeout_settings = {1, 0};

public:


    /**
     * @brief Set the RTO of current mailbox (in seconds)
     * 
     * @param RTO Request TimeOut
     * @return struct timespec Previous value of the RTO (defined as timespec struct)
     */
    struct timespec setRTO_s(time_t RTO);

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
   struct timespec getTimeout_settings(void);

    //bool RetransmitOnTimeout;

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

    int BaseTTL;
                             /**
                              * @brief Construct a new Mailbox object without logger
                              * 
                              * @param identifier String identifier (mailbox's name) - GLOBALLY UNIQUE
                              */
                             //Mailbox (const std::string& identifier);

                            /**
                             * @brief Construct a new Mailbox object with logger
                             * 
                             * @param identifier String identifier (mailbox's name) - GLOBALLY UNIQUE
                             * @param p_logger Pointer to a parent ILogger which is used as a logger. - NULL SAFE
                             */
                             Mailbox (const std::string& identifier, ILogger* p_logger = NulLogger::getInstance());

                             //Mailbox (const std::string& identifier,ILogger* p_logger, unsigned int _RTO, unsigned int _BaseTTL);


                             /**
                              * @brief Destroy the Mailbox object
                              * 
                              */
    virtual                  ~Mailbox (void);

            /**
             * @brief Block until message is received. On reception send "ACK" signal to sender.
             * 
             * @return std::string Mailbox message in raw format ("SENDER|CONTENT")
             */
             mailbox_message receive();

            /**
             * @brief Block until message is received.
             * 
             * @return std::string Mailbox message in raw format ("SENDER|CONTENT")
             */
             mailbox_message receiveWithoutAcknowledgement();


            /**
             * @brief Block until message is received or time runs out (RTO).
             * 
             * @return std::string Mailbox message in raw format ("SENDER|CONTENT")
             */
             mailbox_message timedReceive();

             mailbox_message timedReceiveWithoutAcknowledgement();

             /**
              * @brief Send message to MailboxReference.
              * 
              * Converts message to raw format ("SENDER|CONTENT") and sends it to the destination. \n
              * 
              * @param destination MailboxReference object which represents destination Mailbox
              * @param message Message string
              */
             void sendWithoutAcknowledgement(const MailboxReference& destination, const std::string& message);

             void send(const MailboxReference& destination, const std::string& message);

            /**
             * @brief Blocking receive message immediately
             * 
             * @param timed Receive::NORMAL for non-timed and Receive::TIMED for timed
             * @return mailbox_message Received message
             */
            mailbox_message receiveImmediate(Receive timed = Receive::NORMAL);
            
            /**
             * @brief Send receive message immediately
             * 
             * @param source MailboxReference source mailbox
             * @param message message to be sent
             */
            void sendImmediate(const MailboxReference& source, const std::string& message);

             /**
              * @brief Returns mailbox_message object from raw message.
              * 
              * @param rawMessage Raw message string to be decoded. ("SENDER|CONTENT")
              * @return mailbox_message mailbox_message object formed from raw message
              */
             mailbox_message decodeRawMessage(const std::string& rawMessage); 
};

#endif
