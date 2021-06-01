#include <iostream>

#include "Tables.hpp"
#include "Time.hpp"

int main()
{

	Logger logger("TablesTest.log");
	Database database("/home/pi/NFCDoorAccess_src/DatabaseGateway/res/Database_11032021.db", &logger);

	LogTable logTable(&database, &logger);

	logTable.initialize();

	LogEntry logEntry = 
	{
		.m_timestamp = Time::getDateTime_ISO8601(),
		.m_userId = 2,
		.m_authMethod = "PIN",
		.m_commandId = 1
	};

	logTable.CreateLog(logEntry);

	auto results = logTable.SelectLogs();
	for (auto& entry : results)
	{
		std::cout << entry.m_timestamp << " "
			<< entry.m_userId << " "
			<< entry.m_authMethod << " "
			<< entry.m_commandId << std::endl;
	}

	return 0;
}