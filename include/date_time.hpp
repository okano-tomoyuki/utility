#ifndef _UTILITY_DATE_TIME_HPP_
#define _UTILITY_DATE_TIME_HPP_

#include <chrono>
#include <sstream>
#include <string>
#include <iostream>
#include <locale>

namespace Utility
{

class DateTime
{
using system_clock = std::chrono::system_clock;
using time_point   = std::chrono::system_clock::time_point;

public:
    explicit DateTime() = delete;

    enum Unit
    {
        HOUR,
        MINUTE,
        SECOND,
        MILLI_SECOND
    };

    /**! 現在の Unix 経過時間(ミリ秒) を取得 */
    static int64_t get_epoch(const time_point& tp = system_clock::now())
    {
        auto elapsed = tp.time_since_epoch();
        auto elapsed_milli_second = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        return elapsed_milli_second;
    }

    /**! Unix 経過時間(ミリ秒) */
    static std::string epoch_to_iso(const int64_t& elapsed_milli_second)
    {
        int64_t elapsed_second = elapsed_milli_second / 1000;
        char iso[sizeof("yyyy-mm-ddThh:mi:ss")];
        char iso_msec[sizeof("yyyy-mm-ddThh:mi:ss.sssZ")];
        std::strftime(iso, sizeof(iso), "%FT%T", std::gmtime(&elapsed_second));
        short delta = elapsed_milli_second - (elapsed_second * 1000);
        std::snprintf(iso_msec, sizeof(iso_msec),"%s.%03dZ", iso, delta);
        return iso_msec;
    }

    /**! 現在のISO Unix 経過時間(ミリ秒) を取得 */
    static std::string get_iso(const time_point& tp = system_clock::now())
    {
        auto elapsed_milli_second = get_epoch(tp);
        return epoch_to_iso(elapsed_milli_second);
    }

    /**! ISO Unix時間の文字列 -> Unix 経過時間(ミリ秒)*/
    static int64_t iso_to_epoch(const std::string& iso)
    {
        int year;
        int month;
        int day;
        int hour;
        int minute;
        int second;
        int milli_second;

        std::sscanf(iso.c_str(), "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ", &year, &month, &day, &hour, &minute, &second, &milli_second);
        std::tm tm_val = {};
        tm_val.tm_year = year - 1900;
        tm_val.tm_mon  = month - 1;
        tm_val.tm_mday = day;
        tm_val.tm_hour = hour;
        tm_val.tm_min  = minute;
        tm_val.tm_sec  = second;
        tm_val.tm_isdst = -1;

        std::time_t time_seed;
        std::time(&time_seed);  
        auto offset_from_utc = std::mktime(std::localtime(&time_seed)) - std::mktime(std::gmtime(&time_seed));

        auto tp = system_clock::from_time_t(std::mktime(&tm_val));
        auto elapsed = tp.time_since_epoch();
        auto elapsed_milli_second = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        return elapsed_milli_second + offset_from_utc * 1000 + milli_second;
    }

    static int64_t add(const int64_t& elapsed_millisecond, const int64_t& add_value, const enum Unit& unit=MILLI_SECOND)
    {
        using std::chrono::duration_cast;
        if(unit==MILLI_SECOND)
            return elapsed_millisecond + duration_cast<std::chrono::milliseconds>(std::chrono::milliseconds{add_value}).count();  
        else if(unit==SECOND)
            return elapsed_millisecond + duration_cast<std::chrono::milliseconds>(std::chrono::seconds{add_value}).count();
        else if(unit==MINUTE)
            return elapsed_millisecond + duration_cast<std::chrono::milliseconds>(std::chrono::minutes{add_value}).count();
        else if(unit==HOUR)
            return elapsed_millisecond + duration_cast<std::chrono::milliseconds>(std::chrono::hours{add_value}).count();
        else
            throw std::runtime_error("Input unit name is unknown.");    
    }

    static std::string add(const std::string& iso, const int64_t& add_value, const enum Unit& unit=MILLI_SECOND)
    {
        return epoch_to_iso(add(iso_to_epoch(iso), add_value, unit));
    }

    /**! 時刻比較 Unix経過時間(ミリ秒) どうし */
    static int64_t diff(const int64_t& lhs, const int64_t& rhs, const enum Unit& unit=MILLI_SECOND)
    {
        using std::chrono::duration_cast;
        std::chrono::milliseconds diff_milli_second{lhs - rhs};
        if(unit==MILLI_SECOND)
            return duration_cast<std::chrono::milliseconds>(diff_milli_second).count();            
        else if(unit==SECOND)
            return duration_cast<std::chrono::seconds>(diff_milli_second).count();
        else if(unit==MINUTE)
            return duration_cast<std::chrono::minutes>(diff_milli_second).count();
        else if(unit==HOUR)
            return duration_cast<std::chrono::hours>(diff_milli_second).count();
        else
            throw std::runtime_error("Input unit name is unknown.");  
    }

    /**! 時刻比較 ISO Unix時間の文字列どうし */
    static int64_t diff(const std::string& lhs, const std::string& rhs, const enum Unit& unit=MILLI_SECOND)
    {
        return diff(iso_to_epoch(lhs), iso_to_epoch(rhs), unit);
    }

    /**! 時刻比較 ISO Unix時間の文字列 と Unix経過時間(ミリ秒) */
    static int64_t diff(const std::string& lhs, const int64_t& rhs, const enum Unit& unit=MILLI_SECOND)
    {
        return diff(iso_to_epoch(lhs), rhs, unit);
    }

    /**! 時刻比較 Unix経過時間(ミリ秒) と ISO Unix時間の文字列 */
    static int64_t diff(const int64_t& lhs, const std::string& rhs, const enum Unit& unit=MILLI_SECOND)
    {
        return diff(lhs, iso_to_epoch(rhs), unit);
    }
};

class ProcessTimer
{

using system_clock = std::chrono::system_clock;
using time_point   = std::chrono::system_clock::time_point;

private:

    /**! 処理時間計測に用いる際のタイマー名   */
    std::string name_;

    /**! 処理時間計測に用いる際の基準時刻     */
    time_point base_time_;

    /**! デストラクタ呼び出し時にコンソール出力するか */
    bool console_out_;

public:

    /**! コンストラクタ 計測用タイマーとして利用 */
    explicit ProcessTimer(const bool& console_out, const std::string& name) 
     : console_out_(console_out), base_time_(system_clock::now())
    {
        /**! タイマー名を設定しない場合の識別用ID */
        static int unnamed_timer_id = 0;

        if(name.size()>0) 
            name_ = name;
        else 
            name_ = "Timer_" + std::to_string(unnamed_timer_id++);
        
        if(console_out)
            std::cout << name_ << " : start      " << DateTime::get_iso(base_time_) << std::endl;
    }

    ~ProcessTimer()
    {
        if(console_out_)
        {
            std::cout << name_ << " : terminated " << DateTime::get_iso() << std::endl; 
            std::cout << "latest mesurement time " << measure() << " milli second." << std::endl;
        }
    }

    /**! */
    void restart()
    {
        base_time_ = system_clock::now();
        if(console_out_)
            std::cout << name_ << " : restart    " << DateTime::get_iso(base_time_) << std::endl;
    }

    /**! */
    int64_t measure(const enum DateTime::Unit& unit=DateTime::MILLI_SECOND)
    {
        using std::chrono::duration_cast;

        auto current_time = system_clock::now();
        if(unit==DateTime::MILLI_SECOND) 
            return duration_cast<std::chrono::milliseconds>((current_time - base_time_)).count();
        else if(unit==DateTime::SECOND) 
            return duration_cast<std::chrono::seconds>((current_time - base_time_)).count();
        else if(unit==DateTime::MINUTE) 
            return duration_cast<std::chrono::minutes>((current_time - base_time_)).count();
        else if(unit==DateTime::HOUR) 
            return duration_cast<std::chrono::hours>((current_time - base_time_)).count();
        else 
            throw std::runtime_error("Input unit name is unknown.");
    }
};

} // Utility


#endif
