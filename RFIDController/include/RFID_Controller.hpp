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

#include"NulLogger.hpp"

#include"MFRC522.h"

class Card
{
    private:
    std::string m_UID;

    public:
    Card();
    Card(const std::string& UID);
    ~Card(){}

    std::string getUID();

};

class RFID_Controller
{
    private:
    MFRC522 m_RFID_module;
    ILogger* m_pLogger;

    long m_waitingTime_ms;

    public:
    RFID_Controller(ILogger* pLogger = nullptr);
    ~RFID_Controller();

    void setWaitingTime_ms(long time_ms);

    bool waitUntilCardIsAvailble();
    std::string getUID_OfAvailableCard();
};