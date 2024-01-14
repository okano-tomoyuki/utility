/**
 * @file utility/pythonian/data_frame.hpp
 * @author okano tomoyuki (okano.development@gmail.com)
 * @brief class library for python's data frame handling.
 * @version 0.1
 * @date 2024-01-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _DATA_FRAME_HPP_
#define _DATA_FRAME_HPP_

#include <vector>               // std::vector
#include <string>               // std::string, std::getline
#include <fstream>              // std::ifstream, std::ofstream
#include <sstream>              // std::sstream, std::istringstream
#include <stdexcept>            // std::runtime_error, std::out_of_range
#include <iostream>             // std::cout, std::endl
#include <algorithm>            // std::find
#include <initializer_list>     // std::initilizer_list
#include <utility>              // std::tuple

#ifdef GLOBAL_USE_BUILD_LIBLARY

namespace Utility
{

class DataFrame final
{
using VS  = std::vector<std::string>;
using VVS = std::vector<VS>;
private:
    VS header_;
    VVS data_;
    static std::string concat(const VS& origin, const char& separator=',');
    static VS split(const std::string& origin, const char& separator=',');
    explicit DataFrame(const VS& header, const VVS& data)
     : header_(header), data_(data)
    {}

public:
    enum Axis   { COLUMN, ROW    };
    enum Format { SIMPLE, PRETTY };
    static DataFrame read_csv(const std::string& file_path, const bool& header=true, const char& separator=',');
    void operator=(const DataFrame& other);
    void to_csv(const std::string& file_path, const bool& append=true, const bool& header=true, const char& separator=',') const;
    DataFrame operator[](const std::string& target_column) const;
    DataFrame operator[](const VS& target_columns) const;
    DataFrame operator[](const int& target_row) const;
    DataFrame operator[](const std::pair<int, int>& range) const;
    DataFrame operator[](const std::initializer_list<std::string>& target_columns) const;
    DataFrame& rename(const std::initializer_list<std::string>& header);
    DataFrame& rename(const VS header);
    void show(const enum Format& format=SIMPLE) const;
    void describe() const;

    template<typename T>
    std::vector<T> to_vector(const enum Axis& axis=COLUMN) const
    {
        if(axis==ROW && data_.size()!=1)
            throw std::runtime_error("to_vector method can be used to 1 raw DataFrame only.");
        if(axis==COLUMN && header_.size()!=1)
            throw std::runtime_error("to_vector method can be used to 1 column DataFrame only.");
        
        T element;
        std::vector<T> result;
        std::stringstream ss;

        if(axis==COLUMN)
        {
            for(const auto& row : data_)
            {
                ss << row.at(0);
                ss >> element;
                result.push_back(element);
                ss.clear();
            }
        }
        else // ROW
        {
            for(const auto& e : data_.at(0))
            {
                ss << e;
                ss >> element;
                result.push_back(element);
                ss.clear();
            }
        }
        return result;
    }
};

}

#else

namespace Utility
{

class DataFrame final
{

using VS  = std::vector<std::string>;
using VVS = std::vector<VS>;

private:
    VS  header_;
    VVS data_;
#ifdef __unix__
    std::string delim_ = "\n";
#else
    std::string delim_ = "\r\n";
#endif

    static std::string concat(const VS& origin, const char& separator=',')
    {
        std::string result;
        for (const auto& str : origin)
            result += str + separator;
        result.pop_back(); // erase separator character placed in end.
        return result;
    }

    static VS split(const std::string& origin, const char& separator=',')
    {
        VS result;
        std::string element;
        std::istringstream iss(origin);
        while(std::getline(iss, element, separator))
            result.push_back(element);
        return result;
    }

    explicit DataFrame(const VS& header, const VVS& data)
     : header_(header), data_(data)
    {}

public:
    enum Axis   { COLUMN, ROW    };
    enum Format { SIMPLE, PRETTY };

    void operator=(const DataFrame& other)
    {
        header_ = other.header_;
        data_   = other.data_;
    }

    static DataFrame read_csv(const std::string& file_path, const bool& header=true, const char& separator=',')
    {
        VS header_row;
        VVS data;

        std::ifstream ifs(file_path);
        if(!ifs)
            throw std::runtime_error("file \"" + file_path + "\" doesn't exist.");

        std::string line;
        std::getline(ifs, line);
        auto first_row = split(line, separator);

        // switching header on/off.
        if(header)
            header_row = first_row;
        else
            for(auto i=0;i<first_row.size();i++) 
                header_row.push_back(std::to_string(i));

        // retrieving each row of data.
        while(std::getline(ifs, line))
        {
            auto row = split(line, separator);
            if(row.size() != header_row.size()) 
                throw std::runtime_error("element size between header and column is different.");
            data.push_back(row);
        }

        // create DataFrame instance.
        DataFrame df(header_row, data);
        return df;
    }

    void to_csv(const std::string& file_path, const bool& append=true, const bool& header=true, const char& separator=',') const
    {
        std::ofstream ofs;
        append ? ofs.open(file_path, std::ios::app) : ofs.open(file_path); // switching append or overwrite.
        if(!ofs) 
            throw std::runtime_error("file path \"" + file_path + "\" doesn't exist.");

        // writing header
        if(header)
            ofs << concat(header_, separator) << std::endl;
        
        // writing each row data.
        // if Windows environment, write CRLF.
        // if Unix like environment, write LF.
        std::stringstream ss;
        for (const auto& row : data_)
            ss << concat(row, separator) << delim_;
        
        std::string result = ss.str();
        int pop_size = delim_.size();
        while(pop_size--)
            result.pop_back();

        ofs << result;
    }

    DataFrame operator[](const std::string& target_column) const
    {
        auto itr = std::find(header_.begin(), header_.end(), target_column);
        
        if (itr==header_.end())
        {
            std::stringstream ss;
            ss << "target column \"" << target_column << "\" was not found.";
            throw std::runtime_error(ss.str());
        }

        int index = std::distance(header_.begin(), itr);
        VS header = {header_.at(index)};
        VVS data;

        for(const auto& row : data_)
            data.push_back({row.at(index)});

        DataFrame df(header, data);
        return df;
    }

    DataFrame operator[](const VS& target_columns) const
    {
        std::vector<int> indices;
        for (const auto& column : target_columns)
        {
            auto itr = std::find(header_.begin(), header_.end(), column);
            if (itr==header_.end())
                throw std::runtime_error("target column was not found.");
            int index = std::distance(header_.begin(), itr);
            indices.push_back(index);
        }

        VS header;
        for(const auto& index : indices)
            header.push_back(header_.at(index));

        VS row_data;
        VVS data;
        for(const auto& row : data_)
        {
            row_data.clear();
            for(const auto& index : indices)
                row_data.push_back(row.at(index));
            data.push_back(row_data);
        }

        DataFrame df(header, data);
        return df;
    }

    DataFrame operator[](const int& target_row) const
    {
        int index;
        index = target_row >= 0 ? target_row : data_.size() + target_row;
        if (index < 0 || index >= data_.size())
            throw std::out_of_range("index number [" + std::to_string(target_row) + "] was out of range");
        
        VVS data = {data_.at(index)};
        DataFrame df(header_, data);
        return df;
    }

    DataFrame operator[](const std::pair<int, int>& range) const
    {
        int start_index = (range.first  >= 0) ? range.first  : data_.size() + range.first;
        int end_index   = (range.second >= 0) ? range.second : data_.size() + range.second;
        if (start_index < 0 || start_index >= data_.size())
            throw std::out_of_range("start index number was out of range");
        if (end_index   < 0 || end_index   >= data_.size())
            throw std::out_of_range("end index number was out of range");
        if (start_index > end_index)
            throw std::out_of_range("end index must be larger than start index.");
        
        VVS data;
        for(auto& i=start_index;i<end_index;i++)
            data.push_back(data_.at(i));

        DataFrame df(header_, data);
        return df;
    }

    DataFrame operator[](const std::initializer_list<std::string>& target_columns) const
    {
        VS v(target_columns.begin(), target_columns.end());
        return this->operator[](v);
    }

    DataFrame& rename(const std::initializer_list<std::string>& header)
    {
        VS v(header);
        this->rename(v);
        return *this;
    }

    DataFrame& rename(const VS header)
    {
        if(header.size()!=header_.size())
            throw std::runtime_error("header size is different");
        header_ = header;
        return *this;
    }

    void show(const enum Format& format=SIMPLE) const
    {
        std::cout << concat(header_) << std::endl;
        for (const auto& row : data_)
            std::cout << concat(row) << std::endl;
    }

    void describe() const
    {
        std::cout << "header names: {" << concat(header_) << "}" << std::endl;
        std::cout << "    row size: " << data_.size() << std::endl;
        std::cout << " column size: " << header_.size() << std::endl;
    }

    template<typename T>
    std::vector<T> to_vector(const enum Axis& axis=COLUMN) const
    {
        if(axis==ROW && data_.size()!=1)
            throw std::runtime_error("to_vector method can be used to 1 raw DataFrame only.");
        if(axis==COLUMN && header_.size()!=1)
            throw std::runtime_error("to_vector method can be used to 1 column DataFrame only.");
        
        T element;
        std::vector<T> result;
        std::stringstream ss;

        if(axis==COLUMN)
        {
            for(const auto& row : data_)
            {
                ss << row.at(0);
                ss >> element;
                result.push_back(element);
                ss.clear();
            }
        }
        else // ROW
        {
            for(const auto& e : data_.at(0))
            {
                ss << e;
                ss >> element;
                result.push_back(element);
                ss.clear();
            }
        }
        return result;
    }
};

}

#endif

#endif // _DATA_FRAME_HPP_