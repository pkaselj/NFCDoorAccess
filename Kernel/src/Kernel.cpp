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

#include"Kernel.hpp"

#include"Logger.hpp"

#include "propertiesclass.h"

#include<iostream>

void Kernel::Fatal_Error(const std::string& error_message, unsigned char exitCode)
{
	static const std::string LOG_PATHNAME = GlobalProperties::Get().KERNEL_LOG_NAME;

	Logger main_logger(LOG_PATHNAME, NO_FILE_SIZE_CONSTRAINT);
    main_logger << std::to_string( getpid() ) + ": FATAL_ERROR: " + error_message;
    main_logger.flush();
    exit(exitCode);
}

void Kernel::Warning(const std::string& warning_message)
{
	static const std::string LOG_PATHNAME = GlobalProperties::Get().KERNEL_LOG_NAME;

	Logger main_logger(LOG_PATHNAME, NO_FILE_SIZE_CONSTRAINT);
    main_logger << std::to_string(getpid()) + ": WARNING: " +  warning_message;
	main_logger.flush();
}

void Kernel::Trace(const std::string& message)
{
	static const std::string LOG_PATHNAME = GlobalProperties::Get().KERNEL_LOG_NAME;

	Logger main_logger(LOG_PATHNAME, NO_FILE_SIZE_CONSTRAINT);
    main_logger << std::to_string(getpid()) + ": " + message;
	main_logger.flush();
}

void Kernel::DumpRawData(void* pData, size_t sizeOfData, const std::string& filepath)
{
	if (pData == nullptr)
	{
		Kernel::Warning("Trying to dump message with no serialized data! ");
		return;
	}

	std::ofstream dump(filepath + ".dump", std::ios::out | std::ios::binary);
	if (!dump)
	{
		Kernel::Warning("Cannot dump message!");
		return;
	}

	dump.write((const char*)pData, sizeOfData);

	Kernel::Trace("Serial data dumped to: " + filepath);

	dump.close();
}
