#ifndef PROPERTIESCLASS_H
#define PROPERTIESCLASS_H

#include <QtCore>
#include <QDomDocument>
#include <QDomNode>
#include <QDomElement>

#include <string>

struct Properties
{
    // ---------- Keypad
    unsigned int KEYPAD_BUFFER_SIZE;
    unsigned int KEYPAD_READ_TIMEOUT_MS;

    // ---------- RFID
    unsigned int RFID_BUFFER_SIZE;
    unsigned int RFID_READ_TIMEOUT_MS;
    unsigned int RFID_SAME_CARD_TIMEOUT_MS;

    // ---------- Database
    std::string DB_PATH;

    // ---------- Mailbox
    int QUEUE_SIZE;
    int MAX_MSG_SIZE;
    std::string MAIN_MB_NAME;
    std::string DBGW_MB_NAME;
    unsigned int DATABASE_MB_TIMEOUT;
    std::string DATABASE_LOG_THREAD_MAILBOX_NAME;
    std::string HARDWARED_MB_NAME;

    // ---------- Kernel
    std::string KERNEL_LOG_NAME;
    
    // ---------- Watchdog
    std::string WATCHDOG_SERVER_NAME;
    std::string HARDWARED_WATCHDOG_NAME;
    std::string MAIN_WATCHDOG_NAME;
    std::string DATABASE_WATCHDOG_NAME;
    unsigned int WATCHDOG_SERVER_PERIOD_MS;
    unsigned int HARDWARED_WD_TIMEOUT_MS;
    unsigned int HARDWARED_WD_TTL;
    unsigned int MAIN_WD_TIMEOUT_MS;
    unsigned int MAIN_WD_TTL;
    unsigned int DB_WD_TIMEOUT_MS;
    unsigned int DB_WD_TTL;

    // ----------- Startup
    std::string HARDWARED_EXECUTABLE;
    std::string MAIN_APP_EXECUTABLE;
    std::string DBGW_EXECUTABLE;

    // ----------- Keypad
    std::string KEYPAD_PIPE_NAME;
    std::string KEYPAD_ISTREAM_PATH;

    
    // ----------- Tables
    std::string EMPLOYEES_TABLE_NAME;
    std::string EMPLOYEES_TABLE_ID_COLUMN_NAME;
    std::string EMPLOYEES_TABLE_NAME_COLUMN_NAME;
    std::string EMPLOYEES_TABLE_CLEARANCE_COLUMN_NAME;
    std::string KEYPAD_PASS_TABLE_NAME;
    std::string KEYPAD_PASS_TABLE_ID_COLUMN_NAME;
    std::string KEYPAD_PASS_TABLE_PASSWORD_COLUMN_NAME;
    std::string KEYPAD_PASS_TABLE_OWNER_COLUMN_NAME;
    std::string COMMANDS_TABLE_NAME;
    std::string COMMANDS_TABLE_ID_COLUMN_NAME;
    std::string COMMANDS_TABLE_COMMAND_COLUMN_NAME;
    std::string COMMANDS_TABLE_CLEARANCE_COLUMN_NAME;
    std::string RFID_CARD_TABLE_NAME;
    std::string RFID_CARD_TABLE_ID_COLUMN_NAME;
    std::string RFID_CARD_TABLE_CARD_UUID_COLUMN_NAME;
    std::string RFID_CARD_TABLE_OWNER_COLUMN_NAME;
    std::string LOG_TABLE_NAME;
    std::string LOG_TABLE_ID_COLUMN_NAME;
    std::string LOG_TABLE_TIMESTAMP_COLUMN_NAME;
    std::string LOG_TABLE_USER_ID_COLUMN_NAME;
    std::string LOG_TABLE_AUTH_METHOD_COLUMN_NAME;
    std::string LOG_TABLE_COMMAND_ID_COLUMN_NAME;

    // -------------- Indicators
    std::string INDICATORS_MAILBOX_NAME;
    std::string INDICATORS_MB_SERVER_SUFFIX;
    std::string INDICATORS_MB_CLIENT_SUFFIX;
    unsigned int INDICATORS_MB_TIMEOUT_MS;
    unsigned int BUZZER_BCM_PIN;
    unsigned int DOOR_BCM_PIN;
    unsigned int DOOR_OPEN_TIME_MS;
    unsigned int LCD_I2C_BUS;
    std::string LCD_DEFAULT_IDLE_MESSAGE;
    unsigned int BUZZER_PING_DURATION_MS;
    unsigned int LCD_I2C_ADDRESS;
    unsigned int LCD_DEFULT_MSG_DISPLAY_TIME_MS;
    unsigned int LCD_INTER_COMMANDS_WAIT_TIME_US;

    // std::string LOG_FILE_SUFFIX;

    // --------------- General
    unsigned int DEFAULT_MAX_LOG_FILE_SIZE;
    std::string LOG_FILE_OLD_SUFFIX;
    int MAX_CLEARANCE;
    int FAIL_SAFE_CLEARANCE;
    int NO_CLERANCE;
    std::string MAILBOX_REFRENCE_DEFAULT_NAME;
    unsigned int REQUEST_DEADLINE_TIMER_TIMEOUT_S;

    // std::string SHARED_MEMORY_NAME_SUFFIX;
};

class GlobalProperties
{
public:
    static Properties Get();

private:
    static GlobalProperties* getInstance();
    QDomElement getTag(const QString& path, bool& ok);
    QString getAttribute(const QString& path, bool& ok);
    bool existsTag(const QString& path);
    bool existsAttribute(const QString& path);

    GlobalProperties();
    ~GlobalProperties();
    GlobalProperties(const GlobalProperties&) = delete;
    GlobalProperties(GlobalProperties&&) = delete;

    void init();

    static GlobalProperties* m_pInstance;
    QDomDocument m_XML_document;

    void FatalError(const QString& msg);

    void Trace(const QString& msg);

    QStringList parseNodeName(const QString& rawName);

    QDomElement getParentNodeWithPath(const QStringList& nodePath, bool& ok);
};

#endif // PROPERTIESCLASS_H
