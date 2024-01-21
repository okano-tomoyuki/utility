/**
 * @file ini.hpp
 * @author okano tomoyuki (tomoyuki.okano@tsuneishi.com)
 * @brief ini ファイルのパースを行う @ref Utility::Ini クラスの定義ヘッダー
 * @version 0.1
 * @date 2024-01-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _UTILITY_INI_HPP_
#define _UTILITY_INI_HPP_

#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <fstream>
#include <map>

#ifdef GLOBAL_USE_BUILD_LIBLARY

namespace Utility
{

/**
 * @class Ini
 * @brief Iniファイルパース用クラス
 * @note 本クラス作成に当たり以下のサイトを参考にした。
 * @n クロスプラットフォームのini ファイルパーサ
 * @n @link https://github.com/Rookfighter/inifile-cpp.git @endlink
 * @n 本家と異なる点は以下のとおり。
 * @n 読込時に全セクション・全フィールドを読み込まず、read~ やwrite~ でそれぞれ指定したものだけ読み取れるようにする。
 * @n 上記に伴い対象のセクション・フィールドがiniファイル中に存在しなかった場合第3引数で指定したデフォルト値を読み取る。
 * 
 * @example test/utility/ini/test_ini.cpp
 */
class Ini
{
private:
    std::string file_path_;
    char field_separator_;
    std::vector<std::string> comment_prefix_list_;
    bool is_quiet_;
    void trim(std::string &str) const;
    bool is_convertable_to_long(const std::string &value) const;
    bool is_ignore_line(const std::string &line) const;
    bool try_get_field(std::string &value, const std::string &target_section, const std::string &target_field) const;
    void set_field(const std::string &value, const std::string &target_section, const std::string &target_field) const;

public:

    /**
     * @fn Ini
     * @brief コンストラクタ
     * 
     * @param std::string file_path iniファイルへのパス 
     */
    Ini(const std::string &file_path)
     :  file_path_(std::string(file_path.c_str())), field_separator_('='), comment_prefix_list_({"#", ";"})
    {}

    /**
     * @fn set_field_separator
     * @brief フィールド取得時の区切り文字の変更メソッド
     * @param char field_separator 区切り文字
     * @return Ini& 区切り文字変更後のパーサインスタンス
     * @note インスタンス生成時のデフォルト区切り文字は'='としている。
     */
    Ini& set_field_separator(const char field_separator);

    /**
     * @fn set_comment_prefix_list
     * @brief コメント行の先頭文字列指定メソッド
     * @param std::vector<std::string> コメント行の先頭文字列(複数指定可能)
     * @return Ini& コメント行の先頭文字列変更後のパーサインスタンス
     * @note インスタンス生成時のデフォルトコメント行の先頭文字列は{ "#", ";" }としている。
     */
    Ini& set_comment_prefix_list(const std::vector<std::string> &comment_prefix_list);

    /**
     * @fn read_bool
     * @brief 真理値取得メソッド
     * @param std::string section_name []で囲まれたセクション名
     * @param std::string field_name 取得したいフィールド名
     * @param bool default_value 読取失敗時のデフォルト値
     * @return bool 読取成功時は読み取った値を、失敗した場合はデフォルト値を返却する。
     * @note 真理値を表現文字列としては "true" もしくは "false" とする。(大文字/小文字は問わない。)
     */
    bool read_bool(const std::string& section_name, const std::string& field_name, const bool& default_value) const;

    /**
     * @fn read_int
     * @brief 整数値取得メソッド
     * @param std::string section_name []で囲まれたセクション名
     * @param std::string field_name 取得したいフィールド名
     * @param int default_value 読取失敗時のデフォルト値
     * @return int 読取成功時は読み取った値を、失敗した場合はデフォルト値を返却する。
     */
    int read_int(const std::string& section_name, const std::string& field_name, const int& default_value) const;

    /**
     * @fn read_double
     * @brief 浮動小数点値取得メソッド
     * @param std::string section_name []で囲まれたセクション名
     * @param std::string field_name 取得したいフィールド名
     * @param double default_value 読取失敗時のデフォルト値
     * @return double 読取成功時は読み取った値を、失敗した場合はデフォルト値を返却する。
     */    
    double read_double(const std::string& section_name, const std::string& field_name, const double &default_value) const;

    /**
     * @fn read_str
     * @brief 文字列取得メソッド
     * @param std::string section_name []で囲まれたセクション名
     * @param std::string field_name 取得したいフィールド名
     * @param std::string default_value 読取失敗時のデフォルト値
     * @return std::string 読取成功時は読み取った値を、失敗した場合はデフォルト値を返却する。
     */    
    std::string read_str(const std::string& section_name, const std::string& field_name, const std::string &default_value) const;

    /**
     * @fn write_bool
     * @brief 真理値書込メソッド
     * @param std::string section_name []で囲まれたセクション名
     * @param std::string field_name 取得したいフィールド名
     * @param bool value 書込値
     * @note 既に対象のセクション・フィールドが存在していた場合上書きする。
     */
    void write_bool(const std::string& section_name, const std::string& field_name, const bool& value) const;

    /**
     * @fn write_int
     * @brief 真理値書込メソッド
     * @param std::string section_name []で囲まれたセクション名
     * @param std::string field_name 取得したいフィールド名
     * @param int value 書込値
     * @note 既に対象のセクション・フィールドが存在していた場合上書きする。
     */
    void write_int(const std::string& section_name, const std::string& field_name, const int& value) const;

    /**
     * @fn write_double
     * @brief 浮動小数点値書込メソッド
     * @param std::string section_name []で囲まれたセクション名
     * @param std::string field_name 取得したいフィールド名
     * @param double value 書込値
     * @note 既に対象のセクション・フィールドが存在していた場合上書きする。
     */
    void write_double(const std::string& section_name, const std::string& field_name, const double& value) const;

    /**
     * @fn write_double
     * @brief 浮動小数点値書込メソッド
     * @param std::string section_name []で囲まれたセクション名
     * @param std::string field_name 取得したいフィールド名
     * @param std::string value 書込値
     * @note 既に対象のセクション・フィールドが存在していた場合上書きする。
     */
    void write_str(const std::string& section_name, const std::string& field_name, const std::string& value) const;
};

} 

#else

namespace Utility
{

class Ini
{
private:
    std::string file_path_;
    char field_separator_;
    std::vector<std::string> comment_prefix_list_;
    bool is_quiet_;

    void trim(std::string& str) const
    {
        const char *whitespaces = " \t\n\r\f\v";
        auto lascurrent_tpos = str.find_last_not_of(whitespaces);
        if (lascurrent_tpos == std::string::npos)
        {
            str.clear();
            return;
        }
        str.erase(lascurrent_tpos + 1);
        str.erase(0, str.find_first_not_of(whitespaces));
    }

    bool is_convertable_to_long(const std::string& value) const
    {
        char *endptr;
        long result;
        result = std::strtol(value.c_str(), &endptr, 10);
        if (*endptr == '\0')
            return true;
        result = std::strtol(value.c_str(), &endptr, 8);
        if (*endptr == '\0')
            return true;
        result = std::strtol(value.c_str(), &endptr, 16);
        if (*endptr == '\0')
            return true;
        return false;
    }

    bool is_ignore_line(const std::string& line) const
    {
        if (line.size() == 0)
            return true;

        for (const auto &commentPrefix : comment_prefix_list_)
        {
            std::size_t commentPos = line.find(commentPrefix);
            if (commentPos == 0)
                return true;
        }

        return false;
    }

    bool try_get_field(const std::string& target_section, const std::string& target_field, std::string& value) const
    {
        std::stringstream iss;
        std::ifstream ifs(file_path_);
        iss << ifs.rdbuf();

        int line_num = 0;
        std::string current_section = "";
        std::string line;

        // iterate file line by line
        while (!iss.eof() && !iss.fail())
        {
            std::getline(iss, line, '\n');
            trim(line);
            ++line_num;

            if (is_ignore_line(line))
                continue;

            if (line[0] == '[')
            {
                std::size_t pos = line.find("]");
                if (pos == std::string::npos)
                {
                    std::stringstream ss;
                    ss << "l." << line_num << ": ini parsing failed, section not closed";
                    throw std::runtime_error(ss.str());
                }
                if (pos == 1)
                {
                    std::stringstream ss;
                    ss << "l." << line_num << ": ini parsing failed, section is empty";
                    throw std::runtime_error(ss.str());
                }
                current_section = line.substr(1, pos - 1);
            }
            else
            {
                if (current_section == "")
                {
                    std::stringstream ss;
                    ss << "l." << line_num << ": ini parsing failed, field has no section";
                    throw std::runtime_error(ss.str());
                }

                std::size_t pos = line.find(field_separator_);

                if (pos == std::string::npos)
                {
                    std::stringstream ss;
                    ss << "l." << line_num << ": ini parsing failed, no '" << field_separator_ << "' found";
                    throw std::runtime_error(ss.str());
                }
                else
                {
                    std::string currentField = line.substr(0, pos);
                    trim(currentField);
                    std::string currentValue = line.substr(pos + 1, std::string::npos);
                    trim(currentValue);

                    if (current_section == target_section && currentField == target_field)
                    {
                        value = currentValue;
                        return (value.size() != 0) ? true : false;
                    }
                }
            }
        }
        return false;
    }

    void set_field(const std::string& target_section, const std::string& target_field, const std::string& value) const
    {
        std::stringstream iss, oss;
        std::ifstream ifs(file_path_);
        iss << ifs.rdbuf();

        enum FoundMode { NO_MATCH, SECTION_MATCH, BOTH_MATCH };
        enum FoundMode found_mode = NO_MATCH;

        int line_num = 0;
        std::string current_section = "";
        std::string line;

        // iterate file line by line
        while (!iss.eof() && !iss.fail())
        {
            std::getline(iss, line, '\n');
            trim(line);
            ++line_num;

            if (is_ignore_line(line))
            {
                oss << line << '\n';
                continue;
            }

            if (line[0] == '[')
            {
                std::size_t pos = line.find("]");
                if (pos == std::string::npos)
                {
                    std::stringstream ss;
                    ss << "l." << line_num << ": ini parsing failed, section not closed";
                    throw std::runtime_error(ss.str());
                }
                if (pos == 1)
                {
                    std::stringstream ss;
                    ss << "l." << line_num << ": ini parsing failed, section is empty";
                    throw std::runtime_error(ss.str());
                }
                current_section = line.substr(1, pos - 1);

                if (current_section==target_section)
                    found_mode = SECTION_MATCH;
                else if (found_mode==SECTION_MATCH && current_section!=target_section)
                {
                    line = target_field + field_separator_ + value + '\n' + line;
                    found_mode = BOTH_MATCH;
                }
            }
            else
            {
                if (current_section=="")
                {
                    std::stringstream ss;
                    ss << "line[" << line_num << "] of \"" << file_path_ << "\" parsing failed, field has no section";
                    throw std::runtime_error(ss.str());
                }

                std::size_t pos = line.find(field_separator_);
                if (pos==std::string::npos)
                {
                    std::stringstream ss;
                    ss << "line[" << line_num << ": parsing failed, no \"" << field_separator_ << "\" found";
                    throw std::runtime_error(ss.str());
                }
                else
                {
                    std::string currentField = line.substr(0, pos);
                    trim(currentField);
                    if (current_section==target_section && currentField==target_field)
                    {
                        line = std::string(target_field.c_str()) + field_separator_ + value;
                        found_mode = BOTH_MATCH;
                    }
                }
            }
            oss << line << '\n';
        }

        if (found_mode==NO_MATCH)
        {
            oss << '[' << target_section << ']' << '\n';
            oss << target_field << field_separator_ << value << '\n';
        }
        else if (found_mode==SECTION_MATCH)
        {
            oss << target_field << field_separator_ << value << '\n';
        }

        std::string result = oss.str();
        result.pop_back();

        std::ofstream ofs(file_path_);
        if(ofs.is_open())
            ofs << result;
    }

public:

    Ini(const std::string &file_path, const bool& is_quiet=true)
     :  file_path_(std::string(file_path.c_str())), field_separator_('='), comment_prefix_list_({"#", ";"}), is_quiet_(is_quiet)
    {}

    Ini& set_field_separator(const char field_separator)
    {
        field_separator_ = field_separator;
        return *this;
    }

    Ini& set_comment_prefix_list(const std::vector<std::string>& comment_prefix_list)
    {
        comment_prefix_list_ = comment_prefix_list;
        return *this;
    }

    bool read_bool(const std::string& section_name, const std::string& field_name, const bool& default_value) const
    {
        std::string str;
        if (!try_get_field(section_name, field_name, str))
            return default_value;

        std::transform(str.begin(), str.end(), str.begin(), [](const char c){ 
                return static_cast<char>(::toupper(c)); 
            }
        );

        if (str == "TRUE")
            return true;
        else if (str == "FALSE")
            return false;
        else
            return default_value;
    }

    int read_int(const std::string& section_name, const std::string& field_name, const int& default_value) const
    {
        std::string str;
        if (!try_get_field(section_name, field_name, str))
            return default_value;

        if (!is_convertable_to_long(str))
            return default_value;

        try
        {
            auto value = std::stol(str);
            return value;
        }
        catch (...)
        {
            return default_value;
        }
    }

    double read_double(const std::string& section_name, const std::string& field_name, const double& default_value) const
    {
        std::string str;
        if (!try_get_field(section_name, field_name, str))
            return default_value;
            
        try
        {
            auto value = std::stod(str);
            return value;
        }
        catch (...)
        {
            return default_value;
        }
    }

    std::string read_str(const std::string& section_name, const std::string& field_name, const std::string& default_value) const
    {
        std::string str;
        if (!try_get_field(section_name, field_name, str))
            return default_value;
        return std::string(str);
    }

    void write_bool(const std::string& section_name, const std::string& field_name, const bool &value) const
    {
        std::string result = value ? "true" : "false";
        set_field(section_name, field_name, result);
    }

    void write_int(const std::string& section_name, const std::string& field_name, const int& value) const
    {
        std::stringstream ss;
        ss << value;
        set_field(section_name, field_name, ss.str());
    }

    void write_double(const std::string& section_name, const std::string& field_name, const double& value) const
    {
        std::stringstream ss;
        ss << value;
        set_field(section_name, field_name, ss.str());
    }

    void write_str(const std::string& section_name, const std::string& field_name, const std::string& value) const
    {
        set_field(section_name, field_name, value);
    }

    std::map<std::string, std::map<std::string, std::string>> load_all() const
    {
        std::map<std::string, std::map<std::string, std::string>> result;

        return result;
    }
};

}

#endif

#endif // GLOBAL_USE_BUILD_LIBLARY