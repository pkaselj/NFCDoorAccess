#ifndef TIMEZONES_HPP
#define TIMEZONES_HPP

#include<iostream> // ?


/*namespace Timezone
{
    /// Class containing timezone information
    class timezone
    {
        private:
            int hourOffset = 0;
            int minuteOffset = 0;
        public:
        */
            /**
             * @brief Get the hour offset of current timezone from the GMT/UTC 
             * 
             * @return const int signed hour offset from GMT/UTC
             */
            //const int getHourOffset();

            /**
             * @brief Get the minute offset of current timezone from the GMT/UTC
             * 
             * @return const int signed minute offset from GMT/UTC
             */
            //const int getMinuteOffset();

            /*const timezone() = default;

            const timezone(const float offset);

            const timezone(const timezone&) = default;
            const timezone(timezone&&) = default;
            const timezone& operator= (const timezone&) = default;

            friend const timezone operator+ (const timezone& time_zone, const float offset);
            friend const timezone operator- (const timezone& time_zone, const float offset);

    };

    const timezone::timezone(const float offset)
      : timezone()
    {
        hourOffset = (int)offset;
        minuteOffset = (offset - (int)offset) * 60;
    }

    const int timezone::getHourOffset()
    {
        return hourOffset;
    }
    const int timezone::getMinuteOffset()
    {
        return minuteOffset;
    }

    const timezone operator+ (const timezone& time_zone, const float offset)
    {
        Timezone::timezone temp_time_zone;
        temp_time_zone.hourOffset += (int) offset;
        temp_time_zone.minuteOffset += (offset - (int) offset) * 60;
        return time_zone;
    }
    const timezone operator- (const timezone& time_zone, const float offset)
    {
        return time_zone + (-offset);
    }

    /// Greenwich Mean Time
    const timezone GMT = 0;
    /// Universal Cooridinated Time
    const timezone UTC = GMT;
    /// European Central Time
    const timezone ECT = GMT+1;
    /// Eastern European Time
    const timezone EET = GMT+2;
    /// (Arabic) Egypt Standard Time
    const timezone ART = GMT+2;
    /// Eastern African Time
    const timezone EAT = GMT+3;
    /// Middle East Time
    const timezone MET = GMT+3.5;
    /// Near East Time
    const timezone NET = GMT+4;
    /// Pakistan Lahore Time
    const timezone PLT = GMT+5;
    /// India Standard Time
    const timezone IST = GMT+5.5;
    /// Bangladesh Standard Time
    const timezone BST = GMT+6;
    /// Vietnam Standard Time
    const timezone VST = GMT+7;
    /// China Taiwan Time
    const timezone CTT = GMT+8;
    /// Japan Standard Time
    const timezone JST = GMT+9;
    /// Australia Central Time
    const timezone ACT = GMT+9.5;
    /// Australia Eastern Time
    const timezone AET = GMT+10;
    /// Solomon Standard Time
    const timezone SST = GMT+11;
    /// New Zealand Standard Time
    const timezone NST = GMT+12;
    /// Midway Islands Time
    const timezone MIT = GMT-11;
    /// Hawaii Standard Time
    const timezone HST = GMT-10;
    /// Alaska Standard Time
    const timezone AST = GMT-9;
    /// Pacific Standard Time
    const timezone PST = GMT-8;
    /// Phoenix Standard Time
    const timezone PNT = GMT-7;
    /// Mountain Standard Time
    const timezone MST = GMT-7;
    /// Central Standard Time
    const timezone CST = GMT-6;
    /// Eastern Standard Time
    const timezone EST = GMT-5;
    /// Indiana Eastern Standard Time
    const timezone IET = GMT-5;
    /// Puerto Rico and US Virgin Islands Time
    const timezone PRT = GMT-4;
    /// Canada Newfoundland Time
    const timezone CNT = GMT-3.5;
    /// Argentina Standard Time
    const timezone AGT = GMT-3;
    /// Brazil Eastern Time
    const timezone BET = GMT-3;
    /// Central African Time
    const timezone CAT = GMT-1;



}*/

namespace Timezone
{
    /// Class containing timezone information
    class timezone
    {
        private:
            int hourOffset = 0;
            int minuteOffset = 0;
        public:
        
            /**
             * @brief Get the hour offset of current timezone from the GMT/UTC 
             * 
             * @return const int signed hour offset from GMT/UTC
             */
            int getHourOffset();

            /**
             * @brief Get the minute offset of current timezone from the GMT/UTC
             * 
             * @return const int signed minute offset from GMT/UTC
             */
            int getMinuteOffset();

            timezone() = default;

            timezone(const float offset);

            timezone(const timezone& tz);
            timezone(timezone&& tz);
            const timezone& operator= (const timezone& tz);

            friend timezone operator+ (timezone time_zone, const float offset);
            friend timezone operator- (timezone time_zone, const float offset);

    };

    timezone::timezone(const timezone& tz)
    {
        hourOffset = tz.hourOffset;
        minuteOffset = tz.minuteOffset;
    }
    
    timezone::timezone(timezone&& tz)
    {
        hourOffset = tz.hourOffset;
        minuteOffset = tz.minuteOffset;
    }

    const timezone& timezone::operator=(const timezone& tz)
    {
        hourOffset = tz.hourOffset;
        minuteOffset = tz.minuteOffset;
        return *this;
    }

    timezone::timezone(const float offset)
    {
        hourOffset = (int)offset;
        minuteOffset = (offset - (int)offset) * 60;
    }

    int timezone::getHourOffset()
    {
        return hourOffset;
    }
    int timezone::getMinuteOffset()
    {
        return minuteOffset;
    }

    timezone operator+ (timezone time_zone, const float offset)
    {
        time_zone.hourOffset += (int) offset;
        time_zone.minuteOffset += (offset - (int) offset) * 60;
        return time_zone;
    }
    timezone operator- (timezone time_zone, const float offset)
    {
        return time_zone + (-offset);
    }

    /// Greenwich Mean Time
    const timezone GMT = 0;
    /// Universal Cooridinated Time
    const timezone UTC = GMT;
    /// European Central Time
    const timezone ECT = GMT+1;
    /// Eastern European Time
    const timezone EET = GMT+2;
    /// (Arabic) Egypt Standard Time
    const timezone ART = GMT+2;
    /// Eastern African Time
    const timezone EAT = GMT+3;
    /// Middle East Time
    const timezone MET = GMT+3.5;
    /// Near East Time
    const timezone NET = GMT+4;
    /// Pakistan Lahore Time
    const timezone PLT = GMT+5;
    /// India Standard Time
    const timezone IST = GMT+5.5;
    /// Bangladesh Standard Time
    const timezone BST = GMT+6;
    /// Vietnam Standard Time
    const timezone VST = GMT+7;
    /// China Taiwan Time
    const timezone CTT = GMT+8;
    /// Japan Standard Time
    const timezone JST = GMT+9;
    /// Australia Central Time
    const timezone ACT = GMT+9.5;
    /// Australia Eastern Time
    const timezone AET = GMT+10;
    /// Solomon Standard Time
    const timezone SST = GMT+11;
    /// New Zealand Standard Time
    const timezone NST = GMT+12;
    /// Midway Islands Time
    const timezone MIT = GMT-11;
    /// Hawaii Standard Time
    const timezone HST = GMT-10;
    /// Alaska Standard Time
    const timezone AST = GMT-9;
    /// Pacific Standard Time
    const timezone PST = GMT-8;
    /// Phoenix Standard Time
    const timezone PNT = GMT-7;
    /// Mountain Standard Time
    const timezone MST = GMT-7;
    /// Central Standard Time
    const timezone CST = GMT-6;
    /// Eastern Standard Time
    const timezone EST = GMT-5;
    /// Indiana Eastern Standard Time
    const timezone IET = GMT-5;
    /// Puerto Rico and US Virgin Islands Time
    const timezone PRT = GMT-4;
    /// Canada Newfoundland Time
    const timezone CNT = GMT-3.5;
    /// Argentina Standard Time
    const timezone AGT = GMT-3;
    /// Brazil Eastern Time
    const timezone BET = GMT-3;
    /// Central African Time
    const timezone CAT = GMT-1;



}




#endif