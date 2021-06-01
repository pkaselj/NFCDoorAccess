#ifndef ILOGGER_HPP
#define ILOGGER_HPP

#include<iostream>
#include<fstream>
#include<cstdio>
#include<unistd.h>
#include<cstdlib>
#include<sys/stat.h>
#include<sys/time.h>

/**
 * @brief Logger interface class
 * 
 */
class ILogger
{
    protected:
    /**
     * @brief Main logger function.
     * 
     * Function which get called whenever \n
     * << operator is used on a child object. \n
     * \n
     * Main function is to support polymorphism \n
     * since friend functions cannot be overridden. \n
     * 
     * @param report Message to be logged.
     * @return ILogger& returns ILogger reference to support stacking
     */
    virtual ILogger& logString (std::string const& report) = 0;

    

    public:
    //ILogger(void) = delete;
    virtual ~ILogger(void) {};

    /**
     * @brief Get the Name og the ILogger object
     * 
     * @return const std::string& name
     */
    virtual std::string getName() const = 0;

    /**
     * @brief << operator overload
     * 
     * @param logStream ILogger reference 
     * @param report Message to be written
     * @return ILogger& returns ILogger reference to support stacking
     */
    friend ILogger& operator <<(ILogger& logStream, std::string const& report);
};

inline ILogger& operator <<(ILogger& logStream, std::string const& report)
{
    return logStream.logString(report);
}

#endif