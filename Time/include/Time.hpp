#ifndef TIME_HPP
#define TIME_HPP

#include"Timezones.hpp"

#include<time.h>

#include<string>
#include<limits>

bool operator== (const timespec& t1, const timespec& t2);

/**
 * @brief Static Time class
 * 
 */
class Time
{

    private:
    /// Used to store current time up to nanosecond resolution.
    static timespec raw_time;

    /// Used to store refined (calendar) time and date.
    static tm refined_time;

    /// Defines which clock will be used when getting the time.
    static const clockid_t clock = CLOCK_REALTIME;
    // DEBUG
    // static const clockid_t clock = CLOCK_MONOTONIC;

    /// Defines timezone structure to account for different time zones
    static Timezone::timezone time_zone;
    
    /// Get the system time and updates the `raw_time` and `refined_time` objects
    static void updateTime();
    
    public:

    /// Deleted default constructor (since C++ doesn't have static classes)
    Time() = delete;

    /**
     * @brief Returns the current time up to millisecond resolution
     * 
     * @return std::string 'HH:MM:SS.mmm' time formatted string 
     */
    static std::string getTime();

    /**
     * @brief Returns the current date
     *
     * @return std::string 'YYYY-MM-DD' date formatted string
     */
    static std::string getDate();

    /**
    * @brief Returns the current Datetime up to millisecond resolution in ISO 8601 representation
    *
    * @return std::string ISO 8601 representation Datetime formatted string
    */
    static std::string getDateTime_ISO8601();

    /**
    * @brief Formats the `rawTime` Datetime up to millisecond resolution to ISO 8601 representation
    *
    * @return std::string ISO 8601 representation Datetime formatted string
    */
    // static std::string toDateTime_ISO8601(const timespec rawTime_);

    /**
     * @brief Returns the current time in timespec structure
     * 
     * @return struct timespec raw current time
     */
    static struct timespec getRawTime();

    /**
     * @brief Updates time and gets the current hour
     * 
     * @return unsigned int current hour
     */
    static unsigned int getHours();

    /**
     * @brief Updates time and gets the current minute
     * 
     * @return unsigned int current minute
     */
    static unsigned int getMinutes();

    /**
     * @brief Updates time and gets the current second
     * 
     * @return unsigned int current second
     */
    static unsigned int getSeconds();

    /**
     * @brief Updates time and gets the current millisecond
     * 
     * @return unsigned int current millisecond
     */
    static unsigned int getMilliSeconds();

    /**
     * @brief Updates time and gets the current year
     *
     * @return unsigned int current year
     */
    static unsigned int getYear();

    /**
    * @brief Updates time and gets the current month
    *
    * @return unsigned int current month
    */
    static unsigned int getMonth();

    /**
     * @brief Updates time and gets the current day
     *
     * @return unsigned int current day
     */
    static unsigned int getDay();

    /**
     * @brief Set the Time Zone
     * 
     * Default timezone: GMT/UTC
     * 
     * @param _time_zone `Timezone::timezone` object
     */
    static void setTimeZone(Timezone::timezone _time_zone);

    /**
     * @brief Get the current Time Zone settings
     * 
     * @return Timezone::timezone& reference to the current `Timezone::timezone` object
     */
    static Timezone::timezone& getTimeZone();

    /// Definitions of time units

    static constexpr long ns = 1; 
    static constexpr long us_to_ns = 1000 * ns;
    static constexpr long ms_to_ns = 1000000 * ns;
    static constexpr long ms_to_us = ms_to_ns / us_to_ns;
    static constexpr long s_to_ms  = 1000;
    static constexpr long s_to_us  = 1000000;

    static constexpr long itimerspec_max_ns = 1000 * ms_to_ns; // max value if itimerspec::(...)::tv_nsec

    /// Maximal time interval that can be specified on this machine (in ms)
    static constexpr unsigned int max_time_interval_ms = std::numeric_limits<long>::max() / ms_to_ns;
    
    /// Convert `time_t seconds` to a proper `timespec` structure \see `isValid(...)`
    static timespec getTimespecFrom_s(time_t seconds);

    /// Convert `long milliseconds` to a proper `timespec` structure \see `isValid(...)`
    static timespec getTimespecFrom_ms(long milliseconds);

    /// Convert `long microseconds` to a proper `timespec` structure \see `isValid(...)`
    static timespec getTimespecFrom_us(long microseconds);

    /// Convert `long nanoseconds` to a proper `timespec` structure \see `isValid(...)`
    static timespec getTimespecFrom_ns(long nanoseconds);

    /// Returns true if `timespec_structure` is a valid `timespec` structure. (tv_sec > 0 &&  999,999,999 > tv_nsec > 0)
    static bool isValid(const timespec& timespec_structure);

};

#endif