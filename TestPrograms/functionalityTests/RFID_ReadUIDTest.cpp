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