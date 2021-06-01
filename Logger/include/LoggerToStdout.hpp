#ifndef LOGGER_TO_STDOUT_HPP
#define LOGGER_TO_STDOUT_HPP

#include "ILogger.hpp"

#include "Time.hpp"

#include <iostream>

/**
 * @brief Logger class which writes all log messages to standard output
 *
 */
class LoggerToStdout : public ILogger
{
private:
    std::string m_name;

protected:
    virtual ILogger& logString(std::string const& report)
    {
        std::cout << Time::getTime() << "-- " << report << std::endl;
        return *this;
    }

public:
    LoggerToStdout(const std::string& name) : m_name(name) {}; // TODO check if name is empty (or not)
    virtual ~LoggerToStdout(void) {};

    virtual std::string getName() const { return m_name; };
};
#endif