#include "Time.hpp"

#include<iomanip>
#include<sstream>
#include<sys/time.h>

Timezone::timezone Time::time_zone = Timezone::GMT;

timespec Time::raw_time = {};
tm Time::refined_time = {};

bool operator== (const timespec& t1, const timespec& t2)
{
    return (t1.tv_sec == t2.tv_sec) && (t1.tv_nsec == t2.tv_nsec);
}

void Time::updateTime()
{
    clock_gettime( clock, &raw_time ); // TODO check for errors?
    refined_time = *(localtime(&raw_time.tv_sec));
}

std::string Time::getDate()
{
    std::stringstream dateStringStream;

    dateStringStream << std::setfill('0')
        << std::setw(4) << getYear() << "-"
        << std::setw(2) << getMonth() << "-"
        << std::setw(2) << getDay();

    return dateStringStream.str();
}

std::string Time::getTime()
{
    std::stringstream timeStringStream;

    timeStringStream << std::setfill('0')
                     << std::setw(2) << getHours() << ":"
                     << std::setw(2) << getMinutes() << ":"
                     << std::setw(2) << getSeconds() << "."
                     << std::setw(3) << getMilliSeconds();

    return timeStringStream.str();
}

std::string Time::getDateTime_ISO8601()
{

    std::string timeString = getTime();
    std::string dateString = getDate();

    return dateString + " " + timeString;

}

struct timespec Time::getRawTime()
{  
    updateTime();
    return raw_time;
}


unsigned int Time::getHours()
{

    // not very efficient
    updateTime();
    int hours = refined_time.tm_hour + time_zone.getHourOffset();
    int minutes = refined_time.tm_min + time_zone.getMinuteOffset();
    int offsetDueToMinuteOverflow = 0;

    if(minutes >= 60)
        offsetDueToMinuteOverflow = +1;
    else if(minutes < 0)
        offsetDueToMinuteOverflow = -1;
    
    hours = (hours + offsetDueToMinuteOverflow) % 24; // * date overflow

    return (hours > 0) ? hours : 24 + hours; // 24 + hours because hours are negative => 24 - abs(hours)
}

unsigned int Time::getMinutes()
{
    updateTime();
    int minutes = ( refined_time.tm_min + time_zone.getMinuteOffset() ) % 60;
    return (minutes >= 0) ? minutes : 60 + minutes; // 60 + minutes because minutes are negative => 60 - abs(minutes)
}

unsigned int Time::getSeconds()
{
    updateTime();
    return refined_time.tm_sec;

}

unsigned int Time::getMilliSeconds()
{
    return raw_time.tv_nsec / 1000000;
}

unsigned int Time::getYear()
{
    updateTime();

    const unsigned int yearOffset = 1900;

    return yearOffset + refined_time.tm_year;
}

unsigned int Time::getMonth()
{
    updateTime();
    return refined_time.tm_mon;
}

unsigned int Time::getDay()
{
    updateTime();
    return refined_time.tm_mday;
}

void Time::setTimeZone(Timezone::timezone _time_zone)
{
    time_zone = _time_zone;
}

Timezone::timezone& Time::getTimeZone()
{
    return time_zone;
}

timespec Time::getTimespecFrom_s(time_t seconds)
{
    timespec time_structure = { 0, 0 };

    if(seconds >= 0)
        time_structure.tv_sec = seconds;

    return time_structure;
}

timespec Time::getTimespecFrom_ms(long milliseconds)
{
    timespec time_structure = { 0, 0 };

    if (milliseconds >= 0)
    {
        time_structure.tv_nsec = milliseconds % Time::s_to_ms * Time::ms_to_ns;
        time_structure.tv_sec = milliseconds / Time::s_to_ms;
    }

    return time_structure;
}

timespec Time::getTimespecFrom_us(long microseconds)
{
    timespec time_structure = { 0, 0 };

    if (microseconds >= 0)
    {
        time_structure.tv_nsec = microseconds % Time::s_to_us * Time::us_to_ns;
        time_structure.tv_sec = microseconds / Time::s_to_us;
    }

    return time_structure;
}

timespec Time::getTimespecFrom_ns(long nanoseconds)
{
    timespec time_structure = { 0, 0 };

    if (nanoseconds >= 0)
    {
        time_structure.tv_nsec = nanoseconds % Time::itimerspec_max_ns;
        time_structure.tv_sec = nanoseconds / Time::itimerspec_max_ns;
    }

    return time_structure;
}

bool Time::isValid(const timespec& timespec_structure)
{
    return (timespec_structure.tv_sec >= 0) && (timespec_structure.tv_nsec >= 0 && timespec_structure.tv_nsec < itimerspec_max_ns);
}
