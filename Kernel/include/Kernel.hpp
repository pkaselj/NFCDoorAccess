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