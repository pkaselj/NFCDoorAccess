#ifndef THREAD_LOGGER_SERVER_HPP
#define THREAD_LOGGER_SERVER_HPP

#include"ThreadLoggerClient.hpp"
#include"NulLogger.hpp"

/**
 * @brief Server side class of cross-threaded logger
 * 
 * ThreadLoggerClient acts as a Logger to any class using it, \n
 * but internally it sends all log messages to matching ThreadLoggerServer object, \n
 * via a mailbox. \n
 * ThreadLoggerServer then receives log messages from ThreadLoggerClient and \n
 * forwards them to a Logger which handles them apporpriately. \n
 * \n
 * In essesnce ThreadLoggerClient and ThreadLoggerServer pair \n
 * act as a bridge between a class which logs data \n
 * and a Logger executing concurrently in a separate thread. \n
 * \n
 * When execution speed is critical, then instead of Logger class, \n
 * ThreadLogger class can be implemented. \n
 * \n
 * ThreadLoggerClient acts as a Logger. It takes input the same way Logger does, \n
 * but insted of writing it to a file it sends it to a ThreadLoggerServer via a mailbox. \n
 * \n
 * ThreadLoggerServer is a complementary class of ThreadLoggerClient. It reads input from mailbox \n
 * received from ThreadLoggerClient and writes it to a log file. \n
 * \n
 * \n
 * \n
 * Since ThreadLoggerClient only needs to send log message via mailbox to ThreadLoggerServer which is \n
 * much faster than writing it to a file (which ThreadLoggerServer handles). \n
 */
class ThreadLoggerServer
{
    private:
        /// ThreadLoggerServer mailbox used to receive log messages from ThreadLoggerClient
        Mailbox mailbox;

        std::string m_name;

        /**
         * @brief ILogger pointer used as an endpoint to log messages
         * 
         * ThreadLoggerServer delegates all received messages to \n
         * a logger pointed by p_logger, which then handles the log messages appropriately. \n
         * 
         */
        ILogger* p_logger = nullptr;

        /// stores the boolean state of the logger
        bool available = false;

    public:

        /**
         * @brief Construct a new Thread Logger Server object
         * 
         * @param mailboxId String identifier (name) of server side mailbox of ThreadLogger
         * @param _logger Pointer to a logger which will handle log messages
         */
        ThreadLoggerServer(const std::string& log_file_name, ILogger* _logger = NulLogger::getInstance());
        ~ThreadLoggerServer(void);

        /// ThreadLoggerServer starts receiving messages
        void startLogging(void);

        /// ThreadLoggerServer starts receiving messages and sets unavailable state
        void stopLogging(void) { available = false; };

        /// returns state of the ThreadLoggerServer object
        bool isAvailable(void) { return available; }; 

};
#endif