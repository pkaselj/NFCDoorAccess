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
#include<errno.h>
#include<thread>
#include<cstring>
#include<string>
#include<signal.h>

#include"Settings.hpp"
#include"keypad.hpp"
#include"errorCodes.h"
#include"pipe.hpp"
#include"DataMailbox.hpp"
#include"ThreadLoggerServer.hpp"
#include"ThreadLoggerClient.hpp"
#include"WatchdogClient.hpp"
#include"PN532_NFC.hpp"
#include"InputController.hpp"
#include"IndicatorController.hpp"
#include "propertiesclass.h"

#include<pigpio.h>

const Pinout pins
(
    GlobalProperties::Get().BUZZER_BCM_PIN,
    GlobalProperties::Get().DOOR_BCM_PIN,
    GlobalProperties::Get().DOOR_OPEN_TIME_MS,
    GlobalProperties::Get().LCD_I2C_BUS
);

volatile sig_atomic_t globalTerminateFlag = 0;


void KeypadReaderThreadFunction();
void InputLogicThreadFunction();
void RFID_ReaderThreadFunction();
void IndicatorsThreadFunction();

#define DEBUG(X) std::cout << X << std::endl;

int main(int argc, char** argv)
{

    UNIX_SignalHandler::bindSignalToFlag(UNIX_SignalHandler::enuSIGTERM, &globalTerminateFlag);

    int gpioStatus = gpioInitialise();
    if (gpioStatus < 0)
    {
        Kernel::Fatal_Error("HardwareDaemon - Could not initialize GPIO pins (pigpio)!");
    }

    std::thread inputThread(KeypadReaderThreadFunction);
    std::thread outputThread(InputLogicThreadFunction);
    std::thread RFID_readerThread(RFID_ReaderThreadFunction);
    std::thread indicatorThread(IndicatorsThreadFunction);

    outputThread.join();
    RFID_readerThread.join();
    indicatorThread.join();
    inputThread.join();

    gpioTerminate();

    return 0;
}



void KeypadReaderThreadFunction(void)
{
    const uint BUFFER_SIZE = GlobalProperties::Get().KEYPAD_BUFFER_SIZE;
    const unsigned int keypadReadTimeout_ms = GlobalProperties::Get().KEYPAD_READ_TIMEOUT_MS;

    Logger logger("keypad.driver.log");

    const std::string KEYPAD_PIPE_PATH = GlobalProperties::Get().KEYPAD_PIPE_NAME;
    const std::string KEYPAD_ISTREAM_PATH = GlobalProperties::Get().KEYPAD_ISTREAM_PATH;

    Pipe outputPipe(KEYPAD_PIPE_PATH, Kernel::IOMode::WRITE, BUFFER_SIZE, &logger);
    Keypad keypad(KEYPAD_ISTREAM_PATH, &outputPipe, &logger);

    logger << "Awaiting key press!";

    while(!globalTerminateFlag)
    {
        keypad.readChar();
        usleep(keypadReadTimeout_ms * 1000);
    }
    DEBUG("MARK 1");

}



void InputLogicThreadFunction(void)
{
    Logger watchdogLogger("input.watchdog.log");

    const std::string WATCHDOG_SERVER_NAME = GlobalProperties::Get().WATCHDOG_SERVER_NAME;
    const SlotSettings HARDWARED_WD_SETTINGS =
    {
        .m_BaseTTL = GlobalProperties::Get().HARDWARED_WD_TTL,
        .m_timeout_ms = GlobalProperties::Get().HARDWARED_WD_TIMEOUT_MS
    };

    WatchdogClient watchdog("hardwared.watchdog", WATCHDOG_SERVER_NAME, HARDWARED_WD_SETTINGS, enuActionOnFailure::KILL_ALL , &watchdogLogger );

    Logger keypadLogger("keypad.logic.log");
    Logger RFIDLogger("rfid.logic.log");
    Logger controllerLogger("controller.hardwared.log");
    Logger mailboxLogger("mailbox.log");
    Logger indicatorLogger("indicator.hardwared.client.log");

    const std::string HARDWARED_QUEUE = GlobalProperties::Get().HARDWARED_MB_NAME;
    DataMailbox outputMailbox(HARDWARED_QUEUE, &keypadLogger);

    const std::string MAIN_APP_MESSAGE_QUEUE = GlobalProperties::Get().MAIN_MB_NAME;
    MailboxReference mainAppMailbox(MAIN_APP_MESSAGE_QUEUE);

    const std::string INDICATORS_MAILBOX = GlobalProperties::Get().INDICATORS_MAILBOX_NAME;
    IndicatorController_Client indicators(INDICATORS_MAILBOX, &indicatorLogger);

    const std::string KEYPAD_FIFO_PATH = GlobalProperties::Get().KEYPAD_PIPE_NAME;
    const uint KEYPAD_PIPE_BUFFER_SIZE = GlobalProperties::Get().KEYPAD_BUFFER_SIZE;
    Pipe inputPipeKeypad(KEYPAD_FIFO_PATH, Kernel::IOMode::READ_NONBLOCKING, KEYPAD_PIPE_BUFFER_SIZE, &keypadLogger);
    const uint RFID_PIPR_BUFFER_SIZE = GlobalProperties::Get().RFID_BUFFER_SIZE;
    Pipe inputPipeRFID("rfid.pipe", Kernel::IOMode::READ_NONBLOCKING, RFID_PIPR_BUFFER_SIZE, &RFIDLogger); // TODO add constant

    InputController controller(&inputPipeKeypad,
        &inputPipeRFID,
        &outputMailbox,
        &mainAppMailbox,
        &indicators ,
        &controllerLogger);

    watchdog.Start();
    while (!globalTerminateFlag && watchdog.Kick())
    {
        controller.ProcessInput();
    }

    watchdogLogger << "Program ended. Terminate flag: " + std::to_string(globalTerminateFlag);

}


void RFID_ReaderThreadFunction()
{

    
    const uint BUFFER_SIZE = GlobalProperties::Get().RFID_BUFFER_SIZE;

    Logger logger("rfid.driver.log");
    Pipe outPipe("rfid.pipe", Kernel::IOMode::WRITE, BUFFER_SIZE, &logger);
    PN532_NFC card_reader(&logger);

    const unsigned int timeout_ms = GlobalProperties::Get().RFID_READ_TIMEOUT_MS;
    const unsigned int sameCardTimeout_ms = GlobalProperties::Get().RFID_SAME_CARD_TIMEOUT_MS;
    const std::string sameCardTimerName = "same_card_timer";

    Timer sameCardTimer(sameCardTimerName);
    sameCardTimer.setTimeout_ms(sameCardTimeout_ms);

    std::string lastCardUID = "";
    while(!globalTerminateFlag)
    {
        usleep(timeout_ms * Time::ms_to_us);
        std::string cardUID = card_reader.readCardUUID();
        if (cardUID == "")
        {
            continue;
        }
        else if (cardUID == lastCardUID)
        {
            if (sameCardTimer.getTimerStatus() != Timer::Expired)
            {
                logger << "Detected card same as last one! Same card timer did not expire - ignoring card!";
                continue;
            }
        }

        // DEBUG
        // std::cout << "Keypad read" << std::endl;
        // DEBUG

        outPipe.send(cardUID);

        lastCardUID = cardUID;
        sameCardTimer.Reset();

        // usleep(timeout_ms * Time::ms_to_us);
    }
    DEBUG("MARK 3");
}

void IndicatorsThreadFunction()
{
    const unsigned int indicatorServerTimeout_ms = GlobalProperties::Get().INDICATORS_MB_TIMEOUT_MS;

    Logger logger("indicator.server.log");

    const std::string INDICATORS_MAILBOX = GlobalProperties::Get().INDICATORS_MAILBOX_NAME;
    IndicatorController_Server indicators(INDICATORS_MAILBOX, pins, &logger);

    while (!globalTerminateFlag)
    {
        indicators.ListenAndParseRequest(indicatorServerTimeout_ms);
    }
    DEBUG("MARK 4");

}

