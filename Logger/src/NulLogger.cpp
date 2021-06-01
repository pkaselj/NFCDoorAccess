#include"NulLogger.hpp"

NulLogger* NulLogger::getInstance()
{
    static NulLogger globalNulLogger;
    return &globalNulLogger;
}

NulLogger& NulLogger::logString (std::string const& report)
{
    return *this;
}

std::string NulLogger::getName() const
{
    return "NO LOG FILE CREATED (NULLOGGER)!";
}

NulLogger::~NulLogger()
{
    
}
