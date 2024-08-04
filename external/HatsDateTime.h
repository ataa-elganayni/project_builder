#pragma once
//Author: Ataa A. Elganayni.
//First Version: March 2020.
//Last Updated: July 2024.
//**************************************

#include <iostream>
#include "string"
#include <ctime>
#include <iomanip>
#include <chrono>

using namespace std::chrono;

namespace Hats::Tools
{
    enum class DateTimeType
    {
        Year,
        Month,
        Day,
        Hour,
        Minute,
        Second,
        Milliseconds
    };

    template <DateTimeType T>
    class DateTimeComponent
    {
    private:
        uint64_t m_value;

    public:
        DateTimeComponent(uint64_t value) : m_value(value) {}
        ~DateTimeComponent() = default;

        operator uint64_t() const
        { return (m_value); }
    };

    class CalendarDay
    {
    private:
        DateTimeComponent<DateTimeType::Year> m_year {0};
        DateTimeComponent<DateTimeType::Month> m_month {0};  //1 to 12
        DateTimeComponent<DateTimeType::Day> m_day {0};
        std::string m_DayStr;
        std::string m_key;

        void Swap(CalendarDay &temp)
        {
            std::swap(m_year, temp.m_year);
            std::swap(m_month, temp.m_month);
            std::swap(m_day, temp.m_day);
            std::swap(m_DayStr, temp.m_DayStr);
            std::swap(m_key, temp.m_key);
        }

        void ConvertToString()
        {
            auto month_str = std::to_string(m_month);
            if(month_str.size() == 1)
            {
                month_str.insert(month_str.begin(), '0');
            }

            auto day_str = std::to_string(m_day);
            if(day_str.size() == 1)
            {
                day_str.insert(day_str.begin(), '0');
            }

            m_DayStr = std::string {month_str};
            m_DayStr += "-" + day_str + "-" + std::to_string(m_year);

            m_key = std::string {std::to_string(m_year)};
            m_key += month_str + day_str;
        }

    public:
        CalendarDay() = default;

        CalendarDay(DateTimeComponent<DateTimeType::Year> year,
                    DateTimeComponent<DateTimeType::Month> month,
                    DateTimeComponent<DateTimeType::Day> day) : m_year(year), m_month(month), m_day(day)
        {
            ConvertToString();
        }

        CalendarDay(const CalendarDay &other) : m_year(other.m_year),
                                                m_month(other.m_month),
                                                m_day(other.m_day)
        {
            ConvertToString();
        }

        ~CalendarDay() = default;

        CalendarDay &operator =(const CalendarDay &rhs)
        {
            auto temp = CalendarDay(rhs);
            Swap(temp);
            return (*this);
        }

        friend bool operator <(const CalendarDay& lhs, const CalendarDay& rhs)
        {
            return (lhs.m_key < rhs.m_key);
        }

        friend bool operator >(const CalendarDay& lhs, const CalendarDay& rhs)
        {
            return (lhs.m_key > rhs.m_key);
        }

        [[nodiscard]] uint64_t GetYear() const {return (m_year);}
        [[nodiscard]] uint64_t GetMonth() const {return (m_month);}
        [[nodiscard]] uint64_t GetDay() const {return (m_day);}
        [[nodiscard]] std::string ToString() const {return (m_DayStr);}
        [[nodiscard]] std::string GetKey() const {return (m_key);}
        [[nodiscard]] bool HasValue() const {return (m_day > 0 && m_month > 0 && m_year > 0);}
    };

    class HatsDateTime
    {
    private:
        int64_t m_Timestamp;

    public:
        HatsDateTime() : HatsDateTime(std::chrono::system_clock::now())
        {
        }

        HatsDateTime(DateTimeComponent<DateTimeType::Year> &year,
                     DateTimeComponent<DateTimeType::Month> &month,  //1 to 12
                     DateTimeComponent<DateTimeType::Day> &day,      //1 to 31
                     DateTimeComponent<DateTimeType::Hour> &hour,    //0 to 23
                     DateTimeComponent<DateTimeType::Minute> &minute,
                     DateTimeComponent<DateTimeType::Second> &second,
                     const std::string &timezone)
        {
            auto timeinfo = std::tm();
            timeinfo.tm_year = year - 1900; //See https://cplusplus.com/reference/ctime/tm/
            timeinfo.tm_mon = month - 1;    //tm structs counts from 0. The parameter is 1 to 12.
            timeinfo.tm_mday = day;
            timeinfo.tm_hour = hour;        //tm structs counts from 0 (00 to 23 hours)
            timeinfo.tm_min = minute;
            timeinfo.tm_sec = second;
            timeinfo.tm_isdst = -1;         //Negative value tells std::mktime() to search for daylight saving time info
            char temp[5];
            timezone.copy(temp, timezone.size());
            timeinfo.tm_zone = temp;

            std::time_t t = std::mktime (&timeinfo);
            m_Timestamp = t;
        }

        explicit HatsDateTime(const time_point<system_clock> &dateTime)
        {
            m_Timestamp = duration_cast<std::chrono::seconds>(dateTime.time_since_epoch()).count();
        }

        //brokerTimestamp is the number of seconds since January, 1st 1970 at 00:00
        explicit HatsDateTime(int64_t brokerTimestamp) : m_Timestamp(brokerTimestamp)
        {

        }

        HatsDateTime(const HatsDateTime &dt)
        {
            m_Timestamp = dt.m_Timestamp;
        }

        ~HatsDateTime() = default;

        HatsDateTime &operator =(const HatsDateTime &rhs)
        {
            HatsDateTime dt(rhs.m_Timestamp);
            auto temp = m_Timestamp;
            m_Timestamp = dt.m_Timestamp;
            dt.m_Timestamp = temp;

            return (*this);
        }

        friend bool operator <(const HatsDateTime& lhs, const HatsDateTime& rhs)
        {
            return (lhs.m_Timestamp < rhs.m_Timestamp);
        }

        [[nodiscard]] std::unique_ptr<tm> GetTimeInfo() const
        {
            const long temp = m_Timestamp;
            auto info = std::make_unique<tm>(*std::localtime(&temp));

            return (info);
        }

        [[nodiscard]] int64_t GetTimeStamp() const
        {
            return (m_Timestamp);
        }

        //Returns HatsDateTime that has the same date as this object but the time identified by the parameters.
        [[nodiscard]] HatsDateTime GetTime(DateTimeComponent<DateTimeType::Hour> hour,    //0 to 23
                                           DateTimeComponent<DateTimeType::Minute> minute,
                                           DateTimeComponent<DateTimeType::Second> second) const
        {
            auto timeinfo = GetTimeInfo();
            timeinfo->tm_hour = static_cast<int>(hour);
            timeinfo->tm_min = static_cast<int>(minute);
            timeinfo->tm_sec = static_cast<int>(second);
            timeinfo->tm_isdst = -1;         //Negative value tells std::mktime() to search for daylight saving time info
            std::time_t temp = std::mktime (&(*timeinfo));

            auto dt = HatsDateTime(temp);
            return (dt);
        }

        //Moves the datetime 24 hours from this object's datetime
//        [[nodiscard]] HatsDateTime GetNextDay() const
//        {
//            auto tp = system_clock::from_time_t(static_cast<const time_t>(m_Timestamp));
//            const std::time_t new_time_t = std::chrono::system_clock::to_time_t(tp + 24h);
//            auto time_info = std::localtime(&new_time_t);
//
//            auto dt = HatsDateTime(new_time_t);
//            return (dt);
//        }

        //Same as GetNextDay() above, except the time is set to the time identified by the parameters.
        [[nodiscard]] HatsDateTime GetNextDay(DateTimeComponent<DateTimeType::Hour> hour,    //0 to 23
                                              DateTimeComponent<DateTimeType::Minute> minute,
                                              DateTimeComponent<DateTimeType::Second> second) const
        {
            auto tp = system_clock::from_time_t(static_cast<const time_t>(m_Timestamp));
            std::time_t new_time_t = std::chrono::system_clock::to_time_t(tp + 24h);
            auto time_info = std::localtime(&new_time_t);

            //It's unfortunate we have to this. Eventually, this code would have to be moved
            //C++ 20 and make use of time zone support.
            time_info->tm_hour = static_cast<int>(hour);
            time_info->tm_min = static_cast<int>(minute);
            time_info->tm_sec = static_cast<int>(second);
            time_info->tm_isdst = -1;    //Negative value tells std::mktime() to search for daylight saving time info

            new_time_t = std::mktime (time_info);
            auto dt = HatsDateTime(new_time_t);
            return (dt);
        }

        [[nodiscard]] CalendarDay GetCalendarDay() const
        {
            auto time_info = GetTimeInfo();

            auto year = DateTimeComponent<DateTimeType::Year> {static_cast<uint64_t>(time_info->tm_year)};
            auto month = DateTimeComponent<DateTimeType::Month> {static_cast<uint64_t>(time_info->tm_mon)};
            auto day = DateTimeComponent<DateTimeType::Day> {static_cast<uint64_t>(time_info->tm_mday)};

            year = year + 1900;   //See https://cplusplus.com/reference/ctime/tm/
            month = month + 1;

            auto calendar_day = CalendarDay(year, month, day);

            return (calendar_day);
        }

        [[nodiscard]] std::string FormatDateTime(const std::string &formatStr) const
        {
            std::chrono::seconds duration(m_Timestamp);
            std::chrono::time_point<std::chrono::system_clock> tp(duration);
            std::time_t dt = system_clock::to_time_t(tp);

            char buffer[100];
            std::strftime(buffer, 100, formatStr.c_str(), std::localtime(&dt));

            return ({buffer;});
        }

        [[nodiscard]] std::string FormatDateTime() const
        {
            std::string formatStr = "%m-%d-%Y %H:%M:%S:%ms";
            return (FormatDateTime(formatStr));
        }
    };

    inline bool operator ==(const HatsDateTime &lhs, const HatsDateTime &rhs)
    {
        return (lhs.GetTimeStamp() == rhs.GetTimeStamp());
    }

    inline long operator -(const HatsDateTime &lhs, const HatsDateTime &rhs)
    {
        auto numSeconds = lhs.GetTimeStamp() - rhs.GetTimeStamp();
        return (numSeconds);
    }

    inline HatsDateTime operator +(const HatsDateTime &lhs,
                                   DateTimeComponent<DateTimeType::Milliseconds> num_milliseconds)
    {
        auto newDateTime = lhs.GetTimeStamp() + (num_milliseconds / 1000);
        return (HatsDateTime(newDateTime));
    }

    inline HatsDateTime operator -(const HatsDateTime &lhs, long long rhs)
    {
        auto newDateTime = lhs.GetTimeStamp() - rhs;
        return (HatsDateTime(newDateTime));
    }

    inline bool operator >(const HatsDateTime &lhs, const HatsDateTime &rhs) { return(rhs < lhs);}
    inline bool operator >=(const HatsDateTime &lhs, const HatsDateTime &rhs) {return(!(rhs > lhs));}
    inline bool operator <=(const HatsDateTime &lhs, const HatsDateTime &rhs) {return(!(rhs < lhs));}
}

