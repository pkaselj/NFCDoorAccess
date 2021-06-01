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

#ifndef SHARED_MEMORY_HPP
#define SHARED_MEMORY_HPP

// #include"ILogger.hpp"
#include"NulLogger.hpp"
#include"Kernel.hpp"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include<string>

/**
 * @brief Shared memory wrapper class
 * 
 * @tparam T type T used as the smallest unit of data in shared memory
 */
template<class T>
class SharedMemory
{
    private:
    ILogger* p_logger = nullptr;

    /// Pointer to the base address of the shared memory block
    T* baseAddressPointer = nullptr;

    /**
     * @brief Shared memory identifier
     * Used for implementing POSIX shared memory
     */
    std::string name = "";

    /**
     * @brief File descriptor of shared memory
     * Used for implementing POSIX shared memory
     */
    int fd = -1;

    /// Size of shared memory block in bytes
    size_t size = 0;

    /**
     * @brief Set the name field of the shared memory object
     * Sets and checks the name field; \n
     * \n
     * By POSIX standards, all shared memory objects \n
     * must start with and contain a single forward-slash '/'. \n
     * \n
     * setName() functions deletes all forward-slashes in the name, \n
     * except the one at index 0. \n
     * If there is no forward-slash at index 0, then it adds one. \n
     * @param identifier 
     */
    void setName(const std::string& identifier);

    /// Opens the shared memory
    void open();

    
    ///Maps the shared memory to the process memory
    void map();

    /**
     * @brief Truncates the shared memory
     * Truncates the shared memory to SharedMemory::size bytes or, \n
     * if the SharedMemory::size is equal to 0, \n
     * checks the size of the specified shared memory in the filesystem. \n
     * \n
     * Fails if size is still 0! \n
     */
    void truncate();

    public:
    //SharedMemory(const std::string& _name, unsigned int size = 0);

    /**
     * @brief Construct a new Shared Memory object
     * 
     * @param _name String identifier (name) of shared memory (POSIX standard)
     * @param length Length (number of `sizeof(T)` chunks) of shared memory. If set to 0 (default) then it will try to read the size from the filesystem
     * @param _p_logger Pointer to a parent ILogger which is used as a logger. - NULL SAFE
     */
    SharedMemory(const std::string& _name, unsigned int length = 0, ILogger* _p_logger = NulLogger::getInstance());

    /**
     * @brief Destroy the Shared Memory object
     * 
     */
    ~SharedMemory();

    /**
     * @brief Write data to the shared memory.
     * 
     * @param offset Index at which data will be written
     * @param data Data to be written to the shared memory block
     */
    void write(unsigned int offset, const T& data);

    /**
     * @brief Read data from the shared memory.
     * 
     * @param offset Index from which data will be read.
     * @return const T& returns a copy of object `T` at index `offset` of the shared memory 
     */
    T read(unsigned int offset);

    /**
     * @brief Get reference to the object T at the index `index`
     * @param index 
     * @return Reference to the object T at the index `index`
    */
    T& operator[](unsigned int index);

    /**
     * @brief Get the size of the shared memory
     * 
     * @return size_t Size of the shared memory in bytes
     */
    size_t getSize() const;

    /**
     * @brief Get the length of the shared memory
     * 
     * @return size_t Length of the shared memory in number of `sizeof(T)` chunks
     */
    size_t getLength() const;
};

template<class T>
void SharedMemory<T>::setName(const std::string& identifier)
{
    name = identifier;

    /* erase all forward slashes in name */
    size_t position = name.find('/', 0);
    while(position >= 0 && position < name.length())
    {
        name.erase(position, 1);
        position = name.find('/', 0);
    }
        
    return;
}


/*template<class T>
SharedMemory<T>::SharedMemory(const std::string& _name, unsigned int size = 0)
{
    CreateNulLogger();
    setName(_name);

    int status = shm_open(name.c_str(),
                          O_CREAT | O_RDWR,
                          S_IRUSR | S_IWUSR);

    if(status < 0)
    {
        *p_logger << "Cannot open and/or create a shared memory object named " + name;
        p_logger->Crash();
    }


}*/

template<class T>
SharedMemory<T>::SharedMemory(const std::string& _name, unsigned int length, ILogger* _p_logger)
{
    size = length * sizeof(T);

    p_logger = _p_logger;
    if(p_logger == nullptr)
        p_logger = NulLogger::getInstance();

    setName(_name);

    open();
    truncate();
    map();

}

template<class T>
void SharedMemory<T>::open()
{
    fd = shm_open(name.c_str(),
                  O_CREAT | O_RDWR,
                  S_IRUSR | S_IWUSR);

    if(fd < 0)
    {
        *p_logger << "Cannot open and/or create a shared memory object named " + name;
        Kernel::Fatal_Error("Cannot open and/or create a shared memory object named " + name);
    }
    *p_logger << name + " - Shared memory object successfully opened!";
}

template<class T>
void SharedMemory<T>::map()
{
    if(size == 0)
    {
        *p_logger << name + " - Error! Size of shared memory cannot be 0!";
        Kernel::Fatal_Error(name + " - Error! Size of shared memory cannot be 0!");
    }

    if(fd < 0)
    {
        *p_logger << name + " - Error! Invalid shared memory file descriptor!";
        Kernel::Fatal_Error(name + " - Error! Invalid shared memory file descriptor!");
    }

    void* uncastBasePointer = mmap(nullptr,
                                   size,
                                   PROT_READ | PROT_WRITE,
                                   MAP_SHARED,
                                   fd,
                                   0);
    int _errno = errno;
                                
    if(uncastBasePointer == nullptr)
    {
        *p_logger << name + " - cannot map shared memory! Errno: " + std::to_string(_errno);
        Kernel::Fatal_Error(name + " - cannot map shared memory! Errno: " + std::to_string(_errno));
    }

    baseAddressPointer = static_cast<T*> (uncastBasePointer);
    *p_logger << "Shared memory object successfully mapped!";
}

template<class T>
SharedMemory<T>::~SharedMemory()
{
    if(baseAddressPointer == nullptr)
    {
        *p_logger << name + " - memory leak! Cannot find base pointer to shared memory!";
        Kernel::Warning(name + " - memory leak! Cannot find base pointer to shared memory!");
    }

    if(size == 0)
    {
        *p_logger << name + " - memory leak! Size of shared memory cannot be zero!";
        Kernel::Warning(name + " - memory leak! Size of shared memory cannot be zero!");
    }

    int status = munmap(static_cast<void*>(baseAddressPointer), size);
    int _errno = errno;
    if(status < 0)
    {
        *p_logger << name + " - cannot unmap shared memory! Errno: " + std::to_string(_errno);
        Kernel::Fatal_Error(name + " - cannot unmap shared memory! Errno: " + std::to_string(_errno));
    }
    *p_logger << name + " - shared memory unmapped successfully!";

    status = shm_unlink(name.c_str());
    _errno = errno;
    if(status < 0)
    {
        *p_logger << name + " - Cannot unlink shared memory! Errno: " + std::to_string(_errno);
        //p_logger->Crash(); ERRNO 2 --> already closed
    }
    *p_logger << name + " - Shared memory unlinked successfully!";
}

template<class T>
void SharedMemory<T>::write(unsigned int offset, const T& data)
{
    if(offset > size / sizeof(T) - 1)
    {
        *p_logger << name + " - Shared memory write attempt - out of bounds: " + std::to_string(offset) + " of " + std::to_string(size / sizeof(T));
        Kernel::Warning(name + " - Shared memory write attempt - out of bounds: " + std::to_string(offset) + " of " + std::to_string(size / sizeof(T)));
        return;
    }

    if(baseAddressPointer == nullptr)
    {
        *p_logger << name + " - Pointer to the start of the shared memory segment cannot be null";
        Kernel::Fatal_Error(name + " - Pointer to the start of the shared memory segment cannot be null");
    }

    *p_logger << name + " - Shared memory write operation at offset " + std::to_string(offset);
    *(baseAddressPointer + offset) = data;

}

template<class T>
T SharedMemory<T>::read(unsigned int offset)
{
    if(offset > size / sizeof(T) - 1)
    {
        *p_logger << name + " - shared memory read attempt - out of bounds: " + std::to_string(offset) + " of " + std::to_string(size / sizeof(T));
        Kernel::Fatal_Error(name + " - shared memory read attempt - out of bounds: " + std::to_string(offset) + " of " + std::to_string(size / sizeof(T)));
    }

    if(baseAddressPointer == nullptr)
    {
        *p_logger << name + " - pointer to the start of the shared memory segment cannot be null";
        Kernel::Fatal_Error(name + " - pointer to the start of the shared memory segment cannot be null");
    }

    *p_logger << name + " - Shared memory read operation at offset " + std::to_string(offset);
    return *(baseAddressPointer + offset);
}

template<class T>
T& SharedMemory<T>::operator[](unsigned int index)
{
    if (index >= getLength())
    {
        *p_logger << name + " - shared memory operator[] with index: " + std::to_string(index) + " - out of bounds. Shared memory length: " + std::to_string(getLength());
        Kernel::Fatal_Error( name + " - shared memory operator[] with index: " + std::to_string(index) + " - out of bounds. Shared memory length: " + std::to_string(getLength()) );
    }
        
    *p_logger << "Memory access at location: " + std::to_string(index);
    return *(baseAddressPointer + index);
}

template<class T>
void SharedMemory<T>::truncate()
{
    *p_logger << name + " - Attempting to truncate shared memory size...";

    if(fd < 0)
    {
        *p_logger << name + " - Cannot truncate shared memory, file descriptor invalid!";
        Kernel::Fatal_Error(name + " - Cannot truncate shared memory, file descriptor invalid!");
    }
    
    if(size == 0)
    {
        struct stat status;
        
        if(fstat(fd, &status) < 0)
        {
            int _errno = errno;
            *p_logger << name + " - Cannot get stats about shared memory. Errno: " + std::to_string(_errno);
            Kernel::Fatal_Error(name + " - Cannot get stats about shared memory. Errno: " + std::to_string(_errno));
        }

        size = status.st_size;

        if(size == 0)
        {
            *p_logger << name + " - Shared memory object size is 0. Cannot open file with no size!";
            Kernel::Fatal_Error(name + " - Shared memory object size is 0. Cannot open file with no size!");
        }   
    }

    int status = ftruncate(fd, size);
    int _errno = errno;

    if(status < 0)
    {
        *p_logger << name + " - Cannot truncate shared memory. Errno: " + std::to_string(_errno);
        Kernel::Fatal_Error(name + " - Cannot truncate shared memory. Errno: " + std::to_string(_errno));
    }
    *p_logger << name + " - Shared memory object successfully truncated!";
}

template<class T>
size_t SharedMemory<T>::getSize() const
{
    return size;
}

template<class T>
size_t SharedMemory<T>::getLength() const
{
    return size / sizeof(T);
}

#endif