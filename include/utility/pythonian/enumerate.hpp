/**
 * @file enumerate.hpp
 * @author okano tomoyuki (tomoyuki.okano@tsuneishi.com)
 * @brief 範囲ベースforループでindexとvalueの両方を取得する @ref Utility::enumerate 関数の定義ヘッダー
 * @version 0.1
 * @date 2024-01-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _UTILITY_ENUMERATE_HPP_
#define _UTILITY_ENUMERATE_HPP_

#include <utility>
#include <iterator>

namespace Utility
{

template<typename Iter>
class Enumerate
{
private:
    Iter iter_;
    std::size_t size_;
    decltype(std::begin(iter_)) begin_;
    const decltype(std::end(iter_)) end_;

public:
    Enumerate(Iter iter)
     :  iter_(iter), size_(0), begin_(std::begin(iter)), end_(std::end(iter))
    {}
    const Enumerate& begin()          const { return *this; }
    const Enumerate& end()            const { return *this; }
    bool operator!=(const Enumerate&) const { return begin_ != end_; }
    void operator++() { ++begin_; ++size_; }
    std::pair<std::size_t, decltype(*begin_)> operator*() const { return { size_, *begin_ }; }
};

/**
 * @fn enumerate
 * @brief 範囲ベースforループでindexとvalueの両方を取得する関数
 * 
 * @tparam Iter 
 * @param iter 
 * @return Enumerate<Iter> 
 */
template<typename Iter>
Enumerate<Iter> enumerate(Iter&& iter)      { return {std::forward<Iter>(iter)}; }

/**
 * @fn enumerate
 * @brief 範囲ベースforループでindexとvalueの両方を取得する関数
 * 
 * @tparam Iter 
 * @param iter 
 * @return Enumerate<Iter> 
 */
template<typename Iter>
Enumerate<Iter> enumerate(const Iter& iter) { return {std::forward<Iter>(iter)};}

} // Utility

#endif // _UTILITY_ENUMERATE_HPP_
