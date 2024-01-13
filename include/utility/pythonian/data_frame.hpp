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

class DataFrame final
{
private:
    std::vector<std::string> header_;
    std::vector<std::vector<std::string>> data_;

    static std::string concat(const std::vector<std::string>& origin, const char& separator=',') noexcept
    {
        std::string result;
        for (const auto& str : origin)
        {
            result += str + separator;
        }
        result.pop_back(); // erase separator character placed in end.
        return result;
    }

    static std::vector<std::string> split(const std::string& origin, const char& separator=',') noexcept
    {
        std::vector<std::string> result;
        std::string element;
        std::istringstream iss(origin);
        while(std::getline(iss, element, separator))
        {
            result.push_back(element);
        }
        return result;
    }

    explicit DataFrame(const std::vector<std::string>& header, const std::vector<std::vector<std::string>>& data)
     : header_(header), data_(data)
    {}

public:

    static DataFrame read_csv(const std::string& file_path, const bool& header=true, const char& separator=',')
    {
        std::vector<std::string> header_row;
        std::vector<std::vector<std::string>> data;

        std::ifstream ifs(file_path);
        if(!ifs) 
            throw std::runtime_error("file doesn't exist.");

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
            throw std::runtime_error("file path doesn't exist.");

        // writing header
        if(header)
            ofs << concat(header_, separator) << std::endl;
        
        // writing each row data.
        // if Windows environment, write CRLF.
        // if Unix like environment, write LF.
        std::string s;
        for (const auto& row : data_)
        {
#ifndef __unix__
            s += concat(row) + '\r' + '\n';
#else
            s += concat(row) + '\n';
#endif
        }
        s.pop_back(); // erase latest '\n'
#ifndef __unix__
        s.pop_back(); // erase latest '\r'
#endif
        ofs << s;
    }

    DataFrame operator[](const std::string& target_column) const
    {
        auto itr = std::find(header_.begin(), header_.end(), target_column);
        if (itr==header_.end())
            throw std::runtime_error("target column was not found.");
        int index = std::distance(header_.begin(), itr);

        std::vector<std::string> header = {header_.at(index)};
        std::vector<std::vector<std::string>> data;

        for(const auto& row : data_)
        {
            data.push_back({row.at(index)});
        }

        DataFrame df(header, data);
        return df;
    }

    DataFrame operator[](const std::vector<std::string>& target_columns) const
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

        std::vector<std::string> header;
        for(const auto& index : indices)
            header.push_back(header_.at(index));

        std::vector<std::string> row_data;
        std::vector<std::vector<std::string>> data;
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
            throw std::out_of_range("index number was out of range");
        
        std::vector<std::vector<std::string>> data = {data_.at(index)};
        DataFrame df(header_, data);

        return df;
    }

    DataFrame operator[](const std::pair<int, int>& range) const
    {
        int start_index, end_index;
        start_index = range.first >= 0 ? range.first : data_.size() + range.first;
        end_index = range.second >= 0 ? range.second : data_.size() + range.second;
        if (start_index < 0 || start_index >= data_.size())
            throw std::out_of_range("start index number was out of range");
        if (end_index < 0 || end_index >= data_.size())
            throw std::out_of_range("end index number was out of range");
        if (start_index > end_index)
            throw std::out_of_range("end index must be larger than start index.");
        
        std::vector<std::vector<std::string>> data;
        for(auto& i=start_index;i<end_index;i++)
            data.push_back(data_.at(i));

        DataFrame df(header_, data);
        return df;
    }

    DataFrame operator[](const std::initializer_list<std::string>& target_columns) const
    {
        std::vector<std::string> v(target_columns.begin(), target_columns.end());
        return this->operator[](v);
    }

    DataFrame& rename(const std::initializer_list<std::string>& header)
    {
        std::vector<std::string> v(header);
        this->rename(v);
        return *this;
    }

    DataFrame& rename(const std::vector<std::string> header)
    {
        if(header.size()!=header_.size())
            throw std::runtime_error("header size is different");
        header_ = header;
        return *this;
    }

    void show() const
    {
        std::cout << concat(header_) << std::endl;
        for (const auto& row : data_)
        {
            std::cout << concat(row) << std::endl;
        }
    }

    void describe() const noexcept
    {
        std::cout << "header names: {" << concat(header_) << "}" << std::endl;
        std::cout << "    row size: " << data_.size() << std::endl;
        std::cout << " column size: " << header_.size() << std::endl;
    }

    template<typename T>
    std::vector<T> to_vector() const
    {
        if(header_.size()!=1)
            throw std::runtime_error("to_vector method can be used 1 column DataFrame only.");

        std::vector<T> result;
        std::stringstream ss;
        for(const auto& row : data_)
        {
            T element;
            ss << row.at(0);
            ss >> element;
            result.push_back(element);
            ss.clear();
        }

        return result;
    }

};

#else

#endif

#endif // _DATA_FRAME_HPP_