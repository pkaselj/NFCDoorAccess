#ifndef PIPE_H
#define PIPE_H

#include"Kernel.hpp"
#include"NulLogger.hpp"
#include"errorCodes.h"
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

/*extern mqd_t statusQueueId;
extern char* process_name;*/

typedef struct mq_attr mq_attr;
/*
           struct mq_attr {
               long mq_flags;        Flags (ignored for mq_open()) 
               long mq_maxmsg;       Max. # of messages on queue 
               long mq_msgsize;      Max. message size (bytes) 
               long mq_curmsgs;      # of messages currently in queue
                                       (ignored for mq_open()) 
           };
*/



/**
 * @brief Pipe wrapper class
 * 
 */
class Pipe
{
private:
    const        uint    msg_buffer_size;
                 bool    availability    = false;
                 int     fd              = -1;
                 Kernel::IOMode::IOMode  openMode = Kernel::IOMode::NONE;
                 ILogger* p_parentLogger = NULL;
            std::string  pathname        = "";




public:
        /**
         * @brief Construct a new Pipe object with logger
         * 
         * @param path FIFO path. Created if it doesn't exist.
         * @param mode Pipe access mode {Kernel::IOMode::READ, Kernel::IOMode::WRITE, Kernel::IOMode::READ_NONBLOCKING}
         * @param p_logger pointer to a logger
         */
         Pipe           (const std::string& path, Kernel::IOMode::IOMode mode, const unsigned int bufferSize, ILogger* p_logger = NulLogger::getInstance());

         /**
          * @brief Destroy the Pipe object
          * 
          */
         ~Pipe          (void);

    /// returns TRUE if pipe is available for IO operations, else FALSE
    bool isAvailable    (void);

    /// sets Pipe unavailable for IO operations
    void setUnavailable (void);

    /// sets Pipe available for IO operations
    void setAvailable   (void);

    /**
     * @brief Send message through pipe.
     * 
     * @param message c-style string to send
     * @param count number of characters in `message`
     */
    void send           (const char* message, int count);

    /**
     * @brief Send message through pipe
     * 
     * @param message String message content
     */
    void send           (std::string const& message);


    /**
     * @brief Block until message is received
     * 
     * @return std::string Received message
     */
    std::string receive (void);

    /// Returns the file descriptor of the pipe
    int getFd() const { return fd; }


    // void setNonblockingPermanent();
    // void setBlockingPermanent();
};

#endif