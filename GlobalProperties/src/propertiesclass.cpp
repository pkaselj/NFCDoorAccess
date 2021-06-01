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

#include "propertiesclass.h"

const QString CONFIG_PATH = "config.xml";

GlobalProperties* GlobalProperties::m_pInstance = nullptr;

GlobalProperties::GlobalProperties()
{
    init();
}

void GlobalProperties::init()
{
    QFile XML_file(CONFIG_PATH);
    if(!XML_file.open(QIODevice::ReadOnly))
    {
        FatalError("Error while opening XML file");
    }

    m_XML_document.setContent(&XML_file);
    XML_file.close();
}

GlobalProperties::~GlobalProperties()
{
    delete m_pInstance;
    m_pInstance = nullptr;
}

GlobalProperties* GlobalProperties::getInstance()
{
    if(!m_pInstance) m_pInstance = new GlobalProperties;
    return m_pInstance;
}

QDomElement GlobalProperties::getTag(const QString& path, bool& ok)
{
    QStringList brokenPath = parseNodeName(path);
    QDomElement parentElement = getParentNodeWithPath(brokenPath, ok);

    const QString& targetTag = brokenPath.back();
    QDomElement targetTag_Element = parentElement.firstChildElement(targetTag);
    ok &= !targetTag_Element.isNull();
    return targetTag_Element;
}

QString GlobalProperties::getAttribute(const QString& path, bool& ok)
{
    const QString defValue = QString();
    QStringList brokenPath = parseNodeName(path);
    QDomElement parentElement = getParentNodeWithPath(brokenPath, ok);

    const QString& targetAttr_name = brokenPath.back();
    QString attributeValue = parentElement.attribute(targetAttr_name, defValue);
    ok &= (attributeValue != defValue);
    return attributeValue;
}

Properties GlobalProperties::Get()
{
    GlobalProperties* pXML = GlobalProperties::getInstance();

    Properties prop;
    bool ok = false;

    prop.KEYPAD_BUFFER_SIZE = pXML->getTag("Settings > Keypad > BufferSize", ok).text().toUInt();

    prop.KEYPAD_READ_TIMEOUT_MS = pXML->getTag("Settings > Keypad > ReadTimeout_ms", ok).text().toUInt();

    prop.RFID_BUFFER_SIZE = pXML->getTag("Settings > RFID_Reader > BufferSize", ok).text().toUInt();

    prop.RFID_READ_TIMEOUT_MS = pXML->getTag("Settings > RFID_Reader > ReadTimeout_ms", ok).text().toUInt();

    prop.RFID_SAME_CARD_TIMEOUT_MS = pXML->getTag("Settings > RFID_Reader > SameCardTimeout_ms", ok).text().toUInt();

    prop.DB_PATH = pXML->getTag("Settings > Database > Path", ok).text().toStdString();

    prop.QUEUE_SIZE = pXML->getAttribute("Settings > Mailbox > queue_size", ok).toUInt();

    prop.MAX_MSG_SIZE = pXML->getAttribute("Settings > Mailbox > msg_size", ok).toUInt();

    prop.MAIN_MB_NAME = pXML->getTag("Settings > Mailbox > MainAppMailbox > Name", ok).text().toStdString();

    prop.DBGW_MB_NAME = pXML->getTag("Settings > Mailbox > DatabaseGatewayMailbox > Name", ok).text().toStdString();

    prop.DATABASE_MB_TIMEOUT = pXML->getTag("Settings > Mailbox > DatabaseGatewayMailbox > Timeout_ms", ok).text().toUInt();

    prop.DATABASE_LOG_THREAD_MAILBOX_NAME = pXML->getTag("Settings > Database > LogThread > MailboxName", ok).text().toStdString();

    prop.HARDWARED_MB_NAME = pXML->getTag("Settings > Mailbox > HardwaredMailbox > Name", ok).text().toStdString();

    prop.KERNEL_LOG_NAME = pXML->getTag("Settings > Kernel > LogName", ok).text().toStdString();

    prop.WATCHDOG_SERVER_NAME = pXML->getTag("Settings > Watchdog > Server > Name", ok).text().toStdString();

    prop.HARDWARED_WATCHDOG_NAME = pXML->getTag("Settings > Watchdog > Hardwared > Name", ok).text().toStdString();

    prop.MAIN_WATCHDOG_NAME = pXML->getTag("Settings > Watchdog > Main > Name", ok).text().toStdString();

    prop.DATABASE_WATCHDOG_NAME = pXML->getTag("Settings > Watchdog > Database > Name", ok).text().toStdString();

    prop.WATCHDOG_SERVER_PERIOD_MS = pXML->getTag("Settings > Watchdog > Server > Period_ms", ok).text().toUInt();

    prop.HARDWARED_WD_TIMEOUT_MS = pXML->getTag("Settings > Watchdog > Hardwared > Timeout_ms", ok).text().toUInt();

    prop.HARDWARED_WD_TTL = pXML->getTag("Settings > Watchdog > Hardwared > TTL", ok).text().toUInt();

    prop.MAIN_WD_TIMEOUT_MS = pXML->getTag("Settings > Watchdog > Main > Timeout_ms", ok).text().toUInt();

    prop.MAIN_WD_TTL = pXML->getTag("Settings > Watchdog > Main > TTL", ok).text().toUInt();

    prop.DB_WD_TIMEOUT_MS = pXML->getTag("Settings > Watchdog > Database > Timeout_ms", ok).text().toUInt();

    prop.DB_WD_TTL = pXML->getTag("Settings > Watchdog > Database > TTL", ok).text().toUInt();

    prop.HARDWARED_EXECUTABLE = pXML->getTag("Settings > Startup > Hardwared > Path", ok).text().toStdString();

    prop.MAIN_APP_EXECUTABLE = pXML->getTag("Settings > Startup > MainApp > Path", ok).text().toStdString();

    prop.DBGW_EXECUTABLE = pXML->getTag("Settings > Startup > DatabaseGateway > Path", ok).text().toStdString();

    prop.KEYPAD_PIPE_NAME = pXML->getTag("Settings > Keypad > PipeName", ok).text().toStdString();

    prop.KEYPAD_ISTREAM_PATH = pXML->getTag("Settings > Keypad > IstreamPath", ok).text().toStdString();

    prop.EMPLOYEES_TABLE_NAME = pXML->getAttribute("Settings > Database > Tables > EmployeesTable > name", ok).toStdString();

    prop.EMPLOYEES_TABLE_ID_COLUMN_NAME = pXML->getTag("Settings > Database > Tables > EmployeesTable > ID_ColumnName", ok).text().toStdString();

    prop.EMPLOYEES_TABLE_NAME_COLUMN_NAME = pXML->getTag("Settings > Database > Tables > EmployeesTable > Name_ColumnName", ok).text().toStdString();

    prop.EMPLOYEES_TABLE_CLEARANCE_COLUMN_NAME = pXML->getTag("Settings > Database > Tables > EmployeesTable > Clearance_ColumnName", ok).text().toStdString();

    prop.KEYPAD_PASS_TABLE_NAME = pXML->getAttribute("Settings > Database > Tables > KeypadPassTable > name", ok).toStdString();

    prop.KEYPAD_PASS_TABLE_ID_COLUMN_NAME = pXML->getTag("Settings > Database > Tables > KeypadPassTable > ID_ColumnName", ok).text().toStdString();

    prop.KEYPAD_PASS_TABLE_PASSWORD_COLUMN_NAME = pXML->getTag("Settings > Database > Tables > KeypadPassTable > Password_ColumnName", ok).text().toStdString();

    prop.KEYPAD_PASS_TABLE_OWNER_COLUMN_NAME = pXML->getTag("Settings > Database > Tables > KeypadPassTable > Owner_ColumnName", ok).text().toStdString();

    prop.COMMANDS_TABLE_NAME = pXML->getAttribute("Settings > Database > Tables > CommandsTable > name", ok).toStdString();

    prop.COMMANDS_TABLE_ID_COLUMN_NAME = pXML->getTag("Settings > Database > Tables > CommandsTable > ID_ColumnName", ok).text().toStdString();

    prop.COMMANDS_TABLE_COMMAND_COLUMN_NAME = pXML->getTag("Settings > Database > Tables > CommandsTable > Command_ColumnName", ok).text().toStdString();

    prop.COMMANDS_TABLE_CLEARANCE_COLUMN_NAME = pXML->getTag("Settings > Database > Tables > CommandsTable > Clearance_ColumnName", ok).text().toStdString();

    prop.RFID_CARD_TABLE_NAME = pXML->getAttribute("Settings > Database > Tables > RFID_CardTable > name", ok).toStdString();

    prop.RFID_CARD_TABLE_ID_COLUMN_NAME = pXML->getTag("Settings > Database > Tables > RFID_CardTable > ID_ColumnName", ok).text().toStdString();

    prop.RFID_CARD_TABLE_CARD_UUID_COLUMN_NAME = pXML->getTag("Settings > Database > Tables > RFID_CardTable > CardUUID_ColumnName", ok).text().toStdString();

    prop.RFID_CARD_TABLE_OWNER_COLUMN_NAME = pXML->getTag("Settings > Database > Tables > RFID_CardTable > Owner_ColumnName", ok).text().toStdString();

    prop.LOG_TABLE_NAME = pXML->getAttribute("Settings > Database > Tables > LogTable > name", ok).toStdString();

    prop.LOG_TABLE_ID_COLUMN_NAME = pXML->getTag("Settings > Database > Tables > LogTable > ID_ColumnName", ok).text().toStdString();

    prop.LOG_TABLE_TIMESTAMP_COLUMN_NAME = pXML->getTag("Settings > Database > Tables > LogTable > Timestamp_ColumnName", ok).text().toStdString();

    prop.LOG_TABLE_USER_ID_COLUMN_NAME = pXML->getTag("Settings > Database > Tables > LogTable > UserID_ColumnName", ok).text().toStdString();

    prop.LOG_TABLE_AUTH_METHOD_COLUMN_NAME = pXML->getTag("Settings > Database > Tables > LogTable > AuthMethod_ColumnName", ok).text().toStdString();

    prop.LOG_TABLE_COMMAND_ID_COLUMN_NAME = pXML->getTag("Settings > Database > Tables > LogTable > CommandID_ColumnName", ok).text().toStdString();

    prop.INDICATORS_MAILBOX_NAME = pXML->getTag("Settings > Indicators > Mailbox > Name", ok).text().toStdString();

    prop.INDICATORS_MB_SERVER_SUFFIX = pXML->getTag("Settings > Indicators > Mailbox > ServerSuffix", ok).text().toStdString();

    prop.INDICATORS_MB_CLIENT_SUFFIX = pXML->getTag("Settings > Indicators > Mailbox > ClientSuffix", ok).text().toStdString();

    prop.INDICATORS_MB_TIMEOUT_MS = pXML->getTag("Settings > Indicators > Mailbox > Timeout_ms", ok).text().toUInt();

    prop.BUZZER_BCM_PIN = pXML->getTag("Settings > Indicators > Buzzer > Pin", ok).text().toUInt();

    prop.DOOR_BCM_PIN = pXML->getTag("Settings > Indicators > Door > Pin", ok).text().toUInt();

    prop.DOOR_OPEN_TIME_MS = pXML->getTag("Settings > Indicators > Door > OpenTime_ms", ok).text().toUInt();

    prop.LCD_I2C_BUS = pXML->getTag("Settings > Indicators > LCD > I2C > Bus", ok).text().toUInt();

    prop.LCD_DEFAULT_IDLE_MESSAGE = pXML->getTag("Settings > Indicators > LCD > IdleMessage", ok).text().toStdString();

    prop.BUZZER_PING_DURATION_MS = pXML->getTag("Settings > Indicators > Buzzer > PingDuration_ms", ok).text().toUInt();

    prop.LCD_I2C_ADDRESS = pXML->getTag("Settings > Indicators > LCD > I2C > Address", ok).text().toUInt();

    prop.LCD_DEFULT_MSG_DISPLAY_TIME_MS = pXML->getTag("Settings > Indicators > LCD > MessageLingerTime_ms", ok).text().toUInt();

    prop.LCD_INTER_COMMANDS_WAIT_TIME_US = pXML->getTag("Settings > Indicators > LCD > InterCommandWaitTime_us", ok).text().toUInt();

    prop.DEFAULT_MAX_LOG_FILE_SIZE = pXML->getTag("Settings > General > MaxLogFileSize_MB", ok).text().toUInt();

    prop.LOG_FILE_OLD_SUFFIX = pXML->getTag("Settings > General > LogFileOldSuffix", ok).text().toStdString();

    prop.MAX_CLEARANCE = pXML->getTag("Settings > Clearance > Max", ok).text().toInt();

    prop.FAIL_SAFE_CLEARANCE = pXML->getTag("Settings > Clearance > Default", ok).text().toInt();

    prop.NO_CLERANCE = pXML->getTag("Settings > Clearance > NoPrivileges", ok).text().toInt();

    prop.MAILBOX_REFRENCE_DEFAULT_NAME = pXML->getTag("Settings > General > MailboxRefenreceDefaultName", ok).text().toStdString();

    prop.REQUEST_DEADLINE_TIMER_TIMEOUT_S = pXML->getTag("Settings > General > RequestDeadlineTimerTimeout_s", ok).text().toUInt();


    return prop;
}

void GlobalProperties::FatalError(const QString& msg)
{
    qDebug() << msg;
    exit(-1);
}

void GlobalProperties::Trace(const QString& msg)
{
    qDebug() << msg;
}

QStringList GlobalProperties::parseNodeName(const QString& rawName)
{
    QStringList path;
    path.clear();

    const QString nodeCapturePattern = "([a-zA-Z_]+[a-zA-Z0-9\\-_\\.]*)(\\s*>\\s*)?";
    const QString validateAllPattern = "(([a-zA-Z_]+[a-zA-Z0-9\\-_\\.]*)(\\s*>\\s*))+([a-zA-Z_]+[a-zA-Z0-9\\-_\\.]*)";

    QRegularExpression validateAll(validateAllPattern);

    QRegularExpressionMatch matchAll_result = validateAll.match(rawName);
    if(matchAll_result.hasMatch())
    {
        if(matchAll_result.captured(0) != rawName)
        {
            FatalError("Node path has invalid format!");
            return path;
        }
    }

    QRegularExpression captureNode(nodeCapturePattern);
    QRegularExpressionMatchIterator captureNodeMatches_iter = captureNode.globalMatch(rawName);

    while(captureNodeMatches_iter.hasNext())
    {
        QRegularExpressionMatch match = captureNodeMatches_iter.next();
        path.append(match.captured(1));
    }

    return path;
}

QDomElement GlobalProperties::getParentNodeWithPath(const QStringList& nodePath, bool& ok)
{
    QDomElement currentElement = m_XML_document.firstChildElement();

    ok = false;

    QString debugPath = "[root]/";

    for(int i = 0; i < nodePath.count() - 1; ++i)
    {
        bool contains = false;
        const QString& currentPath = nodePath.at(i);

        for(; !currentElement.isNull(); currentElement = currentElement.nextSiblingElement())
        {
            if(currentElement.tagName() == currentPath)
            {
                contains = true;

                // Skip if current element is one before the targeted element,
                // because the function returns the parent(current) element
                // (Since target element doesn't even have to be element e.g. tag)
                if(i != nodePath.count() - 2)   currentElement = currentElement.firstChildElement();
                break;
            }
        }

        if(!contains)
        {
            FatalError(QString("Element: %1 doesn't contain node: %2").arg(debugPath).arg(currentPath));

            ok = false;
            return QDomElement();
        }

        debugPath.append(currentPath + "/");

    }

    ok = true;
    return currentElement;
}

bool GlobalProperties::existsTag(const QString& path)
{
    bool exists = false;
    // ignore ret value
    getTag(path, exists);
    return exists;
}

bool GlobalProperties::existsAttribute(const QString& path)
{
    bool exists = false;
    // ignore ret value
    getAttribute(path, exists);
    return exists;
}

