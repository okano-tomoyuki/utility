#include <iostream>

#include "utility/date_time.hpp"
#include "utility/process_timer.hpp"

int main()
{
    using Utility::DateTime;
    using Utility::ProcessTimer;

    std::cout << DateTime().to_str("hh:nn:ss.zzz") << std::endl;
    {
        auto mesurement_timer = ProcessTimer("AAAA");
        auto auto_wait_timer  = ProcessTimer(3000);
    }
    std::cout << DateTime().to_str("hh:nn:ss.zzz") << std::endl;

}