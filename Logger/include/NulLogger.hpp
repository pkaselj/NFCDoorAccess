#ifndef NULLOGGER_HPP
#define NULLOGGER_HPP

#include"ILogger.hpp"

/**
 * @brief NulLogger class - Singleton
 * 
 * Used as a default logger in other classes. \n
 * \n
 * Does nothing with the input. Used to implement logger as an optional parameter. \n
 * \n
 * example:
 *      ILogger* pLogger = NulLogger::getInstance();
 *      pLogger << "Do nothing!";
 */
class NulLogger : public ILogger
{
    protected:
    virtual NulLogger& logString (std::string const& report);

    private:
    NulLogger(){};

    public:
    static NulLogger* getInstance();

    virtual std::string getName() const;

    virtual ~NulLogger();

    NulLogger(NulLogger&) = delete;
    void operator= (NulLogger&) = delete;
};

#endif