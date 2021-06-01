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

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include"ILogger.hpp"

#include<mutex>

const off_t SIZE_10MB = 10E6;
const off_t DEFAULT_MAX_LOG_FILE_SIZE_BYTES = SIZE_10MB;
const off_t NO_FILE_SIZE_CONSTRAINT = -1;

/**
 * @brief Class used for logging messages to a file
 * 
 * Derives from ILogger interface
 */
class Logger : public ILogger
{
    private:

        /// Close log file
        void close  (void);

        /// Open log file
        void open(void) { createNewLogFile(); }

        /// Returns `true` if log file named " `filepath`.old " exists in local directory
        bool oldLogFileExists() const;

        /**
         * @brief Checks if log file size is greater than max file size defined in ctor
         * If the file size is greater than the defined limit, it closes `filepath` file,
         * renames it to `filepath.old` (deleting any existing `filepath.old` files), and
         * opens new `filepath file for writing`
        */
        void checkMaxFileSizeOverflow();

        void createNewLogFile();
        void closeLogFileAndMarkOld();
        void deleteOldLogFile();

        void writeToFile(const std::string& log_message);

        const std::string m_OLD_LOG_FILE_NAME;


    protected:

        /// Mutex to resolve concurrent writing to a file
        std::mutex write_lock;

        /// Path to log file
        std::string              filepath;

        /// Output stream object to write to `filepath`
        std::ofstream            output;

        off_t m_max_log_file_size_bytes;

        /// Open log file for writing ONLY if it exists
        void openIfExists(void);

        /// Called by operator<<, used for polymorphism (class NulLogger)
        virtual Logger& logString (std::string const& report);
                        /// Create default Logger object
                        Logger    (void) = default;

    public:

        typedef enum
        {
            OVERWRITE = 0,
            APPEND
        } enuOptions;
            /**
             * @brief Construct a new Logger object
             * 
             * @param path File path to a log file. Creates one if it doesn't exist, else opens at the end of file for appending
             * @param max_log_file_size_bytes Max log file size. Default value: `DEFAULT_MAX_LOG_FILE_SIZE_BYTES`, pass `NO_FILE_SIZE_CONSTRAINT` to ignore file size constraints
             * 
             * `max_log_file_size_bytes` -  Max log file size before log file \n
             * is closed and renamed to `filepath.old`, while new `filepath` \n
             * log file is opened. \n
             * Default value is `DEFAULT_MAX_LOG_FILE_SIZE_BYTES`. \n
             * Pass `NO_FILE_SIZE_CONSTRAINT` or `max_log_file_size_bytes <= 0` to ignore file size constraints.
             * 
             * \see checkMaxFileSizeOverflow(), DEFAULT_MAX_LOG_FILE_SIZE_BYTES, NO_FILE_SIZE_CONSTRAINT
             */
             Logger (const std::string& path, off_t max_log_file_size_bytes = DEFAULT_MAX_LOG_FILE_SIZE_BYTES);

             /// Returns the name (path of the output log file) of the logger
             virtual std::string getName() const;

             off_t getFileSize() const;

             void flush();

             /// Used to crash process in case of major error. (To be removed)
        //void Crash  ();
    virtual ~Logger (void);

    /*  
    *   template <class T>
    *   friend Logger& operator <<(Logger& logStream, T const& report);
    *    
    *   Commented method allows for streaming objects to log,
    *   currently implemented one supports only strings.
    * 
    *   This was done because of child class NulLogger
    *   which is used when a class which uses Logger class
    *   doesn't want to log anything.
    * 
    *   
    * 
    */

};
#endif