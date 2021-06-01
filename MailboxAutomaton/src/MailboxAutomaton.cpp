#include "MailboxAutomaton.hpp"

#include"SimplifiedMailbox.hpp"

#include<sstream>

#include<iostream>


MailboxAutomatonEvent_wMessage::MailboxAutomatonEvent_wMessage(int idEvent, SimpleMailboxMessage* p_message)
    :   m_pMessage(p_message)
{
    m_iEventID = idEvent;

    switch(m_iEventID)
    {
        case MailboxAutomaton::enuEvtStartReceiving :
        m_sDesc = "enuEvtStartReceiving";
        break;

        case MailboxAutomaton::enuEvtStartReceivingIgnoreQueue :
        m_sDesc = "enuEvtStartReceivingIgnoreQueue";
        break;
        
        case MailboxAutomaton::enuEvtStartReceivingConnectionless:
        m_sDesc = "enuEvtStartReceivingConnectionless";
        break;

        case MailboxAutomaton::enuEvtRTS_Received :
        m_sDesc = "enuEvtRTS_Received";
        break;
        
        case MailboxAutomaton::enuEvtInitSending :
        m_sDesc = "enuEvtInitSending";
        break;
        
        case MailboxAutomaton::enuEvtHoldReceived :
        m_sDesc = "enuEvtHoldReceived";
        break;
        
        case MailboxAutomaton::enuEvtCTS_Received :
        m_sDesc = "enuEvtCTS_Received";
        break;
        
        case MailboxAutomaton::enuEvtACK_Received :
        m_sDesc = "enuEvtACK_Received";
        break;
        
        case MailboxAutomaton::enuEvtValidMsg :
        m_sDesc = "enuEvtValidMsg";
        break;
        
        case MailboxAutomaton::enuEvtValidMsgConnectionless:
        m_sDesc = "enuEvtValidMsgConnectionless";
        break;

        case MailboxAutomaton::enuEvtResetQueueEmpty :
        m_sDesc = "enuEvtResetQueueEmpty";
        break;
        
        case MailboxAutomaton::enuEvtResetQueueNotEmpty :
        m_sDesc = "enuEvtResetQueueNotEmpty";
        break;

        case MailboxAutomaton::enuEvtTimedOut :
        m_sDesc = "enuEvtTimedOut";
        break;

    default:
        m_sDesc = "ERROR: Unknown Event!";

    }
}

MailboxAutomatonEvent_wMessage::~MailboxAutomatonEvent_wMessage()
{

}

MailboxAutomaton::MailboxAutomaton(short int siInstID, SimplifiedMailbox* pMailbox, ILogger* pLogger)
    :   MAutomat(siInstID), m_bFinished(false), m_pMessageBuffer(&pMailbox->m_messageBuffer)
{
    if(pLogger == nullptr)
        m_pLogger = NulLogger::getInstance();
    else
        m_pLogger = pLogger;

    m_pMailbox = pMailbox;
    if(m_pMailbox == nullptr)
        Fatal_Error("Mailbox pointer cannot be NULL!"); // possible name clash with Kernel::Fatal_Error

    m_iInitialStateID = enuIdleQueueEmpty;

    /**********************************************
     * TRANSITION TABLE INITIALIZATION            *
     **********************************************/

    MAutState* pState = nullptr;

    // enuIdleQueueEmpty = 0
    pState = new MAutState(enuIdleQueueEmpty);
    pState->addTransition(new MAutTransition(enuEvtStartReceiving, enuWaitingRTS, this, AUT_ACTION(doVoid)));
    pState->addTransition(new MAutTransition(enuEvtRTS_Received, enuIdleQueueNotEmpty, this, AUT_ACTION(doEnqueueAndHold)));
    pState->addTransition(new MAutTransition(enuEvtInitSending, enuRTS_Sent, this, AUT_ACTION(doSendRTS)));
    pState->addTransition(new MAutTransition(enuEvtStartReceivingIgnoreQueue, enuWaitingRTS, this, AUT_ACTION(doVoid)));
    // pState->addTransition(new MAutTransition(enuEvtStartReceivingConnectionless, enuWaitingMSG_Connectionless, this, AUT_ACTION(doVoid)));
    this->addAutState(pState);

    // enuIdleQueueNotEmpty = 1
    pState = new MAutState(enuIdleQueueNotEmpty);
    pState->addTransition(new MAutTransition(enuEvtInitSending, enuRTS_Sent, this, AUT_ACTION(doSendRTS)));
    pState->addTransition(new MAutTransition(enuEvtStartReceiving, enuWaitingMSG, this, AUT_ACTION(doDequeueAndCTS)));
    pState->addTransition(new MAutTransition(enuEvtRTS_Received, enuIdleQueueNotEmpty, this, AUT_ACTION(doEnqueueAndHold)));
    // pState->addTransition(new MAutTransition(enuEvtStartReceivingConnectionless, enuWaitingMSG_Connectionless, this, AUT_ACTION(doVoid)));
    pState->addTransition(new MAutTransition(enuEvtStartReceivingIgnoreQueue, enuWaitingRTS, this, AUT_ACTION(doVoid)));
    this->addAutState(pState);

    // enuRTS_Sent = 2
    pState = new MAutState(enuRTS_Sent);
    pState->addTransition(new MAutTransition(enuEvtRTS_Received, enuRTS_Sent, this, AUT_ACTION(doEnqueueAndHold)));
    pState->addTransition(new MAutTransition(enuEvtHoldReceived, enuHoldState, this, AUT_ACTION(doVoid)));
    pState->addTransition(new MAutTransition(enuEvtCTS_Received, enuWaitingACK, this, AUT_ACTION(doSendMsg)));
    pState->addTransition(new MAutTransition(enuEvtValidMsgConnectionless, enuRTS_Sent, this, AUT_ACTION(doVoid))); // TEMPORARY TODO
    this->addAutState(pState);

    // enuHoldState = 3
    pState = new MAutState(enuHoldState);
    pState->addTransition(new MAutTransition(enuEvtRTS_Received, enuHoldState, this, AUT_ACTION(doEnqueueAndHold)));
    pState->addTransition(new MAutTransition(enuEvtCTS_Received, enuWaitingACK, this, AUT_ACTION(doSendMsg)));
    pState->addTransition(new MAutTransition(enuEvtValidMsgConnectionless, enuHoldState, this, AUT_ACTION(doVoid))); // TEMPORARY TODO
    this->addAutState(pState);

    // enuWaitingACK = 4
    pState = new MAutState(enuWaitingACK);
    pState->addTransition(new MAutTransition(enuEvtRTS_Received, enuWaitingACK, this, AUT_ACTION(doEnqueueAndHold)));
    pState->addTransition(new MAutTransition(enuEvtACK_Received, enuActionSuccessfulyEnded, this, AUT_ACTION(doSetFinishedFlag)));
    pState->addTransition(new MAutTransition(enuEvtValidMsgConnectionless, enuWaitingACK, this, AUT_ACTION(doVoid))); // TEMPORARY TODO
    this->addAutState(pState);

    // enuWaitingRTS = 5
    pState = new MAutState(enuWaitingRTS);
    pState->addTransition(new MAutTransition(enuEvtRTS_Received, enuWaitingMSG, this, AUT_ACTION(doSendCTS)));
    pState->addTransition(new MAutTransition(enuEvtTimedOut, enuActionSuccessfulyEnded, this, AUT_ACTION(doSetTimedOutStatus)));
    pState->addTransition(new MAutTransition(enuEvtValidMsgConnectionless, enuActionSuccessfulyEnded, this, AUT_ACTION(doSaveReceivedMessage)));
    this->addAutState(pState);

    // enuWaitingMSG = 6
    pState = new MAutState(enuWaitingMSG);
    pState->addTransition(new MAutTransition(enuEvtRTS_Received, enuWaitingMSG, this, AUT_ACTION(doEnqueueAndHold)));
    pState->addTransition(new MAutTransition(enuEvtTimedOut, enuWaitingMSG, this, AUT_ACTION(doVoid)));
    pState->addTransition(new MAutTransition(enuEvtValidMsg, enuActionSuccessfulyEnded, this, AUT_ACTION(doSendACK)));
    pState->addTransition(new MAutTransition(enuEvtValidMsgConnectionless, enuWaitingMSG, this, AUT_ACTION(doVoid)));
    this->addAutState(pState);

    // enuActionSuccessfulyEnded = 7
    pState = new MAutState(enuActionSuccessfulyEnded);
    pState->addTransition(new MAutTransition(enuEvtRTS_Received, enuActionSuccessfulyEnded, this, AUT_ACTION(doEnqueueAndHold)));
    pState->addTransition(new MAutTransition(enuEvtResetQueueEmpty, enuIdleQueueEmpty, this, AUT_ACTION(doClearFinishedFlag)));
    pState->addTransition(new MAutTransition(enuEvtResetQueueNotEmpty, enuIdleQueueNotEmpty, this, AUT_ACTION(doClearFinishedFlag)));
    this->addAutState(pState);
    
}

void MailboxAutomaton::Fatal_Error(const std::string& errorMessage)
{
    std::string rawErrorMessage = std::to_string(m_siInstId) + " " + errorMessage;
    *m_pLogger << rawErrorMessage;
    Kernel::Fatal_Error(rawErrorMessage);
}

void MailboxAutomaton::Warning(const std::string& warningMessage)
{
    std::string rawWarningMessage = std::to_string(m_siInstId) + " " + warningMessage;
    *m_pLogger << rawWarningMessage;
    Kernel::Warning(rawWarningMessage);
}

bool MailboxAutomaton::initialize()
{
    return this->test();
}

bool MailboxAutomaton::taskCompleted() const
{
    return m_bFinished || (getErrorStatus() != enumErrorStatusTypes::AUT_OK);
}

/*mailbox_message MailboxAutomaton::getCurrentMessage()
{
    mailbox_message temp_message;
    temp_message.content = m_currentMessage;
    temp_message.destination = m_currentDestination.getName();

    return temp_message;
}*/

// INHERITED METHODS ===========================================

bool MailboxAutomaton::processEvent(MAutEvent* event)
{
    std::stringstream logStringBuilder;

    logStringBuilder << "\n******************************************************"
                     << "\nSTATE: " << getCurrentStateId() << "\n"
                     << "\nEVENT: " << event->getEventDesc() << " [ " << event->getEventId() << " ]"
                     << "\n******************************************************";

    *m_pLogger << logStringBuilder.str();

    return MAutomat::processEvent(event);
    
}

MailboxAutomaton::~MailboxAutomaton()
{

}

bool MailboxAutomaton::reset()
{
    m_iCurrentStateID = m_iInitialStateID;

    std::stringstream logStringBuilder;

    logStringBuilder << "\n******************************************************"
        << "\n******************************************************"
        << "\nAUTOMATON RESET"
        << "\nSTATE: " << getCurrentStateId()
        << "\n******************************************************"
        << "\n******************************************************";

    *m_pLogger << logStringBuilder.str();

    return true;
}

// AUTOMATON FUNCTIONS =========================================

bool MailboxAutomaton::doClearFinishedFlag(MAutEvent* event)
{
    

    m_bFinished = false;

    // m_pMessageBuffer->releaseAndResetResources();
    *m_pMessageBuffer = SimpleMailboxMessage();

    m_pMailbox->m_timedReceiveOverride = enuReceiveOptions::DONT_OVERRIDE;

    return true;
}

bool MailboxAutomaton::doSetFinishedFlag(MAutEvent* event)
{
    
    m_bFinished = true;

    return true;
}

bool MailboxAutomaton::doVoid(MAutEvent* event)
{
    
    // DO NOTHING
    return true;
}

bool MailboxAutomaton::doEnqueueAndHold(MAutEvent* event)
{
    MailboxAutomatonEvent_wMessage* p_event_wMessage= dynamic_cast<MailboxAutomatonEvent_wMessage*> (event); // TODO remove all casting

    MailboxReference destination(p_event_wMessage->m_pMessage->m_sourceName);

    m_pMailbox->sendImmediate(destination, enuMessageType::HOLD);

    m_pMailbox->m_qWaitingList.push(destination.getName());

    
    return true;
}

bool MailboxAutomaton::doSendRTS(MAutEvent* event)
{
    MailboxAutomatonEvent_wMessage* p_event_wMessage= dynamic_cast<MailboxAutomatonEvent_wMessage*> (event);

    *m_pMessageBuffer = *p_event_wMessage->m_pMessage;

    MailboxReference destination(p_event_wMessage->m_pMessage->m_destinationName);

    m_pMailbox->sendImmediate(destination, enuMessageType::RTS);

    return true;
}

bool MailboxAutomaton::doDequeueAndCTS(MAutEvent* event)
{
    MailboxAutomatonEvent_wMessage* p_event_wMessage= dynamic_cast<MailboxAutomatonEvent_wMessage*> (event);

    MailboxReference destination(m_pMailbox->m_qWaitingList.front()); // encapsulate in getNext() TODO
    m_pMailbox->m_qWaitingList.pop();                                 // ^^^^

    m_pMailbox->sendImmediate(destination, enuMessageType::CTS);

    m_pMessageBuffer->m_destinationName = destination.getName();

    return true;

}

bool MailboxAutomaton::doSendMsg(MAutEvent* event)
{
    MailboxAutomatonEvent_wMessage* p_event_wMessage= dynamic_cast<MailboxAutomatonEvent_wMessage*> (event);

    m_pMailbox->sendImmediate( *(m_pMessageBuffer) ); // TODO send by pointer

    return true;
}

#define TRACE(X) std::cout << X << std::endl;

bool MailboxAutomaton::doSendCTS(MAutEvent* event)
{

    MailboxAutomatonEvent_wMessage* p_event_wMessage= dynamic_cast<MailboxAutomatonEvent_wMessage*> (event);

    MailboxReference destination(p_event_wMessage->m_pMessage->m_sourceName);

    m_pMessageBuffer->m_destinationName = destination.getName();

    m_pMailbox->sendImmediate(destination, enuMessageType::CTS);

    m_pMailbox->m_timedReceiveOverride = enuReceiveOptions::OVERRIDE_NORMAL;

    return true;
}

bool MailboxAutomaton::doSendACK(MAutEvent* event)
{
    MailboxAutomatonEvent_wMessage* p_event_wMessage= dynamic_cast<MailboxAutomatonEvent_wMessage*> (event);

    MailboxReference destination(p_event_wMessage->m_pMessage->m_sourceName);

    m_pMailbox->sendImmediate(destination, enuMessageType::ACK);

    *m_pMessageBuffer = *p_event_wMessage->m_pMessage;

    // DO NOT MANUALLY CHANGE m_pMessage->m_pData -> It manages it automatically (frees it in dtor)
    // p_event_wMessage->m_pMessage->m_pData = 0; // Taking ownership of data -- DEPRECATED

    m_bFinished = true;

    return true;
}

bool MailboxAutomaton::doSetTimedOutStatus(MAutEvent* event)
{
    // timed out message
    MailboxAutomatonEvent_wMessage* p_event_wMessage = dynamic_cast<MailboxAutomatonEvent_wMessage*> (event);

    *m_pMessageBuffer = *p_event_wMessage->m_pMessage;

    m_bFinished = true;

    return true;
}

bool MailboxAutomaton::doSaveReceivedMessage(MAutEvent* event)
{
    MailboxAutomatonEvent_wMessage* p_event_wMessage = dynamic_cast<MailboxAutomatonEvent_wMessage*> (event);

    // Kernel::DumpRawData(p_event_wMessage->m_pMessage->m_pData, p_event_wMessage->m_pMessage->m_header.m_payloadSize, "dump_" + Time::getTime());

    *m_pMessageBuffer = *p_event_wMessage->m_pMessage;

    m_bFinished = true;

    return true;
}


// =============================================================