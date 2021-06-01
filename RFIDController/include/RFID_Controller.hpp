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