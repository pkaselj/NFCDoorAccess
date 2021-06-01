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