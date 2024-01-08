#include <chrono>
#include <string>
#include <sstream>
#include <iostream>
#include <thread>

#include "utility/process_timer.hpp"

using namespace Utility;

ProcessTimer::ProcessTimer(const int& interval) 
 :  interval_(interval), name_(std::string()), base_time_(system_clock::now())
{
}

ProcessTimer::ProcessTimer(const std::string& name) 
 :  interval_(0), name_(name), base_time_(system_clock::now())
{
}

ProcessTimer::~ProcessTimer()
{
    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
        
    if(interval_>0)
    {
        auto waste = milliseconds{interval_ - duration_cast<milliseconds>(system_clock::now()-base_time_).count()};
        std::this_thread::sleep_for(waste);
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