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

#ifndef LOGGER_TO_STDOUT_HPP
#define LOGGER_TO_STDOUT_HPP

#include "ILogger.hpp"

#include "Time.hpp"

#include <iostream>

/**
 * @brief Logger class which writes all log messages to standard output
 *
 */
class LoggerToStdout : public ILogger
{
private:
    std::string m_name;

protected:
    virtual ILogger& logString(std::string const& report)
    {
        std::cout << Time::getTime() << "-- " << report << std::endl;
        return *this;
    }

public:
    LoggerToStdout(const std::string& name) : m_name(name) {}; // TODO check if name is empty (or not)
    virtual ~LoggerToStdout(void) {};

    virtual std::string getName() const { return m_name; };
};
#endif