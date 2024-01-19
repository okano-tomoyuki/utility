#include <chrono>
#include <string>
#include <sstream>
#include <iostream>
#include <thread>
#include <stdexcept>

#include "utility/process_timer.hpp"

using namespace Utility;

ProcessTimer::ProcessTimer(const std::string& name, const int& interval, const enum WaitType& wait_type) 
 :  name_(std::string()), interval_(interval), wait_type_(wait_type), base_time_(system_clock::now())
{
}

ProcessTimer ProcessTimer::create_auto_wait(const int& interval, const enum WaitType& wait_type=BUSSY)
{
    if(interval<=0)
    {
        std::stringstream ss;
        ss << "Exception throw by " << __func__ << ".";
        ss << "If you use auto wait timer instance, interval value must be larger than 0."; 
        throw std::runtime_error(ss.str());
    }
    return ProcessTimer(std::string(), interval, wait_type);
}

ProcessTimer ProcessTimer::create_mesure(const std::string& name)
{
    return ProcessTimer(name, 0, BUSSY);
}

ProcessTimer::~ProcessTimer()
{
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
        
    if(interval_>0)
    {
        if(wait_type_ == BUSSY)
        {
            while(true)
                if(system_clock::now() >= base_time_ + milliseconds{interval_})
                    break;
        }
        else // wait_type_ == SLEEP
        {
            auto waste = milliseconds{interval_ - duration_cast<milliseconds>(system_clock::now()-base_time_).count()};
            std::this_thread::sleep_for(waste);
        }
    }
    else
        std::cout << name_ << " : mesured time " << measure() << std::endl;
}

void ProcessTimer::restart()
{
    base_time_ = system_clock::now();
}

std::string ProcessTimer::measure() const
{
    using std::chrono::duration_cast;
    using std::chrono::hours;
    using std::chrono::minutes;
    using std::chrono::seconds;
    using std::chrono::milliseconds;
    using std::chrono::microseconds;

    auto current_time = system_clock::now();
    auto pass_hour = duration_cast<hours>((current_time - base_time_)).count();
    auto pass_min  = duration_cast<minutes>((current_time - base_time_)).count();
    auto pass_sec  = duration_cast<seconds>((current_time - base_time_)).count();
    auto pass_msec = duration_cast<milliseconds>((current_time - base_time_)).count();

    std::stringstream ss;
    ss << pass_hour << ":" 
        << pass_min - pass_hour * 60 << ":" 
        << pass_sec - pass_min  * 60 << "."
        << std::string(3-std::to_string(pass_msec-pass_sec*1000).length(),'0') << pass_msec-pass_sec*1000;

    return ss.str();
}