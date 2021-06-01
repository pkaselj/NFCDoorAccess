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

#ifndef NULLOGGER_HPP
#define NULLOGGER_HPP

#include"ILogger.hpp"

/**
 * @brief NulLogger class - Singleton
 * 
 * Used as a default logger in other classes. \n
 * \n
 * Does nothing with the input. Used to implement logger as an optional parameter. \n
 * \n
 * example:
 *      ILogger* pLogger = NulLogger::getInstance();
 *      pLogger << "Do nothing!";
 */
class NulLogger : public ILogger
{
    protected:
    virtual NulLogger& logString (std::string const& report);

    private:
    NulLogger(){};

    public:
    static NulLogger* getInstance();

    virtual std::string getName() const;

    virtual ~NulLogger();

    NulLogger(NulLogger&) = delete;
    void operator= (NulLogger&) = delete;
};

#endif