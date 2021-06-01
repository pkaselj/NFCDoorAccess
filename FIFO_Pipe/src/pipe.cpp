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

#include"pipe.hpp"

#include<unistd.h>
#include<sys/ipc.h>
#include<mqueue.h>
#include<string.h>
#include<errno.h>
#include<string>
#include<sys/stat.h>
#include<fcntl.h>

#include"propertiesclass.h"


Pipe::Pipe(const std::string& _path, Kernel::IOMode::IOMode _mode, const unsigned int bufferSize, ILogger* p_logger)
    :   openMode(_mode),
    msg_buffer_size(bufferSize),
    pathname(_path)
{
    if(p_logger == nullptr)
        p_parentLogger = NulLogger::getInstance();
    else
        p_parentLogger = p_logger;

    struct stat fileStatus;
    if(stat(pathname.c_str(), &fileStatus) < 0)
    {
        *p_parentLogger << "Attempting to create a FIFO";

        fd = mkfifo(pathname.c_str(), Kernel::Permission::OWNER_RW);
        int _errno = errno;

        *p_parentLogger << "Error in creating a FIFO... resolving";
    }

    *p_parentLogger << "FIFO exists. Trying to open ...";
    fd = open(pathname.c_str(), openMode);
    
    if(fd < 0)
    {
        *p_parentLogger << "Cannot open Pipe with path: " + pathname;
        Kernel::Fatal_Error("Cannot open Pipe with path: " + pathname);
    }

    availability = true;
    *p_parentLogger << "Successfully opened pipe - " + pathname;

}


Pipe::~Pipe(void)
{
    if(fd >= 0)
        close(fd);
    else
        Kernel::Warning(pathname + "- pipe file descriptor not valid (for closing)");

    *p_parentLogger << "Pipe closed";
}

bool Pipe::isAvailable(void) {return availability;};
void Pipe::setUnavailable(void) {availability = false;}
void Pipe::setAvailable(void) {availability = true;}

void Pipe::send(const char* message, int count)
{
    //write(fd, message, sizeof(message)); // -CHECK for allocated strings
    
    ssize_t size = write(fd, message, count);
    if(size < 0)
    {
        Kernel::Fatal_Error("Cannot write to a pipe " + pathname);
    }

    *p_parentLogger << pathname +  "[" + std::to_string(size) +  " B] Pipe output: " + std::string(message);
}

void Pipe::send(std::string const& message)
{
    send(message.c_str(), message.size());
}

std::string Pipe::receive(void)
{
    if(fd < 0 || (openMode != Kernel::IOMode::READ && openMode != Kernel::IOMode::READ_NONBLOCKING) )
    {
        *p_parentLogger << "Pipe not open for sending";
        return "";
    }

    //char* message = new char[Pipe::msg_buffer_size](); // initialize all elements to '\0'
    char message[msg_buffer_size] = { 0 };

    if(message == NULL)
    {
        *p_parentLogger << "Cannot write to a NULL container";
        return "";
    }

    std::string result = "";

    setUnavailable();
    *p_parentLogger << "Preparing to read message";
    ssize_t size = read(fd, message, msg_buffer_size -1);
    int _errno = errno;

    if(size <= 0)
    {
        if (_errno == EAGAIN || _errno == EWOULDBLOCK)
        {
            *p_parentLogger << "Nonblocking read - no data to read";
            result = "";
        }
        else
        {
            *p_parentLogger << "Cannot read the message";
            Kernel::Warning("Pipe cannot read message - " + pathname);
        }
        
    }
    else
    {
        result = message;
    }

    // delete[] message;

    setAvailable();

    *p_parentLogger << pathname + "[" + std::to_string(size) + " B] Pipe input: " + result;
    return result;
}

