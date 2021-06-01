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