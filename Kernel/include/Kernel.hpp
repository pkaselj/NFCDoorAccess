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

#ifndef KERNEL_HPP
#define KERNEL_HPP

#include<string>

#include<fcntl.h>
#include<sys/stat.h>

#include"Logger.hpp"
#include"Settings.hpp"

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef FLAG_REGISTER
#define FLAG_REGISTER unsigned char
#endif

#define BIT(X) 1 << (X)

namespace Kernel
{

    namespace IOMode
    {
        /**
        * @brief Specifies enum for IO Access Modes
        * 
        * Every mode includes create (O_CREAT)
        */
        enum IOMode : mode_t {
                            NONE = (mode_t)-1,
                            WRITE = O_WRONLY | O_CREAT,
                            READ = O_RDONLY | O_CREAT,
                            RW = O_RDWR | O_CREAT,
                            READ_NONBLOCKING = READ | O_NONBLOCK
                            };
    }


    namespace Permission
    {
        /// Linux Permissions
        enum Permission : mode_t {
                            NONE = (mode_t)-1,
                            OWNER_XRW = S_IRWXU,
                            OWNER_R = S_IRUSR,
                            OWNER_W = S_IWUSR,
                            OWNER_RW = S_IRUSR | S_IWUSR,
                            OWNER_X = S_IXUSR,
                            GROUP_XRW = S_IRWXG,
                            GROUP_R = S_IRGRP,
                            GROUP_W = S_IWGRP,
                            GROUP_RW = S_IRGRP | S_IWGRP,
                            GROUP_X = S_IXGRP,
                            OTHER_XRW = S_IRWXO,
                            OTHER_R = S_IROTH,
                            OTHER_W = S_IWOTH,
                            OTHER_RW = S_IROTH | S_IWOTH,
                            OTHER_X = S_IXOTH
                            };  
    }

    namespace
    {
        // Logger main_logger(MAIN_ERROR_LOG_PATHNAME); // Private ? CHECK
    }
    
    /// Log `error_message` in kernel.log and exit with exit code `exitCode` which defaults to -1 (255) and process PID
    void Fatal_Error(const std::string& error_message, unsigned char exitCode = -1); 

    /// Log `warning_message` and process PID to kernel.log
    void Warning(const std::string& warning_message);

    /// Log `message` and process PID to kernel.log
    void Trace(const std::string& message);

    /// Dumps raw data pointed to by `pData` with size `sizeOfData` to `filepath`.dump file and logs message to kernel.log
    void DumpRawData(void* pData, size_t sizeOfData, const std::string& filepath);
}

#endif