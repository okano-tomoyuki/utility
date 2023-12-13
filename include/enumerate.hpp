#ifndef _UTILITY_ENUMERATE_HPP_
#define _UTILITY_ENUMERATE_HPP_

#include <iterator>

namespace Utility
{

template<typename Iterable>
class Enumerate
{
private:
    Iterable iter_;
    std::size_t size_;
    decltype(std::begin(iter_)) begin_;
    const decltype(std::end(iter_)) end_;

public:
    Enumerate(Iterable iter):
        iter_(iter),
        size_(0),
        begin_(std::begin(iter)),
        end_(std::end(iter))
    {}

    const Enumerate& begin() const { return *this; }
    const Enumerate& end()   const { return *this; }

    bool operator!=(const Enumerate&) const
    {
        return begin_ != end_;
    }

    void operator++()
    {
        ++begin_;
        ++size_;
    }

    std::pair<std::size_t, decltype(*begin_)> operator*() const
    {
        return { size_, *begin_ };
    }

};

template<typename Iterable>
Enumerate<Iterable> enumerate(Iterable&& iter)
{
    return {std::forward<Iterable>(iter)};
}

template<typename Iterable>
Enumerate<Iterable> enumerate(const Iterable& iter)
{
    return {std::forward<Iterable>(iter)};
}

} // Utility

#endif // _UTILITY_ENUMERATE_HPP_
