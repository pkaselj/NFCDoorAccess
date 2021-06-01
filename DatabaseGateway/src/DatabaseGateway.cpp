#include<iostream>
#include<iomanip>
#include<cstring>
#include<thread>

#include "DatabaseRequest.hpp"
#include "UNIX_SignalHandler.hpp"
#include "WatchdogClient.hpp"
#include "propertiesclass.h"


volatile sig_atomic_t globalTerminateFlag = 0;

void databaseLoggerThreadFunction(DatabaseResources& resources);

int main()
{

    Logger wd_logger("database.watchdog.log");

    UNIX_SignalHandler::bindSignalToFlag(UNIX_SignalHandler::enuSIGTERM, &globalTerminateFlag);

    const std::string DATABASE_WATCHDOG = GlobalProperties::Get().DATABASE_WATCHDOG_NAME;
    const unsigned int DATABASE_MAILBOX_TIMEOUT_MS = GlobalProperties::Get().DATABASE_MB_TIMEOUT;
    const std::string DATABASE_LOG_THREAD_MB = GlobalProperties::Get().DATABASE_LOG_THREAD_MAILBOX_NAME;
    const std::string WATCHDOG_SERVER_NAME = GlobalProperties::Get().WATCHDOG_SERVER_NAME;
    const SlotSettings DATABASE_WATCHDOG_SETTINGS =
    {
        .m_BaseTTL = GlobalProperties::Get().DB_WD_TTL,
        .m_timeout_ms = GlobalProperties::Get().DB_WD_TIMEOUT_MS
    };

    WatchdogClient watchdog(DATABASE_WATCHDOG, WATCHDOG_SERVER_NAME, DATABASE_WATCHDOG_SETTINGS, enuActionOnFailure::KILL_ALL, &wd_logger);

    Logger mb_logger("database.mailbox.log");
    Logger db_logger("database.databaseObj.log");

    const std::string DATABASE_GATEWAY_MAILBOX_NAME = GlobalProperties::Get().DBGW_MB_NAME;
    DataMailbox mailbox(DATABASE_GATEWAY_MAILBOX_NAME, &mb_logger);
    mailbox.setRTO_ns(DATABASE_MAILBOX_TIMEOUT_MS * Time::ms_to_ns);
    // MailboxReference router(ROUTER_TO_DBG_QUEUE); // REMOVE

    SimplifiedMailbox logMailbox(DATABASE_LOG_THREAD_MB + ".client");
    MailboxReference refLogThread(DATABASE_LOG_THREAD_MB + ".server");

    DatabaseResources resources =
    {
        .m_pDatabaseObject = nullptr, // not yet created
        .m_pMailbox = &mailbox,
        .m_pLogMailbox = &logMailbox,
        .m_refLogThread = refLogThread
    };

    const std::string DATABASE_PATH = GlobalProperties::Get().DB_PATH;
    DatabaseObject database(DATABASE_PATH, resources, &db_logger);

    resources.m_pDatabaseObject = &database; // created here, required for DatabaseRequestFactory

    DatabaseRequestFactory requestFactory(resources, &db_logger);

    std::thread databaseLoggerThread(databaseLoggerThreadFunction, std::ref(resources));

    watchdog.Start();
    while (!globalTerminateFlag && watchdog.Kick())
    {
        DataMailboxMessage* pReceivedMessage = mailbox.receive(enuReceiveOptions::TIMED);

        if (pReceivedMessage->getDataType() != MessageDataType::enuType::CommandMessage)
        {
            delete pReceivedMessage;
            continue;

        }

        CommandMessage* pReceivedRequestMessage = dynamic_cast<CommandMessage*>(pReceivedMessage);
        IDatabaseRequest* pRequest = requestFactory.createRequestObjectFrom(&pReceivedRequestMessage);
        
        pRequest->Process();
       
        delete pRequest;

    }

    wd_logger << "Program ended. Terminate flag: " + std::to_string(globalTerminateFlag);

    databaseLoggerThread.join();

    return 0;
}

void databaseLoggerThreadFunction(DatabaseResources& resources)
{
    if (resources.m_pDatabaseObject == nullptr)
    {
        Kernel::Fatal_Error("DatabaseLoggerThread - pDatabaseObject is null!");
        return;
    }

    const std::string DATABASE_LOG_THREAD_MB = GlobalProperties::Get().DATABASE_LOG_THREAD_MAILBOX_NAME;

    SimplifiedMailbox mailbox(DATABASE_LOG_THREAD_MB + ".server");

    const unsigned int mailbox_timeout_ms = 100;
    const timespec timeout_settings = Time::getTimespecFrom_ms(mailbox_timeout_ms);

    mailbox.setTimeout_settings(timeout_settings);

    while (!globalTerminateFlag)
    {
        SimpleMailboxMessage message = mailbox.receive(enuReceiveOptions::TIMED);

        if (message.isTimedOut() || message.isSyscallInterrupted())
        {
            continue;
        }

        // DataSizePair temp = message.getOwnershipOfData();
        // char* pRawData = temp.m_pData;
        // temp.Clear(); // Safety TODO

        char* pRawData = message.m_pData;

        /********************************************************************************************************************************************************
        *   pRawData points to buffer which contains logEntry address on heap => char* pRawData = & ( &logEntry )
        *                                                                       pointer to buffer ^   ^ buffer contents
        *   
        *   so => LogEntry logEntry = * (*pRawData);
        *     dereference the address ^  ^ contents of buffer which contains logEntry address on heap   
        *
        *
        ********************************************************************************************************************************************************/

        LogEntry** pToBufferContainingLogEntryAddress = (LogEntry**) pRawData;
        LogEntry* pLogEntry = *(pToBufferContainingLogEntryAddress);

        if (pLogEntry == nullptr)
        {
            Kernel::Warning("DatabaseGateway received log data is null!");
            delete pLogEntry;
            continue;
        }

        resources.m_pDatabaseObject->WriteLogToLogTable(*pLogEntry);
        delete pLogEntry;
    }
}
