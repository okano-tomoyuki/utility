#include <iostream>

#include "utility/date_time.hpp"

int main()
{
    using Utility::DateTime;

    // create instance by now (UTC).
    auto dt = DateTime();
    std::cout << "                       year : "  <<                                dt.year()  << std::endl;
    std::cout << "                      month : "  <<                               dt.month()  << std::endl;
    std::cout << "                        day : "  <<                                 dt.day()  << std::endl;
    std::cout << "                       hour : "  <<                                dt.hour()  << std::endl;
    std::cout << "                     minute : "  <<                              dt.minute()  << std::endl;
    std::cout << "                     second : "  <<                              dt.second()  << std::endl;
    std::cout << "               milli second : "  <<                         dt.millisecond()  << std::endl;
    std::cout << "               micro second : "  <<                         dt.microsecond()  << std::endl;

    std::cout << "   yyyy : "   <<                       dt.to_str("yyyy")  << std::endl;
    std::cout << "     mm : "   <<                         dt.to_str("mm")  << std::endl;
    std::cout << "     dd : "   <<                         dt.to_str("dd")  << std::endl;
    std::cout << "     hh : "   <<                         dt.to_str("hh")  << std::endl;
    std::cout << "     nn : "   <<                         dt.to_str("nn")  << std::endl;
    std::cout << "     ss : "   <<                         dt.to_str("ss")  << std::endl;
    std::cout << "    zzz : "   <<                        dt.to_str("zzz")  << std::endl;
    std::cout << " zzzzzz : "   <<                     dt.to_str("zzzzzz")  << std::endl;

    std::cout << " yyyymmdd                   : "   <<                   dt.to_str("yyyymmdd")  << std::endl;
    std::cout << " yyyy/mm/dd                 : "   <<                 dt.to_str("yyyy/mm/dd")  << std::endl;
    std::cout << " yyyy-mm-dd                 : "   <<                 dt.to_str("yyyy-mm-dd")  << std::endl;

    std::cout << " hhnnss                     : "   <<                     dt.to_str("hhnnss")  << std::endl;
    std::cout << " hh:nn:ss                   : "   <<                   dt.to_str("hh:nn:ss")  << std::endl; 
    std::cout << " hh-nn-ss                   : "   <<                   dt.to_str("hh-nn-ss")  << std::endl;

    std::cout << " yyyymmddThhnnss.zzzzzzZ    : "   <<    dt.to_str("yyyymmddThhnnss.zzzzzzZ")  << std::endl;
    std::cout << " yyyy/mm/dd hh:nn:ss.zzzzzz : "   << dt.to_str("yyyy/mm/dd hh:nn:ss.zzzzzz")  << std::endl;


}