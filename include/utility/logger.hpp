#ifndef _UTILITY_LOGGER_HPP_
#define _UTILITY_LOGGER_HPP_

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <memory>

namespace Utility
{

class Logger final
{

private:
    std::stringstream buffer_;
    std::string separator_;
    std::string new_line_;

public:
    enum Color { WHITE, YELLOW, GREEN, BLUE, RED };

    explicit Logger(const std::string& separator=" ", const std::string& new_line="\n")
     : separator_(separator), new_line_(new_line)
    {}

    template<typename T>
    Logger& add(const T& val) 
    { buffer_ << val; return *this; }

    template<typename T, typename... Remain>
    Logger& add(const T& val, const Remain&... remain) 
    { buffer_ << val << separator_; add(remain...); return *this; }

    template<typename T>
    Logger& add(const std::vector<T>& val)
    {
        std::string result;
        for (const auto& str : val)
            result += std::to_string(str) + ' ';
        result.pop_back();
        buffer_ << "[" << result << "]";
        return *this;
    }

    template<typename T, typename... Remain>
    Logger& add(const std::vector<T>& val, const Remain&... remain)
    {
        std::string result;
        for (const auto& str : val)
            result += std::to_string(str) + ' ';
        result.pop_back();
        buffer_ << "[" << result << "]" << separator_;
        add(remain...);
        return *this;
    }

    template<typename T>
    Logger& add_line(const std::vector<T>& val)
    {
        std::string result;
        for (const auto& str : val)
            result += std::to_string(str) + ' ';
        result.pop_back();
        buffer_ << "[" << result << "]" << new_line_;
        return *this;
    }

    template<typename T, typename... Remain>
    Logger& add_line(const std::vector<T>& val, const Remain&... remain)
    {
        std::string result;
        for (const auto& str : val)
            result += std::to_string(str) + ' ';
        result.pop_back();
        buffer_ << "[" << result << "]" << new_line_;
        add_line(remain...);
        return *this;
    }

    Logger& print(const enum Color& color = WHITE)
    {
        if     ( color ==  WHITE ) std::cout << "\033[m"   << buffer_.str() << "\033[m";
        else if( color == YELLOW ) std::cout << "\033[33m" << buffer_.str() << "\033[m";
        else if( color ==  GREEN ) std::cout << "\033[32m" << buffer_.str() << "\033[m";
        else if( color ==   BLUE ) std::cout << "\033[34m" << buffer_.str() << "\033[m";
        else if( color ==    RED ) std::cout << "\033[31m" << buffer_.str() << "\033[m";
        return *this;
    }

    template<typename... Args>
    static std::string format(const std::string& format, const Args&... args )
    {
        int size_s = std::snprintf(nullptr, 0, format.c_str(), args... ) + 1;
        if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
        auto size = static_cast<size_t>(size_s);
        std::unique_ptr<char[]> buf( new char[ size ] );
        std::snprintf(buf.get(), size, format.c_str(), args...);
        return std::string(buf.get(), buf.get() + size - 1 );
    }

    Logger& flush() 
    { buffer_.str(""); buffer_.clear(std::stringstream::goodbit); return *this; }

    Logger& endl() 
    { buffer_ << new_line_; return *this; }

    template<typename T> 
    Logger& add_line(const T& val) 
    { 
        buffer_ << val << new_line_; 
        return *this; 
    }

    template<typename T, typename... Remain>
    Logger& add_line(const T& val, const Remain&... remain) 
    { 
        buffer_ << val << separator_; 
        add_line(remain...); 
        return *this; 
    }

    Logger& set_separator(const std::string& separator) 
    { 
        separator_ = separator; 
        return *this; 
    }
    
    Logger& set_new_line(const std::string& new_line)   
    { 
        new_line_ = new_line; 
        return *this; 
    }
    
    size_t size() const 
    { return buffer_.str().size(); }
    
    std::string get_buffer() const 
    { return buffer_.str(); }

    Logger& save(const std::string& file_path, const std::ios_base::openmode& mode = std::ios_base::app)
    { std::ofstream ofs; ofs.open(file_path, mode); ofs << buffer_.str(); return *this; }

};

}

#endif
