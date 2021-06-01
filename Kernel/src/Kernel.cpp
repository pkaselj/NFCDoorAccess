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
