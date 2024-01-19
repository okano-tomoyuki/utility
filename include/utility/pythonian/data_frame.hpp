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

/**
 * @class DataFrame
 * @brief Pythonにおける表形式データハンドリング用ライブラリPandasの代替ライブラリ
 * 
 * 
 */
class DataFrame final
{

private:
    std::vector<std::string> header_;
    std::vector<std::vector<std::string>> data_;
#ifdef __unix__
    std::string delim_ = "\n";
#else
    std::string delim_ = "\r\n";
#endif
    static std::string concat(const std::vector<std::string>& origin, const char& separator=',');
    static std::vector<std::string> split(const std::string& origin, const char& separator=',');
    explicit DataFrame(const std::vector<std::string>& header, const std::vector<std::vector<std::string>>& data)
     : header_(header), data_(data)
    {}

public:
    enum Axis   { COLUMN, ROW    };
    enum Format { SIMPLE, PRETTY };

    /**
     * @fn read_csv
     * @brief CSV読取メソッド (Factory Method)
     * 
     * @param std::string file_path csvのファイルパス
     * @param bool header ヘッダー有無
     * @param char separator 要素の分割文字 
     * @return DataFrame 読取後DataFrameインスタンス
     */
    static DataFrame read_csv(const std::string& file_path, const bool& header=true, const char& separator=',');

    /**
     * @fn operator=
     * @brief コピー
     * 
     * @param DataFrame other コピー対象のDataFrameインスタンス 
     */
    void operator=(const DataFrame& other);

    /**
     * @fn to_csv
     * @brief csvファイルへの書込メソッド
     * 
     * @param std::stirng file_path 書込先ファイルパス 
     * @param append 追記モードか、上書きモードか
     * @param header ヘッダーを出力データに含めるか
     * @param separator 区切り文字
     */
    void to_csv(const std::string& file_path, const bool& append=true, const bool& header=true, const char& separator=',') const;

    /**
     * @fn operator[]
     * @brief 列名によるDataFrameの切出メソッド
     * 
     * @param std::stirng target_column 取得対象の列名
     * @return DataFrame 切出処理後の新たなDataFrameインスタンス
     */
    DataFrame operator[](const std::string& target_column) const;

    /**
     * @fn operator[]
     * @brief 列名によるDataFrameの切出メソッド
     * 
     * @param std::vector<std::stirng> target_column_list 取得対象の列名のリスト
     * @return DataFrame 切出処理後の新たなDataFrameインスタンス
     */
    DataFrame operator[](const std::vector<std::string>& target_column_list) const;

    /**
     * @fn operator[]
     * @brief 行インデックスによるDataFrameの切出メソッド
     * 
     * @param std::stirng target_row 取得対象の行インデックス
     * @return DataFrame 切出処理後の新たなDataFrameインスタンス
     * @note 負数を指定した場合の取得対象行のインデックスはpandasの仕様に従う。
     * @n    --例--  df[-1] == df[0] ... true
     */
    DataFrame operator[](const int& target_row) const;

    /**
     * @fn operator[]
     * @brief 行インデックスの開始・終了指定によるDataFrameの切出メソッド
     * 
     * @param std::pair<int, int> range 開始インデックス・終了インデックス 
     * @return DataFrame 切出処理後の新たなDataFrameインスタンス
     */
    DataFrame operator[](const std::pair<int, int>& range) const;

    /**
     * @fn operator[]
     * @brief 列名によるDataFrameの切出メソッド
     * 
     * @param std::initializer_list<std::stirng> target_column_list 取得対象の列名のリスト
     * @return DataFrame 切出処理後の新たなDataFrameインスタンス
     */
    DataFrame operator[](const std::initializer_list<std::string>& target_column_list) const;

    /**
     * @fn rename
     * @brief 列名のリネームメソッド
     * 
     * @param std::initializer_list<std::stirng> header リネーム後のヘッダー名のリスト 
     * @return DataFrame& リネーム後の自身のインスタンス
     */
    DataFrame& rename(const std::initializer_list<std::string>& header);

    /**
     * @fn rename
     * @brief 列名のリネームメソッド
     * 
     * @param std::vector<std::stirng> header リネーム後のヘッダー名のリスト 
     * @return DataFrame& リネーム後の自身のインスタンス
     */
    DataFrame& rename(const std::vector<std::string> header);

    /**
     * @fn data
     * @brief 2次元ベクトルのGetterメソッド
     *  
     * @return std::vector<std::vector<std::string> data 
     */   
    std::vector<std::vector<std::string>> data() const;

    /**
     * @fn show
     * @brief ヘッダー部・データ部の表示メソッド
     */ 
    void show(const enum Format& format=SIMPLE) const;


    /**
     * @fn describe
     * @brief メタ情報取得表示メソッド
     */ 
    void describe() const;

    /**
     * @fn to_vector
     * @brief 列・行いずれかを指定の型の1次元ベクターに変換するメソッド
     * 
     * @tparam T 
     * @param enum Axis axis 行・列 { ROW, COLUMN } 
     * @return std::vector<T> 
     */
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

private:
    std::vector<std::string>  header_;
    std::vector<std::vector<std::string>> data_;
#ifdef __unix__
    std::string delim_ = "\n";
#else
    std::string delim_ = "\r\n";
#endif

    static std::string concat(const std::vector<std::string>& origin, const char& separator=',')
    {
        std::string result;
        for (const auto& str : origin)
            result += str + separator;
        result.pop_back(); // erase separator character placed in end.
        return result;
    }

    static std::vector<std::string> split(const std::string& origin, const char& separator=',')
    {
        std::vector<std::string> result;
        std::string element;
        std::istringstream iss(origin);
        while(std::getline(iss, element, separator))
            result.push_back(element);
        return result;
    }

    explicit DataFrame(const std::vector<std::string>& header, const std::vector<std::vector<std::string>>& data)
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
        std::vector<std::string> header = {header_.at(index)};
        std::vector<std::vector<std::string>> data;

        for(const auto& row : data_)
            data.push_back({row.at(index)});

        DataFrame df(header, data);
        return df;
    }

    DataFrame operator[](const std::vector<std::string>& target_column_list) const
    {
        std::vector<int> indices;
        for (const auto& column : target_column_list)
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
            throw std::out_of_range("index number [" + std::to_string(target_row) + "] was out of range");
        
        std::vector<std::vector<std::string>> data = {data_.at(index)};
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
        
        std::vector<std::vector<std::string>> data;
        for(auto& i=start_index;i<end_index;i++)
            data.push_back(data_.at(i));

        DataFrame df(header_, data);
        return df;
    }

    DataFrame operator[](const std::initializer_list<std::string>& target_column_list) const
    {
        std::vector<std::string> v(target_column_list.begin(), target_column_list.end());
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

    std::vector<std::vector<std::string>> data() const
    {
        return data_;
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