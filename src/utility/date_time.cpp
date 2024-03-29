#include <chrono>
#include <string>
#include <sstream>
#include <iostream>
#include <thread>
#include <ctime>
#include <stdexcept>

#include "utility/date_time.hpp"

using namespace Utility;

std::string DateTime::zero_fill(const std::string& str, const std::size_t& digit) const
{
    std::string result(str);
    int remain = digit > str.size() ? digit - str.size() : 0;
    while(remain--)
        result = '0' + result;
    return result;
}

void DateTime::dump(std::string& str, const char* fmt, const std::string& rep) const
{
    std::string::size_type pos = 0;
    int fmt_size = std::string(fmt).size();
    while(true)
    {
        pos = str.find(fmt, pos);
        if(pos == std::string::npos)
            break;
        else
        {
            auto l = str.substr(0, pos);
            auto r = str.substr(pos+fmt_size);
            str = l + rep + r;
            pos = pos + rep.size();
        }
    }
}

void DateTime::parse(std::string& dt_str, std::string& fmt_str, const char* fmt, int& val, const int& low_range, const int& upper_range)
{
    std::string::size_type pos = 0;
    int fmt_size = std::string(fmt).size();
    while(true)
    {
        pos = fmt_str.find(fmt, pos);
        if(pos == std::string::npos)
            break;
        else
        {
            int tmp;
            try
            {
                tmp = std::stol(dt_str.substr(pos, fmt_size));
            }
            catch(const std::invalid_argument& err)
            {
                std::stringstream ss;
                ss << " convert from " << dt_str.substr(pos, fmt_size) << " to " << fmt << " failed";
                throw std::runtime_error(ss.str());
            }
            fmt_str.replace(pos, fmt_size, fmt_size, '*');
            dt_str.replace(pos, fmt_size, fmt_size, '*');
            
            if(tmp<low_range || tmp>upper_range)
            {
                std::stringstream ss;
                ss << tmp  << " is out of range for " << fmt 
                    << " (" << low_range << " - " << upper_range << ")";
                throw std::runtime_error(ss.str());
            }

            val = tmp;
            pos = pos + fmt_size;
        }
    }
}

DateTime DateTime::from_str(const char* date_time_str, const char* format)
{
    std::string dt_str(date_time_str);
    std::string fmt_str(format);
    int year, mon, day, hour, min, sec, msec, usec;
    year = 1970; mon = day = 1; day = hour = min = sec = msec = usec = 0;
    int days_of_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    try 
    {
        parse(dt_str, fmt_str,   "yyyy", year, 1970,    2300);
        if(year%4==0 && !(year%100==0 && year%400!=0))
            days_of_month[1]++;
        
        parse(dt_str, fmt_str,     "mm",  mon,    1,      12);
        parse(dt_str, fmt_str,     "dd",  day,    1, days_of_month[mon-1]);
        parse(dt_str, fmt_str,     "hh", hour,    0,      23);
        parse(dt_str, fmt_str,     "nn",  min,    0,      59);
        parse(dt_str, fmt_str,     "ss",  sec,    0,      59);
        parse(dt_str, fmt_str, "zzzzzz", usec,    0,  999999);
        msec = usec/1000; usec=usec%1000;
        parse(dt_str, fmt_str,    "zzz", msec,    0,    999);
    } 
    catch(const std::runtime_error& e) 
    {
        std::stringstream ss;
        ss << __func__      << " failed. "
            << date_time_str << " couldn't parse to " 
            << format        << "." << e.what();
        throw std::runtime_error(ss.str());
    }

    return DateTime(year, mon, day, hour, min, sec, msec, usec);
}

DateTime::DateTime(const system_clock::time_point& tP)
{
    using std::chrono::duration_cast;
    using std::chrono::seconds;
    using std::chrono::microseconds;
    
    auto tT  = system_clock::to_time_t(tP);
    auto tM = std::gmtime(&tT);
    int usecEpoch  = duration_cast<microseconds>(tP.time_since_epoch()).count();
    int secEpoch   = 1000000 * duration_cast<seconds>(tP.time_since_epoch()).count();
    int usec = usecEpoch - secEpoch;
    year_ = tM->tm_year+1900; mon_  = tM->tm_mon+1; day_  = tM->tm_mday;
    hour_ = tM->tm_hour; min_  = tM->tm_min; sec_  = tM->tm_sec;
    msec_ = usec/1000; usec_ = usec%1000;
}

DateTime::DateTime(const char* str, const char* fmt)
{
    *this = from_str(str, fmt);
}

DateTime::DateTime(const int& year, const int& mon, const int& day, const int& hour, const int& min, const int& sec, const int& msec, const int& usec)
 :  year_(year), mon_(mon), day_(day), 
    hour_(hour), min_(min), sec_(sec), msec_(msec), usec_(usec)
{}

DateTime::DateTime(const DateTime& other)
 :  year_(other.year_), mon_(other.mon_), day_(other.day_), 
    hour_(other.hour_), min_(other.min_), sec_(other.sec_), msec_(other.msec_), usec_(other.usec_)
{}

void DateTime::operator=(const DateTime& other)
{
    year_ = other.year_; mon_  = other.mon_; day_ = other.day_; 
    hour_ = other.hour_; min_  = other.min_; sec_ = other.sec_; 
    msec_ = other.msec_; usec_ = other.usec_;
}

bool DateTime::operator==(const DateTime& other) const { return (to_time_point() == other.to_time_point()); }
bool DateTime::operator!=(const DateTime& other) const { return (to_time_point() != other.to_time_point()); }
bool DateTime::operator<(const DateTime& other)  const { return (to_time_point() <  other.to_time_point()); }
bool DateTime::operator>(const DateTime& other)  const { return (to_time_point() >  other.to_time_point()); }
int DateTime::year()        const { return year_; }
int DateTime::month()       const { return  mon_; }
int DateTime::day()         const { return  day_; }
int DateTime::hour()        const { return hour_; }
int DateTime::minute()      const { return  min_; }
int DateTime::second()      const { return  sec_; }
int DateTime::millisecond() const { return msec_; }
int DateTime::microsecond() const { return usec_; }

std::string DateTime::to_str(const char *format) const
{
    std::string result(format);

    dump(result,   "yyyy", std::to_string(year_));
    dump(result,     "mm", zero_fill(std::to_string(mon_), 2));
    dump(result,     "dd", zero_fill(std::to_string(day_), 2));
    dump(result,     "hh", zero_fill(std::to_string(hour_), 2));
    dump(result,     "nn", zero_fill(std::to_string(min_), 2));
    dump(result,     "ss", zero_fill(std::to_string(sec_), 2));
    dump(result, "zzzzzz", zero_fill(std::to_string(1000*msec_ + usec_), 6));
    dump(result,    "zzz", zero_fill(std::to_string(msec_), 3));

    return std::string(result);
}

std::chrono::system_clock::time_point DateTime::to_time_point() const
{
    using std::chrono::milliseconds;
    using std::chrono::microseconds;

    std::tm tm_var{0};
    tm_var.tm_year  = year_  - 1900;
    tm_var.tm_mon   = mon_ - 1;
    tm_var.tm_mday  = day_;
    tm_var.tm_hour  = hour_;
    tm_var.tm_min   = min_;
    tm_var.tm_sec   = sec_;
    tm_var.tm_isdst = -1;
    std::time_t time_seed;
    std::time(&time_seed);  
    auto tt = std::mktime(&tm_var) + std::mktime(std::localtime(&time_seed)) - std::mktime(std::gmtime(&time_seed));
    auto tp = system_clock::from_time_t(tt) + milliseconds(msec_) + microseconds(usec_);

    return tp;
}

DateTime& DateTime::add(const int& increment, const enum Unit& unit)
{
    if(unit==HOUR)
        *this = DateTime(to_time_point() + std::chrono::hours(increment));
    else if(unit==MIN)
        *this = DateTime(to_time_point() + std::chrono::minutes(increment));
    else if(unit==SEC)
        *this = DateTime(to_time_point() + std::chrono::seconds(increment));
    else if(unit==MSEC)
        *this = DateTime(to_time_point() + std::chrono::milliseconds(increment));
    else if(unit==USEC)
        *this = DateTime(to_time_point() + std::chrono::microseconds(increment));
    return *this;
}

DateTime DateTime::add(const DateTime& other, const int& increment, const enum Unit& unit)
{
    auto result = other;
    result.add(increment, unit);
    return result;
}