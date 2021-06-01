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

#ifndef ILOGGER_HPP
#define ILOGGER_HPP

#include<iostream>
#include<fstream>
#include<cstdio>
#include<unistd.h>
#include<cstdlib>
#include<sys/stat.h>
#include<sys/time.h>

/**
 * @brief Logger interface class
 * 
 */
class ILogger
{
    protected:
    /**
     * @brief Main logger function.
     * 
     * Function which get called whenever \n
     * << operator is used on a child object. \n
     * \n
     * Main function is to support polymorphism \n
     * since friend functions cannot be overridden. \n
     * 
     * @param report Message to be logged.
     * @return ILogger& returns ILogger reference to support stacking
     */
    virtual ILogger& logString (std::string const& report) = 0;

    

    public:
    //ILogger(void) = delete;
    virtual ~ILogger(void) {};

    /**
     * @brief Get the Name og the ILogger object
     * 
     * @return const std::string& name
     */
    virtual std::string getName() const = 0;

    /**
     * @brief << operator overload
     * 
     * @param logStream ILogger reference 
     * @param report Message to be written
     * @return ILogger& returns ILogger reference to support stacking
     */
    friend ILogger& operator <<(ILogger& logStream, std::string const& report);
};

inline ILogger& operator <<(ILogger& logStream, std::string const& report)
{
    return logStream.logString(report);
}

#endif