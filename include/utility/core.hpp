/**
 * @file core.hpp
 * @author okano tomoyuki (tomoyuki.okano@tsuneishi.com)
 * @brief 再利用性の高い関数・定数群定義ヘッダー
 * @version 0.1
 * @date 2024-01-19
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

#ifdef __unix__
#include <unistd.h>
#include <cxxabi.h>
#else
#include <windows.h>
#endif

/**
 * @namespace Utility
 * @brief 再利用性の高い定数・関数・クラス等を管理する名前空間
 * 
 */
namespace Utility
{

/** 海水密度 [kgf s^2/m^4] */
static constexpr double RHO_WATER = 104.61;

/** 空気密度 [kgf s^2/m^4] */
static constexpr double RHO_AIR = 0.125;

/** 重力加速度 [m/s^2] */
static constexpr double GRAVITY = 9.80665;

/** 1海里 1852.0[m] */
static constexpr double NAUTICAL_MILE = 1852.0;

/**
 * @fn radians(const double&)
 * @brief degree => radian への変換関数
 * @param double value : degree 単位 の値
 * @return value を radian 単位に変換した値
*/
double radians(const double& value) noexcept
{
    return value * M_PI / 180.;
}

/**
 * @fn degrees(const double&)
 * @brief radian => degree への変換関数
 * @param double value : radian 単位 の値
 * @return value を degree 単位に変換した値
*/
double degrees(const double& value) noexcept
{
    return value * 180. / M_PI;
}

/**
 * @fn meter_per_seconds(const double&)
 * @brief knot => m/s への変換関数
 * @param double value : knot 単位 の値
 * @return value を m/s 単位に変換した値
*/
double meter_per_seconds(const double& value) noexcept
{
    return value * 1852. / 3600.;
}

/**
 * @fn knots(const double&)
 * @brief m/s => knot への変換関数
 * @param double value : m/s単位 の値
 * @return valueをknot単位に変換した値
*/
double knots(const double& value) noexcept
{
    return value * 3600. / 1852.;
}

/**
 * @fn template<type T> sgn(const T&)
 * @brief 符号取得関数
 * @param double value : 符号を取得したい変数
 * @return valueの符号(値が0の場合のみ0を返却する。)
*/
template<typename T>
T sgn(const T& value)
{
    return (value>0)-(value<0);
}

/**
 * @fn template<typename T> limit(const T&, const T&)
 * @brief limit関数
 * @param double value : 制限前の引数
 * @param limit_value : 閾値
 * @return value が ±limit_value 以内に制限する。(範囲内であった場合そのままの値を出力する。)
*/
template<typename T>
T limit(const T& value, const T& limit_value) noexcept
{
    return std::max(std::min(value, std::abs(limit_value)), -std::abs(limit_value));
}

/**
 * @fn template<typename T> limit(const T&, const T&, const T&)
 * @brief limit関数
 * @param double value : 制限前の引数
 * @param limit_value1 : 閾値(片側)
 * @param limit_value2 : 閾値(片側)
 * @return value が limit_value1 ~ limit_value2 以内に制限する。(範囲内であった場合そのままの値を出力する。)
 * @note 引数の順序間違いの可能性を考慮し、limit_value1, limit_value2の大小関係が逆順でも同様の結果が得られるようにする。
*/
template<typename T>
T limit(const T& value, const T& limit_value1, const T& limit_value2) noexcept
{
    if(limit_value1 < limit_value2)
        return std::max(std::min(value, limit_value2), limit_value1);
    else
        return std::max(std::min(value, limit_value1), limit_value2);
}

/**
 * @fn adjust_pi(const double& )
 * @brief -π~π へのレンジ調整関数
 * @param double value : 単位 radian　のデータ
 * @return value が -π ~ π のレンジの値になった値
*/
double adjust_pi(const double& value) noexcept
{
    double result = value;
    while(result>M_PI)
        result -= 2*M_PI;
    while(result<-M_PI)
        result += 2*M_PI;
    return result;
}

/**
 * @fn adjust_180(const double&)
 * @brief -180° ~ 180° へのレンジ調整関数
 * @param double value : 単位 degree　のデータ
 * @return value が -180 ~ 180 のレンジの値になった値
*/
double adjust_180(const double& value) noexcept
{
    double result = value;
    while(result>180.)
        result -= 360.;
    while(result<-180.)
        result += 360.;
    return result;
}

/**
 * @fn template<typename T> concat(const std::vector<T>&, const std::string&)
 * @brief vectorデータからの文字列連結関数
 * @param origin : 要素がプリミティブな型であるvectorデータ
 * @param separator : 文字列データにして連結する際の区切り文字列
 * @return origin の各要素を separator でつないだ文字列
*/
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

/**
 * @fn split(const std::string& origin, const std::string&)
 * @brief 文字列分割関数
 * @param origin : 分割前文字列
 * @param separator : 区切り文字列
 * @return origin を separator で分割し、各要素としたvectorデータ
*/
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

/**
 * @fn range(const int&)
 * @brief Pythonにおけるrange関数の代替
 * @note 範囲ベースforループで使用
*/
std::vector<int> range(const int& end) noexcept
{
    std::vector<int> result(end, 0);
    std::iota(result.begin(), result.end(), 0);
    return result;
}

/**
 * @fn range(const int&, const int&, const int&)
 * @brief Pythonにおけるrange関数の代替
 * @note 範囲ベースforループで使用
*/
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

/**
 * @fn template<typename T> get_type(const T&)
 * @brief 型名取り出し用関数
 * @param t : 型名を取得したい変数
 * @return t の型名の文字列
*/
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

/**
 * @fn sleep(const int&)
 * @brief スリープ関数
 * @note 休止関数はWindows/Linuxともに存在するが動作仕様が異なるため、共通でこの関数を使用する。
*/
void sleep(const int& milli_second) noexcept
{
#ifdef __unix__
    usleep(1000*milli_second);
#else
    Sleep(milli_second);
#endif
}

/**!
 * @fn template<typename T> is_exist(const T& value, const std::vector<T>&)
 * @brief vectorデータ内に指定した要素があるか確認する関数
 * @param double value : 存在するか確認したいデータ
 * @param list  : 確認対象のリスト
 * @return true - list内にvalue有, false - list内にvalue無
*/
template<typename T>
bool is_exist(const T& value, const std::vector<T>& list)
{
    return std::find(list.begin(), list.end(), value) != list.end();   
}

/**
 * @fn template<typename T> is_in_range(const T&, const T&)
 * @brief 値が指定した閾値以内に入っているか確認する関数
 * @param double value : 確認対象の値
 * @return true - valueが -std::abs(limit_value) ~ std::abs(limit_value) の範囲内, false - 範囲外
*/
template<typename T>
bool is_in_range(const T& value, const T& limit_value)
{
    return (limit(value, limit_value) != value);
}

/**
 * @fn template<typename T> is_in_range(const T&, const T&, const T&)
 * @brief 値が指定した閾値以内に入っているか確認する関数
 * @param double value : 確認対象の値
 * @return true - valueが limit_value1 ~ limit_value2 の範囲内, false - 範囲外
*/
template<typename T>
bool is_in_range(const T& value, const T& limit_value1, const T& limit_value2)
{
    return (limit(value, limit_value1, limit_value2) != value);
}

} // Utility

#endif // _UTILITY_CORE_HPP_