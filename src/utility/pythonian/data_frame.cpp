#include "utility/pythonian/data_frame.hpp"

using namespace Utility;

std::string DataFrame::concat(const std::vector<std::string>& origin, const char& separator)
{
    std::string result;
    for (const auto& str : origin)
        result += str + separator;
    result.pop_back(); // erase separator character placed in end.
    return result;
}

std::vector<std::string> DataFrame::split(const std::string& origin, const char& separator)
{
    std::vector<std::string> result;
    std::string element;
    std::istringstream iss(origin);
    while(std::getline(iss, element, separator))
        result.push_back(element);
    return result;
}

DataFrame::DataFrame(const std::vector<std::string>& header, const std::vector<std::vector<std::string>>& data)
    : header_(header), data_(data)
{}

void DataFrame::operator=(const DataFrame& other)
{
    header_ = other.header_;
    data_   = other.data_;
}

DataFrame DataFrame::read_csv(const std::string& file_path, const bool& header, const char& separator)
{
    std::vector<std::string> header_row;
    std::vector<std::vector<std::string>> data;

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

void DataFrame::to_csv(const std::string& file_path, const bool& append=true, const bool& header, const char& separator) const
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

DataFrame DataFrame::operator[](const std::string& target_column) const
{
    auto itr = std::find(header_.begin(), header_.end(), target_column);
    
    if (itr==header_.end())
    {
        std::stringstream ss;
        ss << "target column \"" << target_column << "\" was not found.";
        throw std::runtime_error(ss.str());
    }

    int index = std::distance(header_.begin(), itr);
    std::vector<std::string> header = {header_.at(index)};
    std::vector<std::vector<std::string>> data;

    for(const auto& row : data_)
        data.push_back({row.at(index)});

    DataFrame df(header, data);
    return df;
}

DataFrame DataFrame::operator[](const std::vector<std::string>& target_columns) const
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

DataFrame DataFrame::operator[](const int& target_row) const
{
    int index;
    index = target_row >= 0 ? target_row : data_.size() + target_row;
    if (index < 0 || index >= data_.size())
        throw std::out_of_range("index number [" + std::to_string(target_row) + "] was out of range");
    
    std::vector<std::vector<std::string>> data = {data_.at(index)};
    DataFrame df(header_, data);
    return df;
}

DataFrame DataFrame::operator[](const std::pair<int, int>& range) const
{
    int start_index = (range.first  >= 0) ? range.first  : data_.size() + range.first;
    int end_index   = (range.second >= 0) ? range.second : data_.size() + range.second;
    if (start_index < 0 || start_index >= data_.size())
        throw std::out_of_range("start index number was out of range");
    if (end_index   < 0 || end_index   >= data_.size())
        throw std::out_of_range("end index number was out of range");
    if (start_index > end_index)
        throw std::out_of_range("end index must be larger than start index.");
    
    std::vector<std::vector<std::string>> data;
    for(auto& i=start_index;i<end_index;i++)
        data.push_back(data_.at(i));

    DataFrame df(header_, data);
    return df;
}

DataFrame DataFrame::operator[](const std::initializer_list<std::string>& target_columns) const
{
    std::vector<std::string> v(target_columns.begin(), target_columns.end());
    return this->operator[](v);
}

DataFrame& DataFrame::rename(const std::initializer_list<std::string>& header)
{
    std::vector<std::string> v(header);
    this->rename(v);
    return *this;
}

DataFrame& DataFrame::rename(const std::vector<std::string> header)
{
    if(header.size()!=header_.size())
        throw std::runtime_error("header size is different");
    header_ = header;
    return *this;
}

void DataFrame::show(const enum Format& format) const
{
    std::cout << concat(header_) << std::endl;
    for (const auto& row : data_)
        std::cout << concat(row) << std::endl;
}

void DataFrame::describe() const
{
    std::cout << "header names: {" << concat(header_) << "}" << std::endl;
    std::cout << "    row size: " << data_.size() << std::endl;
    std::cout << " column size: " << header_.size() << std::endl;
}

std::vector<std::vector<std::string>> DataFrame::data() const
{
    return data_;
}
