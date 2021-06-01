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

#ifndef PROCESS_STATUS_HPP
#define PROCESS_STATUS_HPP

/**
 * @brief Enum of possible states of processes/threads declared as `char`s
 * 
 */
enum Status : char {CLEAR = 0, BUSY = 1, IDLE = 2, TERMINATE = 3, FREE = 4, CLAIMED = 5};


/**
 * @brief Wrapper struct for process status
 * 
 */
typedef struct ProcessStatus ProcessStatus;
struct ProcessStatus
{
    private:
    /// Status of the current process/thread
    Status status;

    public:

    /**
     * @brief Construct a new Process Status object with status equal to Status::FREE
     * 
     */
    ProcessStatus();

    /**
     * @brief Construct a new Process Status object with status equal to _status
     * 
     * @param _status Status enum representing current status
     */
    ProcessStatus(Status _status);

    /**
     * @brief Set the status of the ProcessStatus object
     * 
     * @param _status New status
     */
    void set(Status _status);

    /**
     * @brief Compare the ProcessStatus object to the Status enum
     * 
     * @param _status Status enum
     * @return true ProcessStatus is equal to the Status enum
     * @return false ProcessStatus is NOT equal to the Status enum
     */
    bool operator==(Status _status) const;

    /**
     * @brief Update the ProcessStatus to new status
     * 
     * @param _status Status enum
     * @return const ProcessStatus& Returns the reference to this ProcessStatus object. Used for stacking.
     */
    ProcessStatus& operator=(Status _status);

    /**
     * @brief Conversion to `int` type
     * 
     * Mostly used for printing `int` value of ProcessStatus
     * 
     * @return int `int` value of current ProcessStatus
     */
    operator int() const;
};

ProcessStatus::ProcessStatus()
{
    ProcessStatus(Status::FREE);
}

ProcessStatus::ProcessStatus(Status _status)
{
    set(_status);
}

void ProcessStatus::set(Status _status)
{
    status = _status;
}

bool ProcessStatus::operator==(Status _status) const
{
    return (status == _status) ? true : false;
}
ProcessStatus& ProcessStatus::operator=(Status _status)
{
    status = _status;
    return *this;
}

ProcessStatus::operator int() const
{
    return status;
}

#endif