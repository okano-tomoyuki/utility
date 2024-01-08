#include <iostream>

#include "utility/date_time.hpp"

int main()
{
    std::cout << Utility::DateTime("2022/AA/08T01:02:03.456789Z", "yyyy/mm/ddThh:nn:ss.zzzzzzZ").to_str("yyyy/mm/ddThh:nn:ss.zzzzzzZ") << std::endl;
}