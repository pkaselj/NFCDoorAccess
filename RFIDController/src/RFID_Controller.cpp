#include "RFID_Controller.hpp"

#include"Kernel.hpp"
#include"Time.hpp"

#include <sstream>
#include <iomanip>

Card::Card()
    :   m_UID("")
{}

Card::Card(const std::string& UID)
    : m_UID(UID)
{}

std::string Card::getUID()
{
    return m_UID;
}

RFID_Controller::RFID_Controller(ILogger* pLogger)
    : m_RFID_module()
{
    m_pLogger = pLogger;
    if(m_pLogger == nullptr)
        m_pLogger = NulLogger::getInstance();

    m_RFID_module.PCD_Init();
    setWaitingTime_ms(-1); // Default waiting time (5 ms)

    *m_pLogger << "MFRC522 successfully initialized!";
}

RFID_Controller::~RFID_Controller()
{
    *m_pLogger << "MFRC522 successfully DEinitialized!";
}

bool RFID_Controller::waitUntilCardIsAvailble()
{
    *m_pLogger << "Waiting for a card to come in range!";

    while(!m_RFID_module.PICC_IsNewCardPresent())
    {
        usleep(m_waitingTime_ms * Time::ms_to_us);
    }

    *m_pLogger << "A card detected!";
    return true;
}

std::string RFID_Controller::getUID_OfAvailableCard()
{
    if(m_RFID_module.PICC_ReadCardSerial() == false)
    {
        *m_pLogger << "Card could not be read!";
        return "";
    }
        

    std::stringstream ssUID;

    for(int i = 0; i < m_RFID_module.uid.size ; i++)
    {
        unsigned int currentByte = m_RFID_module.uid.uidByte[i];

        ssUID << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << currentByte;

        if(i < m_RFID_module.uid.size - 1)
            ssUID << "-";
    }

    std::string UID_string = ssUID.str();

    *m_pLogger << "Card UID successfully read: " + UID_string;
    return UID_string;
}

void RFID_Controller::setWaitingTime_ms(long time_ms)
{
    m_waitingTime_ms = time_ms;
    
    if(time_ms <= 0)
        m_waitingTime_ms = 5;
}