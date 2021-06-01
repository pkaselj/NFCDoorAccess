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

#include"Logger.hpp"

#include "Time.hpp"

#include <sstream>
#include <fcntl.h>

struct timespec Time::raw_time;
struct tm Time::refined_time;

Logger::Logger(const std::string& path, off_t max_log_file_size_bytes)
    :   m_max_log_file_size_bytes(max_log_file_size_bytes),
    m_OLD_LOG_FILE_NAME(path + ".old")
{
    filepath = path;

    createNewLogFile();
}

Logger::~Logger(void)
{
    *this << "Closing logger";
    close();
}

void Logger::close(void)
{
    output.close();
}

Logger& Logger::logString (std::string const& report)
{
    checkMaxFileSizeOverflow();
    writeToFile(report);

    return *this;
}

std::string Logger::getName() const
{
    return filepath;
}

void Logger::flush()
{
    output.flush();
}

void Logger::checkMaxFileSizeOverflow()
{
    // No constraints on file size
    if (m_max_log_file_size_bytes <= 0)
    {
        return;
    }

    const off_t fileSize_bytes = getFileSize();

    if (fileSize_bytes == -1)
    {
        writeToFile("FATAL ERROR: Could not get the log file size!");
        exit(-1);
    }

    if (fileSize_bytes <= m_max_log_file_size_bytes)
    {
        return;
    }

    deleteOldLogFile();
    closeLogFileAndMarkOld();
    createNewLogFile();

}

void Logger::writeToFile(const std::string& log_message)
{
    std::stringstream logStringBuilder;
    logStringBuilder << Time::getTime() << " --- " << log_message << "\n";

    output << logStringBuilder.str();
    output.flush();
}

off_t Logger::getFileSize() const
{
    struct stat file_info;
    int status = stat(filepath.c_str(), &file_info);
    if (status != 0)
    {
        return -1;
    }

    return file_info.st_size;
}

void Logger::createNewLogFile()
{
    if (output.is_open()) return;

    output.open(filepath.c_str(), std::ios::out | std::ios::ate | std::ios::app);
    if (output.is_open() == false)
        exit(-1);

    output << "Logger " + filepath + " created!" << std::endl;
}

void Logger::closeLogFileAndMarkOld()
{

    this->close();
    int status = rename(filepath.c_str(), m_OLD_LOG_FILE_NAME.c_str());
    int _errno = errno;
    if (status != 0)
    {
        this->open();
        writeToFile("Error while trying to rename log file to .old file. Continuing without renaming... ERRNO: " + std::to_string(_errno));
    }
}

void Logger::deleteOldLogFile()
{
    if (oldLogFileExists())
    {
        int status = remove(m_OLD_LOG_FILE_NAME.c_str());
        int _errno = errno;

        if (status != 0)
        {
            writeToFile("Could not delete old log file. ERRNO: " + std::to_string(_errno));
        }
    }
}

bool Logger::oldLogFileExists() const
{
    return (access(m_OLD_LOG_FILE_NAME.c_str(), F_OK) == 0);
}
