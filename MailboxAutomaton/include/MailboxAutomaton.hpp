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