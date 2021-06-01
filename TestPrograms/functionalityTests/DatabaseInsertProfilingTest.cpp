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

#include "Database.hpp"
#include "Time.hpp"

#include <iostream>
#include <string>

// Create Test.db sqlite3 database with table "TestTable" containing column Value;

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cout << "Error while parsing input arguments! Usage: " << argv[0] << " INSERT_COUNT" << std::endl;
		return -1;
	}

	int INSERT_COUNT = std::stoi(argv[1]);
	if (INSERT_COUNT < 1)
	{
		std::cout << "Input argument INSERT_COUNT cannot be negative or zero!" << std::endl;
		return -1;
	}

	const std::string DB_NAME = "Test.db";
	int i = 0;

	statement stmtInsert("INSERT INTO TestTable(Value) VALUES (?);");
	Database db(DB_NAME);

	std::cout << "Database INSERT test: INSERTING " << INSERT_COUNT << " records." << std::endl;

	db.NewQuery(stmtInsert);

	std::cout << "Testing INSERT into DB on hard drive. Start time: " << Time::getTime() << std::endl;

	i = 0;
	while (++i <= INSERT_COUNT)
	{
		db.CurrentQuery()->clearBindingsAndReset();
		db.CurrentQuery()->bind(ARGUMENT(0), 1);
		db.CurrentQuery()->next();
	}

	std::cout << "DONE testing DB on hard drive. End time: " << Time::getTime() << std::endl;

	Database db_RAM(":memory:");
	db_RAM.NewQuery(stmtInsert);

	std::cout << "Testing INSERT into DB in RAM. Start time: " << Time::getTime() << std::endl;

	i = 0;
	while (++i <= INSERT_COUNT)
	{
		db.CurrentQuery()->clearBindingsAndReset();
		db.CurrentQuery()->bind(ARGUMENT(0), 1);
		db.CurrentQuery()->next();
	}

	std::cout << "DONE testing DB in RAM. End time: " << Time::getTime() << std::endl;



}