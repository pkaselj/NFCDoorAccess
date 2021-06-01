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

#include "RFID_Controller.hpp"
#include "Logger.hpp"

#include <iostream>

int main()
{
    Logger logger("RFID_read_UID.log");
    RFID_Controller card_reader(&logger);

    while(true)
    {
        bool success = card_reader.waitUntilCardIsAvailble();
        if(success == false)
            continue;

        std::string newUID = card_reader.getUID_OfAvailableCard();
        std::cout << "New UID read: " << newUID << std::endl;
    }

    return 0;
}