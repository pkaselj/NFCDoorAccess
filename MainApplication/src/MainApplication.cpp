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

#include<unistd.h>
#include<cstdio>
#include<cstdlib>
#include<linux/input.h>
#include<linux/input-event-codes.h>
#include<cstring>
#include<string>
#include<errno.h>
#include<signal.h>

#include"Settings.hpp"
#include"DataMailbox.hpp"
#include"AutomatonPairFactory.hpp"
#include"WatchdogClient.hpp"
#include"IndicatorController.hpp"
#include "propertiesclass.h"


volatile sig_atomic_t globalTerminateFlag = 0;

const std::string HARDWARED_MAILBOX_NAME = GlobalProperties::Get().HARDWARED_MB_NAME;
const std::string DATABASE_GATEWAY_MAILBOX_NAME = GlobalProperties::Get().DBGW_MB_NAME;

const MailboxReference refKeypadMailbox(HARDWARED_MAILBOX_NAME);
const MailboxReference refDatabaseMailbox(DATABASE_GATEWAY_MAILBOX_NAME);


OWNER MainAutomatonEvent* parseMessageToMainAutomatonEvent(DataMailboxMessage* pMessage);

int main(void)
{

    UNIX_SignalHandler::bindSignalToFlag(UNIX_SignalHandler::enuSIGTERM, &globalTerminateFlag);

    Logger mailbox_logger("main.mailbox.log");
    Logger main_aut_logger("main.automaton.log");
    Logger keypad_aut_logger("keypad.automaton.log");
    Logger watchdog_logger("main.watchdog.log");
    Logger indicators_logger("indicators.client.log");

    const SlotSettings settings =
    {
        .m_BaseTTL = GlobalProperties::Get().MAIN_WD_TTL,
        .m_timeout_ms = GlobalProperties::Get().MAIN_WD_TIMEOUT_MS
    };

    const std::string MAIN_APP_WATCHDOG_NAME = GlobalProperties::Get().MAIN_WATCHDOG_NAME;
    const std::string WATCHDOG_SERVER_NAME = GlobalProperties::Get().WATCHDOG_SERVER_NAME;
    WatchdogClient watchdog(MAIN_APP_WATCHDOG_NAME, WATCHDOG_SERVER_NAME, settings, enuActionOnFailure::KILL_ALL, &watchdog_logger);

    const std::string DATABASE_GATEWAY_MAILBOX_NAME = GlobalProperties::Get().DBGW_MB_NAME;
    MailboxReference databaseGateway(DATABASE_GATEWAY_MAILBOX_NAME);

    const std::string MAIN_APP_MAILBOX_NAME = GlobalProperties::Get().MAIN_MB_NAME;
    DataMailbox mailbox(MAIN_APP_MAILBOX_NAME/*, &mailbox_logger*/);
    timespec timeoutSettings = Time::getTimespecFrom_ms(10);
    mailbox.setTimeout_settings(timeoutSettings);

    const std::string INDICATORS_MAILBOX_NAME = GlobalProperties::Get().INDICATORS_MAILBOX_NAME;
    IndicatorController_Client indicators(INDICATORS_MAILBOX_NAME, &indicators_logger);

    AutomatonPairFactory automata(&mailbox, &databaseGateway, &indicators, &main_aut_logger);

    MainAutomaton& mainAutomaton = automata.getMainAutomatonReference();
    KeypadAutomaton& keypadAutomaton = automata.getKeypadAutomatonReference();

    watchdog.Start();
    while (!globalTerminateFlag && watchdog.Kick() )
    {
        DataMailboxMessage* pMessage = mailbox.receive(enuReceiveOptions::TIMED);

        if (pMessage->getDataType() == MessageDataType::enuType::DataMailboxErrorMessage)
        {
            delete pMessage;
            continue;
        }

        MainAutomatonEvent* pEvent = parseMessageToMainAutomatonEvent(pMessage);
        mainAutomaton.processEvent(pEvent);
        
        delete pMessage;
    }

    watchdog_logger << "Program ended. Terminate flag: " + std::to_string(globalTerminateFlag);
}

OWNER MainAutomatonEvent* parseMessageToMainAutomatonEvent(DataMailboxMessage* pMessage)
{
    if (pMessage == nullptr)
    {
        return nullptr;
    }

    if (pMessage->getSource() == refKeypadMailbox)
    {
        return new MainAutomatonEvent(MainAutomaton::enuEvtKeypadMessageReceived, pMessage);
    }

    else if (pMessage->getSource() == refDatabaseMailbox)
    {
        // TODO fix for multiple automatons
        return new MainAutomatonEvent(MainAutomaton::enuEvtKeypadMessageReceived, pMessage);
    }

    return nullptr;
}
