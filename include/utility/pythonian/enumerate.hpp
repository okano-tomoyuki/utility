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

template<typename Iter>
Enumerate<Iter> enumerate(Iter&& iter)      { return {std::forward<Iter>(iter)}; }

template<typename Iter>
Enumerate<Iter> enumerate(const Iter& iter) { return {std::forward<Iter>(iter)};}

} // Utility

#endif // _UTILITY_ENUMERATE_HPP_
