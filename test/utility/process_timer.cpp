#include <iostream>

#include "utility/date_time.hpp"
#include "utility/process_timer.hpp"

int main()
{

    std::cout << Utility::DateTime().to_str("hh:nn:ss.zzz") << std::endl;
    {
        auto mesurement_timer = Utility::ProcessTimer("AAAA");
        auto auto_wait_timer  = Utility::ProcessTimer(30000);
    }
    std::cout << Utility::DateTime().to_str("hh:nn:ss.zzz") << std::endl;

}