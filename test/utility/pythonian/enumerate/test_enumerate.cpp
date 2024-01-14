#include <vector>
#include <string>
#include <iostream>

#include "utility/pythonian/enumerate.hpp"

struct Sample
{
    int i;
    double d;
    std::string s;
};

int main()
{
    using Utility::enumerate;

    auto sample_list = std::vector<Sample>(10, Sample());

    for(auto&& sample_pair : enumerate(sample_list))
    {
        auto& index = sample_pair.first;
        auto& value = sample_pair.second;
        value.i = index;
        value.d = 1.1 * index;
        value.s = std::to_string(index);
    }

    for(const auto& sample_pair : enumerate(sample_list))
    {
        const auto& index = sample_pair.first;
        const auto& value = sample_pair.second;
        std::cout << "index : "     << index << std::endl;
        std::cout << "vale  : {" 
                  << "\"i\" : "     << value.i
                  << ", \"d\" : "   << value.d 
                  << ", \"s\" : "   << "\"" << value.s << "\"" 
                  << "}"            << std::endl;
    }

#if __cplusplus >= 201703UL

    for(const auto& [index, value] : enumerate(sample_list))
    {
        std::cout << "index : "     << index << std::endl;
        std::cout << "vale  : {" 
                  << "\"i\" : "     << value.i 
                  << ", \"d\" : "   << value.d 
                  << ", \"s\" : "   << "\"" << value.s << "\"" 
                  << "}"            << std::endl;
    }

#endif

}