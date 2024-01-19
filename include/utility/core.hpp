/**
 * @file   core.hpp
 * @author okano tomoyuki (tomoyuki.okano@tsuneishi.com)
 * @brief  利用頻度の高い関数を定義するためのヘッダー
 * @version 0.1
 * @date 2024-01-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _UTILITY_CORE_HPP_
#define _UTILITY_CORE_HPP_

#include <numeric>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>
#include <typeinfo>
#include <algorithm>
#include <iostream>

#ifdef __unix__
#include <unistd.h>
#include <cxxabi.h>
#else
#include <windows.h>
#endif

/**
 * @namespace Utility
 * @brief     インフラストラクチャー層を中心に関係各者が再利用できるクラス、関数を定義する名前空間
 * 
 */
namespace Utility
{

double radians(const double& value) noexcept
{
    return value * M_PI / 180.;
}

double degrees(const double& value) noexcept
{
    return value * 180. / M_PI;
}

double meter_per_seconds(const double& value) noexcept
{
    return value * 1852. / 3600.;
}

double knots(const double& value) noexcept
{
    return value * 3600. / 1852.;
}

template<typename T>
T limit(const T& value, const T& limit) noexcept
{
    return std::max(std::min(value, limit), -limit);
}

template<typename T>
T limit(const T& value, const T& lower_limit, const T& upper_limit) noexcept
{
    return std::max(std::min(value, upper_limit), lower_limit);
}

double adjust_pi(const double& value) noexcept
{
    double result = value;
    while(result>M_PI)
        result -= 2*M_PI;
    while(result<-M_PI)
        result += 2*M_PI;
    return result;
}

double adjust_180(const double& value) noexcept
{
    double result = value;
    while(result>180.)
        result -= 360.;
    while(result<-180.)
        result += 360.;
    return result;
}

template<typename T>
std::string concat(const std::vector<T>& origin, const std::string& separator) noexcept
{
    std::string result;
    for (const auto& str : origin)
        result += std::to_string(str) + separator;
    std::size_t separator_size = separator.size();
    while(separator_size--)
         result.pop_back(); // erase separator character placed in end.   
    return result;
}

std::vector<std::string> split(const std::string& origin, const std::string& separator)
{
    if (origin.empty())
        return {};
        
    if (separator.empty())
        return {origin};

    std::vector<std::string> result;
    std::size_t separator_size = separator.size();
    std::size_t find_start = 0;

    while (true)
    {
        // セパレータ文字列の先頭要素発見位置を取得
        std::size_t find_position = origin.find(separator, find_start);
        
        // 末尾まで先頭要素なし
        if (find_position == std::string::npos)
        {
            result.emplace_back(origin.begin() + find_start, origin.end());
            break;
        }
  
        // 文字列を切り出しコンテナに追加
        result.emplace_back(origin.begin() + find_start, origin.begin() + find_position);
        find_start = find_position + separator_size;
    }
    return result;
}

std::vector<int> range(const int& end) noexcept
{
    std::vector<int> result(end, 0);
    std::iota(result.begin(), result.end(), 0);
    return result;
}

std::vector<int> range(const int& start, const int& end, const int& interval=1) noexcept
{
    std::vector<int> result((std::ceil(static_cast<float>(end-start)/interval)), 0);
    std::iota(result.begin(), result.end(), 0);
    for (auto& i : result)
    {
        i = start + interval * i;
    }
    return result;
}

template<typename T> 
void show(T e) noexcept
{
    std::cout << e << std::endl;
}

template<typename T> 
void show(const std::vector<T>& v) noexcept
{
    std::cout << "{" << concat(v) << "}" << std::endl;
}

template<typename T> 
void show(const std::vector<std::vector<T>>& vv) noexcept
{ 
    std::string s; 
    for(const auto& v : vv)
        s += " {" + concat(v) + "},\n";
    s.pop_back();
    s.pop_back();
    std::cout << "{\n" << s << "\n}" << std::endl;
}

std::string zero_fill(const int& number, const int& digits) noexcept
{
    std::string number_str = std::to_string(number);
    return std::string(std::max(0, digits-static_cast<int>(number_str.size())), '0') + number_str;
}

template<typename T>
std::string get_type(const T& t)
{
#ifdef __unix__
    int status;
    return std::string(abi::__cxa_demangle(typeid(t).name(), 0, 0, &status));
#else // Windows
    return std::string(typeid(t).name());
#endif
}

void sleep(const int& milli_second) noexcept
{
#ifdef __unix__
    usleep(1000*milli_second);
#else
    Sleep(milli_second);
#endif
}

template<typename T>
bool is_exist(const T& val, const std::vector<T>& list)
{
    return std::find(list.begin(), list.end(), val) != list.end();   
}

} // Utility

#endif // _UTILITY_CORE_HPP_
