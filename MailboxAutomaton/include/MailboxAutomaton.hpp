#ifndef MAILBOX_AUTOMATON_HPP
#define MAILBOX_AUTOMATON_HPP

#include"mautomat.h"
#include"SimplifiedMailboxEnums.hpp"

class SimplifiedMailbox;
class SimpleMailboxMessageHeader;
class SimpleMailboxMessage;


#define AUT_ACTION(X) (bool (MAutomat::*)(MAutEvent*)) &MailboxAutomaton::X

class MailboxAutomaton : public MAutomat
{
    public:
    MailboxAutomaton(short int siInstID,
                     SimplifiedMailbox* pMailbox,
                     ILogger* pLogger = nullptr);

    virtual ~MailboxAutomaton();

    bool initialize();

    // STATES ======================================================
    typedef enum
    {
        enuIdleQueueEmpty = 0,
        enuIdleQueueNotEmpty = 1,
        enuRTS_Sent = 2,
        enuHoldState = 3,
        enuWaitingACK = 4,
        enuWaitingRTS = 5,
        enuWaitingMSG = 6,
        enuActionSuccessfulyEnded = 7,
        enuWaitingMSG_Connectionless = 8

    } enumAutStateType;
    // =============================================================

    // EVENTS ======================================================
    typedef enum
    {
        enuEvtStartReceiving,
        enuEvtStartReceivingIgnoreQueue,
        enuEvtStartReceivingConnectionless,
        enuEvtRTS_Received,
        enuEvtInitSending,
        enuEvtHoldReceived,
        enuEvtCTS_Received,
        enuEvtACK_Received,
        enuEvtValidMsg,
        enuEvtValidMsgConnectionless,
        enuEvtResetQueueEmpty,
        enuEvtResetQueueNotEmpty,
        enuEvtTimedOut,

    } enumAutEventType;

    // ============================================================

    virtual bool reset();
    bool processEvent(MAutEvent* event);

    bool taskCompleted() const;

    SimpleMailboxMessage* m_pMessageBuffer;

    private:

    // PRIVATE DATA ================================================
    ILogger* m_pLogger;

    SimplifiedMailbox* m_pMailbox;

    bool m_bFinished;

    // =============================================================

    // PRIVATE HELPER FUNCTIONS ====================================
    void Fatal_Error(const std::string& errorMessage);

    void Warning(const std::string& warningMessage);
    // =============================================================

    // AUTOMATON FUNCTIONS =========================================

    bool doVoid(MAutEvent* event);

    bool doClearFinishedFlag(MAutEvent* event);

    bool doSetFinishedFlag(MAutEvent* event);

    bool doEnqueueAndHold(MAutEvent* event);

    bool doSendRTS(MAutEvent* event);

    bool doDequeueAndCTS(MAutEvent* event);

    bool doSendMsg(MAutEvent* event);

    bool doSendCTS(MAutEvent* event);
    
    bool doSendACK(MAutEvent* event);

    bool doSetTimedOutStatus(MAutEvent* event);

    bool doSaveReceivedMessage(MAutEvent* event);

    // =============================================================
};

/*class MailboxAutomatonEvent : public MAutEvent
{
    public:
    MailboxAutomatonEvent() {}
    MailboxAutomatonEvent(int idEvent);
    ~MailboxAutomatonEvent();
};*/

class MailboxAutomatonEvent_wMessage : public MAutEvent
{
    public:
    MailboxAutomatonEvent_wMessage(int idEvent, SimpleMailboxMessage* p_message = nullptr);

    virtual ~MailboxAutomatonEvent_wMessage();

    SimpleMailboxMessage* m_pMessage;
};



#endif