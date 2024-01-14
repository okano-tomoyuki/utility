#ifndef _BUILDER_ALTERNATIVE_HPP_
#define _BUILDER_ALTERNATIVE_HPP_

#ifndef __BCPLUSPLUS__
#define __fastcall
#include <string.h>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <cerrno>
#include <cstring>

#include <sys/stat.h>

#ifdef __unix__
#include <unistd.h>
#include <sys/time.h>
#include <glob.h>
#include <zlib.h>
#include <linux/limits.h>
#else
#include <windows.h>
#include <ShlObj.h>
#endif

// ---------------------------------------------------------------------------

struct NotImplementedExeption : public std::logic_error
{
    NotImplementedExeption() : std::logic_error("Not Implemented.") {}
};

// ---------------------------------------------------------------------------

enum FileStreamOpenMode
{
    fmCreate,
    fmOpenWrite,
    fmOpenRead
};

// ---------------------------------------------------------------------------

enum ZipOpenMode
{
    zmWrite,
    zmRead
};

// ---------------------------------------------------------------------------

enum class TSearchOption
{
    soTopDirectoryOnly,
    soAllDirectories
};

// ---------------------------------------------------------------------------

enum TValueRelationship
{
    LessThanValue = -1,
    EqualsValue = 0,
    GreaterThanValue = 1
};

// ---------------------------------------------------------------------------

class AnsiString final
{
private:
    std::string str_;

public:
    AnsiString() : str_(std::string()) {}
    AnsiString(const AnsiString &ansi) : str_(ansi.str_) {}
    AnsiString(const std::string &str) : str_(str) {}
    AnsiString(const char *str) : str_(std::string(str)) {}
    AnsiString(const char c) : str_({c}) {}
    std::string str() const { return str_; }
    void operator=(const AnsiString &other) { str_ = other.str_; }
    void operator=(const std::string &other) { str_ = other; }
    void operator=(const char *other) { str_ = std::string(other); }
    void operator=(const char other) { str_ = {other}; }
    void operator+=(const AnsiString &other) { str_ += other.str_; }
    void operator+=(const std::string &other) { str_ += other; }
    void operator+=(const char *other) { str_ += std::string(other); }
    void operator+=(const char other) { str_ += {other}; }
    char operator[](const int &idx) const { return str_.at(idx - 1); }
    friend std::ostream& operator<<(std::ostream &os, const AnsiString &val) { os << val.str(); return os; }
    const char *c_str() const { return str_.c_str(); }

    AnsiString UpperCase() const
    {
        std::string str(str_);
        std::transform(str.begin(), str.end(), str.begin(), [](const char c){ return static_cast<char>(::toupper(c)); });
        return AnsiString(str);
    }

    AnsiString LowerCase() const
    {
        std::string str(str_);
        std::transform(str.begin(), str.end(), str.begin(), [](const char c){ return static_cast<char>(::toupper(c)); });
        return AnsiString(str);
    }

    template <typename... Args>
    AnsiString sprintf(const char *fmt, Args... args)
    {
        int size_s = std::snprintf(nullptr, 0, fmt, args...) + 1;
        if (size_s <= 0)
        {
            throw std::runtime_error("Error during formatting.");
        }
        auto size = static_cast<size_t>(size_s);
        std::unique_ptr<char[]> buf(new char[size]);
        std::snprintf(buf.get(), size, fmt, args...);
        return AnsiString(std::string(buf.get(), buf.get() + size - 1));
    }

    int ToInt() const { return std::stol(str_); }
    double ToFloat() const { return std::stod(str_); }
    std::size_t Length() const { return str_.length(); }
    bool IsEmpty() const { return str_.empty(); }

    AnsiString& Delete(const int &start, const int &length)
    {
        str_.erase(start - 1, length);
        return *this;
    }

    AnsiString TrimRight() const
    {
        std::string str(str_);
        auto findPos = str.find_last_not_of(" \t\n\r\f\v");
        if (findPos == std::string::npos)
            str.clear();
        else
            str.erase(findPos + 1);
        return AnsiString(str);
    }

    AnsiString TrimLeft() const
    {
        std::string str(str_);
        auto findPos = str.find_first_not_of(" \t\n\r\f\v");
        if (findPos == std::string::npos)
            str.clear();
        else
            str.erase(0, findPos);
        return AnsiString(str);
    }

    AnsiString Trim() const
    {
        auto leftTrimed = TrimLeft();
        return leftTrimed.TrimRight();
    }

    AnsiString SubString(const int &start, const int &substractSize) { return str_.substr(start - 1, substractSize); }

    template <typename T>
    std::vector<AnsiString> SplitString(const T &separator) const
    {
        auto sep = std::string(AnsiString(separator).c_str());
        if (str_.empty())
            return {};
        if (sep.empty())
            return {AnsiString(str_)};
        std::vector<AnsiString> result;
        std::size_t sepSize = sep.size();
        std::size_t findStart = 0;
        while (true)
        {
            std::size_t findPos = str_.find(sep, findStart);
            if (findPos == std::string::npos)
            {
                result.emplace_back(AnsiString(std::string(str_.begin() + findStart, str_.end())));
                break;
            }
            result.emplace_back(AnsiString(std::string(str_.begin() + findStart, str_.begin() + findPos)));
            findStart = findPos + sepSize;
        }
        return result;
    }

    template <typename T>
    int AnsiPos(T target) const
    {
        auto tgt = AnsiString(target);
        auto match_index = str_.find_first_of(tgt.str_);
        return (match_index != std::string::npos) ? (match_index + 1) : 0;
    }

    template <typename T>
    int Pos(T target) const
    {
        return AnsiPos(target);
    }

#ifndef __unix__
    std::wstring wstr() const
    {
        std::string temp = str();
        int size = MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int)temp.size(), NULL, 0);
        std::wstring result(size, 0);
        MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int)temp.size(), &result[0], size);
        return result;
    }

    AnsiString (const std::wstring& wstring)
    {
        std::string str;
        if (!wstring.empty())
        {
            int size = WideCharToMultiByte(CP_UTF8, 0, &wstring[0], (int)wstring.size(), NULL, 0, NULL, NULL);
            string.resize(size, 0);
            WideCharToMultiByte(CP_UTF8, 0, &wstring[0], (int)wstring.size(), &str[0], size, NULL, NULL);
        }
        str_ = str;
    }

    void operator=(const std::wstring& wstring)
    {
        str_ = AnsiString(wstring).str();
    }

    void operator+=(const std::wstring& wstring)
    {
        str_ += AnsiString(wstring).str();
    }

#endif

};

bool operator==(const AnsiString &lhs, const AnsiString &rhs) { return lhs.str() == rhs.str(); }
bool operator==(const AnsiString &lhs, const std::string &rhs) { return lhs.str() == rhs; }
bool operator==(const AnsiString &lhs, const char *rhs) { return lhs.str() == std::string(rhs); }
bool operator==(const AnsiString &lhs, const char rhs) { return lhs.str() == std::to_string(rhs); }
bool operator==(const std::string &lhs, const AnsiString &rhs) { return AnsiString(lhs) == rhs; }
bool operator==(const char *lhs, const AnsiString &rhs) { return AnsiString(lhs) == rhs; }
bool operator==(const char lhs, const AnsiString &rhs) { return AnsiString(lhs) == rhs; }
bool operator!=(const AnsiString &lhs, const AnsiString &rhs) { return !(lhs == rhs); }
bool operator!=(const AnsiString &lhs, const std::string &rhs) { return !(lhs == rhs); }
bool operator!=(const AnsiString &lhs, const char *rhs) { return !(lhs == rhs); }
bool operator!=(const AnsiString &lhs, const char rhs) { return !(lhs == rhs); }
bool operator!=(const std::string &lhs, const AnsiString &rhs) { return !(lhs == rhs); }
bool operator!=(const char *lhs, const AnsiString &rhs) { return !(lhs == rhs); }
bool operator!=(const char lhs, const AnsiString &rhs) { return !(lhs == rhs); }
AnsiString operator+(const AnsiString &lhs, const AnsiString &rhs) { return lhs + rhs; }
AnsiString operator+(const AnsiString &lhs, const std::string &rhs) { return lhs + rhs; }
AnsiString operator+(const AnsiString &lhs, const char *rhs) { return lhs + rhs; }
AnsiString operator+(const AnsiString &lhs, const char rhs) { return lhs + rhs; }
AnsiString operator+(const std::string &lhs, const AnsiString &rhs) { return lhs + rhs; }
AnsiString operator+(const char *lhs, const AnsiString &rhs) { return lhs + rhs; }
AnsiString operator+(const char lhs, const AnsiString &rhs) { return lhs + rhs; }
double StrToFloat(const AnsiString &str) { return str.ToFloat(); }
template <typename T>
int AnsiPos(T target, const AnsiString &str) { return str.AnsiPos(target); }

// ---------------------------------------------------------------------------

using UnicodeString = AnsiString;

// ---------------------------------------------------------------------------

class TStringDynArray final
{
    std::vector<AnsiString> arr_;

public:
    std::size_t Length;
    TStringDynArray(const std::vector<AnsiString> &arr)
     :  arr_(arr), Length(arr.size()) 
    {}
    
    TStringDynArray(std::vector<AnsiString> &&arr)
     :  arr_(std::move(arr)), Length(arr.size()) 
    {}
    
    TStringDynArray(const std::vector<std::string> &arr) 
     :  Length(arr.size()) 
    {
        arr_.reserve(Length);
        for(auto&& e : arr)
            arr_.push_back(e);
    }

    TStringDynArray(std::vector<std::string>&& arr) 
     :  Length(arr.size()) 
    {
        arr_.reserve(Length);
        for(auto&& e : arr)
            arr_.push_back(std::move(e));
    }

    AnsiString operator[](const int &index) const { return arr_.at(index); }
};

// ---------------------------------------------------------------------------

template <typename T>
TStringDynArray SplitString(const AnsiString &str, const T &separator) { return TStringDynArray(str.SplitString(separator)); }

// ---------------------------------------------------------------------------

class TDateTime final
{

private:
    int year_;
    int month_;
    int day_;
    int hour_;
    int minute_;
    int second_;
    int millisecond_;
    int microsecond_;

    std::string zeroFill(const std::string& str, const int& digit) const
    {
        std::string result(str);
        int remain = digit > str.size() ? digit - str.size() : 0;
        while(remain--)
            result = '0' + result;
        return result;
    }

    void convert(std::string& str, const char* fmt, const std::string& rep) const
    {
        std::string::size_type pos = 0;
        int fmtSize = std::strlen(fmt);
        while(true)
        {
            pos = str.find(fmt, pos);
            if(pos == std::string::npos)
                break;
            else
            {
                if(pos>0 && pos+fmtSize+1<str.size() && str[pos-1]=='\'' && str[pos+fmtSize]=='\'')
                {
                    auto l = str.substr(0, pos-1);
                    auto r = str.substr(pos+fmtSize+1);
                    str = l + fmt + r;
                    pos = pos + fmtSize + 1;
                }
                else if(pos!=0 && pos+fmtSize+1<str.size() && str[pos-1]=='\"' && str[pos+fmtSize]=='\"')
                {
                    auto l = str.substr(0, pos-1);
                    auto r = str.substr(pos+fmtSize+1);
                    str = l + fmt + r;
                    pos = pos + fmtSize-1;
                }
                else
                {
                    auto l = str.substr(0, pos);
                    auto r = str.substr(pos+fmtSize);
                    str = l + rep + r;
                    pos = pos + rep.size();
                }
            }
        }
    }

public:

    TDateTime(const std::chrono::system_clock::time_point& tP = std::chrono::system_clock::now())
    { 
        using std::chrono::system_clock;
        using std::chrono::duration_cast;
        using std::chrono::seconds;
        using std::chrono::microseconds;
        
        auto tT  = system_clock::to_time_t(tP);
        auto tM = std::gmtime(&tT);
        int usecEpoch  = duration_cast<microseconds>(tP.time_since_epoch()).count();
        int secEpoch   = 1000000 * duration_cast<seconds>(tP.time_since_epoch()).count();
        int usec = usecEpoch - secEpoch;
        year_        = tM->tm_year+1900;
        month_       = tM->tm_mon+1;
        day_         = tM->tm_mday;
        hour_        = tM->tm_hour;
        minute_      = tM->tm_min;
        second_      = tM->tm_sec;
        millisecond_ = usec/1000;
        microsecond_ = usec%1000;
    }

    TDateTime(const int& year, const int& month, const int& day, const int& hour, const int& minute, const int& second, const int& millisecond, const int& microsecond)
     :  year_(year), month_(month), day_(day), 
        hour_(hour), minute_(minute), second_(second), millisecond_(millisecond), microsecond_(microsecond)
    {}

    TDateTime(const TDateTime& other)
     :  year_(other.year_), month_(other.month_), day_(other.day_), 
        hour_(other.hour_), minute_(other.minute_), second_(other.second_), millisecond_(other.millisecond_), microsecond_(other.microsecond_)
    {}

    int YearOf()              const { return year_;        }
    int MonthOf()             const { return month_;       }
    int DayOf()               const { return day_;         }
    int HourOf()              const { return hour_;        }
    int MinuteOf()            const { return minute_;      }
    int SecondOf()            const { return second_;      }
    int MilliSecondOf()       const { return millisecond_; }
    int MicroSecondOf()       const { return microsecond_; }
    int MilliSecondOfTheDay() const { return 3600000 * HourOf() + 60000 * MinuteOf() + 1000 * SecondOf() + MicroSecondOf(); }

    AnsiString FormatString(const char *format)
    {
        const char* yyyy   = "yyyy";
        const char* mm     = "mm";
        const char* dd     = "dd";
        const char* hh     = "hh";
        const char* nn     = "nn";
        const char* ss     = "ss";
        const char* zzzzzz = "zzzzzz";
        const char* zzz    = "zzz";

        std::string result(format);

        convert(result,   yyyy, std::to_string(YearOf()));
        convert(result,     mm, zeroFill(std::to_string(MonthOf()), 2));
        convert(result,     dd, zeroFill(std::to_string(DayOf()), 2));
        convert(result,     hh, zeroFill(std::to_string(HourOf()), 2));
        convert(result,     nn, zeroFill(std::to_string(MinuteOf()), 2));
        convert(result,     ss, zeroFill(std::to_string(SecondOf()), 2));
        convert(result, zzzzzz, zeroFill(std::to_string(1000*MilliSecondOf()+MicroSecondOf()), 6));
        convert(result,    zzz, zeroFill(std::to_string(MilliSecondOf()), 3));

        return AnsiString(result);
    }

    std::chrono::system_clock::time_point ToTimePoint() const
    {
        using std::chrono::system_clock;
        using std::chrono::milliseconds;
        using std::chrono::microseconds;

        std::tm tmVar{0};
        tmVar.tm_year  = YearOf() - 1900;
        tmVar.tm_mon   = MonthOf() - 1;
        tmVar.tm_mday  = DayOf();
        tmVar.tm_hour  = HourOf();
        tmVar.tm_min   = MinuteOf();
        tmVar.tm_sec   = SecondOf();
        tmVar.tm_isdst = -1;
#ifdef __unix__
        auto timeType = timegm(&tmVar);
#else 
        auto timeType = _mkgmtime(&tmVar);
#endif
        auto timePoint = system_clock::from_time_t(timeType) + milliseconds(MilliSecondOf()) + microseconds(MicroSecondOf());

        return timePoint;
    }
};

TDateTime Now() { return TDateTime(); }
TDateTime IncMilliSecond(const TDateTime &other, const int &increment) { return TDateTime(other.ToTimePoint() + std::chrono::milliseconds(increment)); }
TDateTime IncSecond(const TDateTime &other, const int &increment) { return TDateTime(other.ToTimePoint() + std::chrono::seconds(increment)); }
TDateTime IncMinute(const TDateTime &other, const int &increment) { return TDateTime(other.ToTimePoint() + std::chrono::minutes(increment)); }
TDateTime IncHour(const TDateTime &other, const int &increment) { return TDateTime(other.ToTimePoint() + std::chrono::hours(increment)); }

enum TValueRelationship CompareTime(const TDateTime &lhs, const TDateTime &rhs)
{
    if(lhs.MilliSecondOfTheDay() > rhs.MilliSecondOfTheDay())       return GreaterThanValue;
    else if(lhs.MilliSecondOfTheDay() < rhs.MilliSecondOfTheDay())  return LessThanValue;
    else                                                            return EqualsValue;
}

// ---------------------------------------------------------------------------

class TFileStream final
{
private:
    char *dataPtr_;

public:
    std::size_t Size;

    TFileStream(const AnsiString &filePath, const enum FileStreamOpenMode &fileStreamOpenMode)
    {
        throw NotImplementedExeption();

        std::fstream fs;

        if (fileStreamOpenMode == fmCreate)
        {
            fs.open(filePath.str(), std::ios_base::binary);
        }
        else if (fileStreamOpenMode == fmOpenRead)
        {
            fs.open(filePath.str(), std::ios_base::binary);
            if(!fs.is_open())
            {
                
            }            
        }
        else if (fileStreamOpenMode == fmOpenWrite)
        {
        }
    }

    int Read(char *dataPtr, const int &readSize)
    {
        throw NotImplementedExeption();
        return 0;
    }

    void Write(const char *dataPtr, const int &writeSize)
    {
        throw NotImplementedExeption();
    }
};

// ---------------------------------------------------------------------------

class TZipFile : z_stream
{

private:

public:
    TZipFile()
    {
        this->zalloc = nullptr;
        this->zfree  = nullptr;
        this->opaque = nullptr;
    }

    ~TZipFile()
    {
        if(true)
        {
            inflateEnd(this);
        }
        else
        {
            deflateEnd(this);
        }
    }


    void Open(const AnsiString &filePath, const enum ZipOpenMode &zipOpenMode)
    {
        throw NotImplementedExeption();
    }

    void Close()
    {
        throw NotImplementedExeption();
    }

    void ExtractAll(const AnsiString &filePath) const
    {
        throw NotImplementedExeption();
        return;
    }

    void Add(const AnsiString &filePath)
    {
        throw NotImplementedExeption();
    }
};

// ---------------------------------------------------------------------------

class TPath final
{
public:
    enum PathType
    {
        windowsPath = 0,
        posixPath = 1,
#ifdef __unix__
        nativePath = posixPath
#else
        nativePath = windowsPath
#endif
    };

private:

#ifndef __unix__
    static const size_t MAX_PATH_WINDOWS = 32767;
#endif
    static const size_t MAX_PATH_WINDOWS_LEGACY = 260;
    PathType type_;
    std::vector<std::string> path_;
    bool isAbsolute_;
    bool isSmb_; // Unused, except for on Windows

    static std::vector<std::string> tokenize(const std::string &string, const std::string &delim)
    {
        std::string::size_type lastPos = 0, pos = string.find_first_of(delim, lastPos);
        std::vector<std::string> tokens;

        while (lastPos != std::string::npos)
        {
            if (pos != lastPos)
                tokens.push_back(string.substr(lastPos, pos - lastPos));
            lastPos = pos;
            if (lastPos == std::string::npos || lastPos + 1 == string.length())
                break;
            pos = string.find_first_of(delim, ++lastPos);
        }

        return tokens;
    }

public:

    TPath()
     :  type_(nativePath), isAbsolute_(false), isSmb_(false) 
    {}

    TPath(const TPath &path)
     :  type_(path.type_), path_(path.path_), isAbsolute_(path.isAbsolute_), isSmb_(path.isSmb_)
    {}

    TPath(TPath &&path)
     :  type_(path.type_), path_(std::move(path.path_)), isAbsolute_(path.isAbsolute_), isSmb_(path.isSmb_) 
    {}

    TPath(const char *string)
     :  isSmb_(false) 
    { set(string); }

    TPath(const std::string &string)
     :  isSmb_(false) 
    { set(string); }

#ifndef __unix__
    TPath(const std::wstring &wstring) { set(wstring); }
    TPath(const wchar_t *wstring) { set(wstring); }
#endif

    size_t length() const { return path_.size(); }

    bool empty() const { return path_.empty(); }

    bool isAbsolute() const { return isAbsolute_; }

    TPath make_absolute() const
    {
#ifdef __unix__
        char temp[PATH_MAX];
        if (realpath(str().c_str(), temp) == NULL)
            throw std::runtime_error("Internal error in realpath(): " + std::string(strerror(errno)));
        return TPath(temp);
#else
        std::wstring value = wstr(), out(MAX_PATH_WINDOWS, '\0');
        DWORD length = GetFullPathNameW(value.c_str(), MAX_PATH_WINDOWS, &out[0], NULL);
        if (length == 0)
            throw std::runtime_error("Internal error in realpath(): " + std::to_string(GetLastError()));
        return TPath(out.substr(0, length));
#endif
    }

    bool exists() const
    {
#ifdef __unix__
        struct stat sb;
        return stat(str().c_str(), &sb) == 0;
#else
        return GetFileAttributesW(wstr().c_str()) != INVALID_FILE_ATTRIBUTES;
#endif
    }

    size_t file_size() const
    {
#ifdef __unix__
        struct stat sb;
        if (stat(str().c_str(), &sb) != 0)
            throw std::runtime_error("path::file_size(): cannot stat file \"" + str() + "\"!");
#else
        struct _stati64 sb;
        if (_wstati64(wstr().c_str(), &sb) != 0)
            throw std::runtime_error("path::file_size(): cannot stat file \"" + str() + "\"!");
#endif
        return (size_t)sb.st_size;
    }

    bool is_directory() const
    {
#ifdef __unix__
        struct stat sb;
        if (stat(str().c_str(), &sb))
            return false;
        return S_ISDIR(sb.st_mode);
#else
        DWORD result = GetFileAttributesW(wstr().c_str());
        if (result == INVALID_FILE_ATTRIBUTES)
            return false;
        return (result & FILE_ATTRIBUTE_DIRECTORY) != 0;
#endif
    }

    bool is_file() const
    {
#ifdef _WIN32
        DWORD attr = GetFileAttributesW(wstr().c_str());
        return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
#else
        struct stat sb;
        if (stat(str().c_str(), &sb))
            return false;
        return S_ISREG(sb.st_mode);
#endif
    }

    std::string extension() const
    {
        const std::string &name = filename();
        size_t pos = name.find_last_of(".");
        if (pos == std::string::npos)
            return "";
        return name.substr(pos + 1);
    }

    std::string filename() const
    {
        if (empty())
            return "";
        const std::string &last = path_[path_.size() - 1];
        return last;
    }

    TPath parent_path() const
    {
        TPath result;
        result.isAbsolute_ = isAbsolute_;
        result.isSmb_ = isSmb_;

        if (path_.empty())
        {
            if (!isAbsolute_)
                result.path_.push_back("..");
        }
        else
        {
            size_t until = path_.size() - 1;
            for (size_t i = 0; i < until; ++i)
                result.path_.push_back(path_[i]);
        }
        return result;
    }

    TPath operator/(const TPath &other) const
    {
        if (other.isAbsolute_)
            throw std::runtime_error("TPath::operator/(): expected a relative path!");
        if (type_ != other.type_)
            throw std::runtime_error("TPath::operator/(): expected a path of the same type!");

        TPath result(*this);

        for (size_t i = 0; i < other.path_.size(); ++i)
            result.path_.push_back(other.path_[i]);

        return result;
    }

    std::string str(PathType type = nativePath) const
    {
        std::ostringstream oss;

        if (isAbsolute_)
        {
            if (type_ == posixPath)
                oss << "/";
            else
            {
                size_t length = 0;
                for (size_t i = 0; i < path_.size(); ++i)
                    // No special case for the last segment to count the NULL character
                    length += path_[i].length() + 1;
                if (isSmb_)
                    length += 2;

                // Windows requires a \\?\ prefix to handle paths longer than MAX_PATH
                // (including their null character). NOTE: relative paths >MAX_PATH are
                // not supported at all in Windows.
                if (length > MAX_PATH_WINDOWS_LEGACY)
                {
                    if (isSmb_)
                        oss << "\\\\?\\UNC\\";
                    else
                        oss << "\\\\?\\";
                }
                else if (isSmb_)
                    oss << "\\\\";
            }
        }

        for (size_t i = 0; i < path_.size(); ++i)
        {
            oss << path_[i];
            if (i + 1 < path_.size())
            {
                if (type == posixPath)
                    oss << '/';
                else
                    oss << '\\';
            }
        }

        return oss.str();
    }

    void set(const std::string &str, PathType type = nativePath)
    {
        type_ = type;
        if (type == windowsPath)
        {
            std::string tmp = str;

            // Long windows paths (sometimes) begin with the prefix \\?\. It should only
            // be used when the path is >MAX_PATH characters long, so we remove it
            // for convenience and add it back (if necessary) in str()/wstr().
            static const std::string LONG_PATH_PREFIX = "\\\\?\\";
            if (tmp.length() >= LONG_PATH_PREFIX.length() && std::mismatch(std::begin(LONG_PATH_PREFIX), std::end(LONG_PATH_PREFIX), std::begin(tmp)).first == std::end(LONG_PATH_PREFIX))
            {
                tmp.erase(0, LONG_PATH_PREFIX.length());
            }

            // Special-case handling of absolute SMB paths, which start with the prefix "\\".
            if (tmp.length() >= 2 && tmp[0] == '\\' && tmp[1] == '\\')
            {
                path_ = {};
                tmp.erase(0, 2);

                // Interestingly, there is a special-special case where relative paths may be specified as beginning with a "\\"
                // when a non-SMB file with a more-than-260-characters-long absolute _local_ path is double-clicked. This seems to
                // only happen with single-segment relative paths, so we can check for this condition by making sure no further
                // path separators are present.
                if (tmp.find_first_of("/\\") != std::string::npos)
                    isAbsolute_ = isSmb_ = true;
                else
                    isAbsolute_ = isSmb_ = false;

                // Special-case handling of absolute SMB paths, which start with the prefix "UNC\"
            }
            else if (tmp.length() >= 4 && tmp[0] == 'U' && tmp[1] == 'N' && tmp[2] == 'C' && tmp[3] == '\\')
            {
                path_ = {};
                tmp.erase(0, 4);
                isAbsolute_ = true;
                isSmb_ = true;
                // Special-case handling of absolute local paths, which start with the drive letter and a colon "X:\"
                // So that UTF-8 works, do not call std::isalpha if the high bit is set, as that causes an assert on Windows.
            }
            else if (tmp.length() >= 3 && ((unsigned char)tmp[0] < 0x80) && std::isalpha(tmp[0]) &&
                     tmp[1] == ':' && (tmp[2] == '\\' || tmp[2] == '/'))
            {
                path_ = {tmp.substr(0, 2)};
                tmp.erase(0, 3);
                isAbsolute_ = true;
                isSmb_ = false;
                // Relative path
            }
            else
            {
                path_ = {};
                isAbsolute_ = false;
                isSmb_ = false;
            }

            std::vector<std::string> tokenized = tokenize(tmp, "/\\");
            path_.insert(std::end(path_), std::begin(tokenized), std::end(tokenized));
        }
        else
        {
            path_ = tokenize(str, "/");
            isAbsolute_ = !str.empty() && str[0] == '/';
        }
    }

    TPath &operator=(const TPath &path)
    {
        type_ = path.type_;
        path_ = path.path_;
        isAbsolute_ = path.isAbsolute_;
        isSmb_ = path.isSmb_;
        return *this;
    }

    TPath &operator=(TPath &&path)
    {
        if (this != &path)
        {
            type_ = path.type_;
            path_ = std::move(path.path_);
            isAbsolute_ = path.isAbsolute_;
            isSmb_ = path.isSmb_;
        }
        return *this;
    }

    friend std::ostream &operator<<(std::ostream &os, const TPath &path)
    {
        os << path.str();
        return os;
    }

    bool remove_file()
    {
#ifndef _WIN32
        return std::remove(str().c_str()) == 0;
#else
        return DeleteFileW(wstr().c_str()) != 0;
#endif
    }

    bool resize_file(size_t target_length)
    {
#ifdef __unix__
        return ::truncate(str().c_str(), (off_t)target_length) == 0;
#else
        HANDLE handle = CreateFileW(wstr().c_str(), GENERIC_WRITE, 0, nullptr, 0, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (handle == INVALID_HANDLE_VALUE)
            return false;
        LARGE_INTEGER size;
        size.QuadPart = (LONGLONG)target_length;
        if (SetFilePointerEx(handle, size, NULL, FILE_BEGIN) == 0)
        {
            CloseHandle(handle);
            return false;
        }
        if (SetEndOfFile(handle) == 0)
        {
            CloseHandle(handle);
            return false;
        }
        CloseHandle(handle);
        return true;
#endif
    }

    static TPath getcwd()
    {
#ifdef __unix__
        char temp[PATH_MAX];
        if (::getcwd(temp, PATH_MAX) == NULL)
            throw std::runtime_error("Internal error in getcwd(): " + std::string(strerror(errno)));
        return TPath(temp);
#else
        std::wstring temp(MAX_PATH_WINDOWS, '\0');
        if (!_wgetcwd(&temp[0], MAX_PATH_WINDOWS))
            throw std::runtime_error("Internal error in getcwd(): " + std::to_string(GetLastError()));
        return TPath(temp.c_str());
#endif
    }

#ifndef __unix__
    std::wstring wstr(PathType type = nativePath) const
    {
        std::string temp = str(type);
        int size = MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int)temp.size(), NULL, 0);
        std::wstring result(size, 0);
        MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int)temp.size(), &result[0], size);
        return result;
    }

    void set(const std::wstring &wstring, PathType type = nativePath)
    {
        std::string string;
        if (!wstring.empty())
        {
            int size = WideCharToMultiByte(CP_UTF8, 0, &wstring[0], (int)wstring.size(),
                                           NULL, 0, NULL, NULL);
            string.resize(size, 0);
            WideCharToMultiByte(CP_UTF8, 0, &wstring[0], (int)wstring.size(),
                                &string[0], size, NULL, NULL);
        }
        set(string, type);
    }

    TPath &operator=(const std::wstring &str)
    {
        set(str);
        return *this;
    }
#endif

    bool operator==(const TPath &p) const { return p.path_ == path_; }
    bool operator!=(const TPath &p) const { return p.path_ != path_; }
};

static TPath current_path_ = TPath::getcwd();

static TPath current_path()
{
    return current_path_;
}

static void current_path(const char* p)
{
    current_path_ = TPath(p);
}

bool create_directory(const TPath &p)
{
#ifdef _WIN32
    return CreateDirectoryW(p.wstr().c_str(), NULL) != 0;
#else
    return mkdir(p.str().c_str(), S_IRWXU) == 0;
#endif
}

bool create_directories(const TPath &p)
{
#ifdef __unix__
    if (create_directory(p.str().c_str()))
        return true;

    if (p.empty())
        return false;

    if (errno == ENOENT)
    {
        if (create_directory(p.parent_path()))
            return mkdir(p.str().c_str(), S_IRWXU) == 0;
        else
            return false;
    }
    return false;
#else
    return SHCreateDirectory(nullptr, p.make_absolute().wstr().c_str()) == ERROR_SUCCESS;
#endif
}

// ---------------------------------------------------------------------------

class TDirectory
{

private:

    static std::vector<std::string> searchPath(const std::string& filePath, const std::string& searchPattern, const TSearchOption searchOption)
    {
        glob_t globResult;
        std::string pattern = filePath + "/{*,.[!.]*,..?*}";
        glob(pattern.c_str(), GLOB_TILDE | GLOB_MARK | GLOB_BRACE, NULL, &globResult);
        
        std::vector<std::string> matchPathList;
        for(auto&& i=0; i<globResult.gl_pathc; i++)
        {
            auto matchPath = std::string(globResult.gl_pathv[i]);
            if(matchPath.back()=='/')
            {
                matchPath.pop_back();
                if(searchOption == TSearchOption::soAllDirectories)
                {
                    auto subDirectoryMatchPathList = searchPath(matchPath, searchPattern, searchOption);
                    for(auto&& subDirectoryMatchPath: subDirectoryMatchPathList)
                        matchPathList.push_back(std::move(subDirectoryMatchPath));
                }
            }
            else
            {
                matchPathList.push_back(std::move(matchPath));
            }
        }
        
        globfree(&globResult);
        return matchPathList;
    }

public:

    static TStringDynArray GetFiles(const AnsiString &directoryPath, const AnsiString &searchPattern, const TSearchOption searchOption)
    {
        auto matchFilePathList = searchPath(directoryPath.str(), searchPattern.str(), searchOption);
        return TStringDynArray(std::move(matchFilePathList));
    }

    static TStringDynArray GetDirectories(const AnsiString &directoryPath, const AnsiString &searchPattern, const TSearchOption searchOption)
    {
        auto matchFilePathList = searchPath(directoryPath.str(), searchPattern.str(), searchOption);
        return TStringDynArray(std::move(matchFilePathList));
    }

};

// ---------------------------------------------------------------------------

class TThread
{
    TThread() { throw NotImplementedExeption(); }
};

// ---------------------------------------------------------------------------

class TIniFile
{
private:
    std::string iniFilePath_;
    char fieldSeparator_;
    std::vector<std::string> commentPrefixList_;

    void trim(std::string &str)
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

    bool isConvertableToLong(const std::string &value) const
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

    bool isIgnoreLine(const std::string &line)
    {
        if (line.size() == 0)
            return true;

        for (const auto &commentPrefix : commentPrefixList_)
        {
            std::size_t commentPos = line.find(commentPrefix);
            if (commentPos == 0)
                return true;
        }

        return false;
    }

    bool tryGetField(std::string &value, const AnsiString &targetSection, const AnsiString &targetField)
    {
        std::stringstream iss;
        {
            std::ifstream ifs(iniFilePath_);
            iss << ifs.rdbuf();
        }

        int lineNo = 0;
        std::string currentSection = "";
        std::string line;

        // iterate file line by line
        while (!iss.eof() && !iss.fail())
        {
            std::getline(iss, line, '\n');
            trim(line);
            ++lineNo;

            if (isIgnoreLine(line))
                continue;

            if (line[0] == '[')
            {
                std::size_t pos = line.find("]");
                if (pos == std::string::npos)
                {
                    std::stringstream ss;
                    ss << "l." << lineNo << ": ini parsing failed, section not closed";
                    throw std::logic_error(ss.str());
                }
                if (pos == 1)
                {
                    std::stringstream ss;
                    ss << "l." << lineNo << ": ini parsing failed, section is empty";
                    throw std::logic_error(ss.str());
                }
                currentSection = line.substr(1, pos - 1);
            }
            else
            {
                if (currentSection == "")
                {
                    std::stringstream ss;
                    ss << "l." << lineNo << ": ini parsing failed, field has no section";
                    throw std::logic_error(ss.str());
                }

                std::size_t pos = line.find(fieldSeparator_);

                if (pos == std::string::npos)
                {
                    std::stringstream ss;
                    ss << "l." << lineNo << ": ini parsing failed, no '" << fieldSeparator_ << "' found";
                    throw std::logic_error(ss.str());
                }
                else
                {
                    std::string currentField = line.substr(0, pos);
                    trim(currentField);
                    std::string currentValue = line.substr(pos + 1, std::string::npos);
                    trim(currentValue);

                    if (currentSection == targetSection && currentField == targetField)
                    {
                        value = currentValue;
                        return (value.size() != 0) ? true : false;
                    }
                }
            }
        }

        return false;
    }

    void setField(const std::string &value, const AnsiString &targetSection, const AnsiString &targetField)
    {
        std::stringstream iss, oss;
        {
            std::ifstream ifs(iniFilePath_);
            iss << ifs.rdbuf();
        }

        enum FoundMode
        {
            NO_MATCH = 0,
            SECTION_MATCH = 1,
            BOTH_MATCH = 2
        } found_mode = NO_MATCH;

        int lineNo = 0;
        std::string currentSection = "";
        std::string line;

        // iterate file line by line
        while (!iss.eof() && !iss.fail())
        {
            std::getline(iss, line, '\n');
            trim(line);
            ++lineNo;

            if (isIgnoreLine(line))
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
                    ss << "l." << lineNo << ": ini parsing failed, section not closed";
                    throw std::logic_error(ss.str());
                }
                if (pos == 1)
                {
                    std::stringstream ss;
                    ss << "l." << lineNo << ": ini parsing failed, section is empty";
                    throw std::logic_error(ss.str());
                }
                currentSection = line.substr(1, pos - 1);

                if (currentSection == targetSection)
                    found_mode = SECTION_MATCH;
                else if (found_mode == SECTION_MATCH && currentSection != targetSection)
                {
                    line = targetField.str() + fieldSeparator_ + value + '\n' + line;
                    found_mode = BOTH_MATCH;
                }
            }
            else
            {
                if (currentSection == "")
                {
                    std::stringstream ss;
                    ss << "l." << lineNo << ": ini parsing failed, field has no section";
                    throw std::logic_error(ss.str());
                }

                std::size_t pos = line.find(fieldSeparator_);

                if (pos == std::string::npos)
                {
                    std::stringstream ss;
                    ss << "l." << lineNo << ": ini parsing failed, no '" << fieldSeparator_ << "' found";
                    throw std::logic_error(ss.str());
                }
                else
                {
                    std::string currentField = line.substr(0, pos);
                    trim(currentField);

                    if (currentSection == targetSection && currentField == targetField)
                    {
                        line = std::string(targetField.c_str()) + fieldSeparator_ + value;
                        found_mode = BOTH_MATCH;
                    }
                }
            }
            oss << line << '\n';
        }

        if (found_mode == NO_MATCH)
        {
            oss << '[' << targetSection << ']' << '\n';
            oss << targetField << fieldSeparator_ << value << '\n';
        }
        else if (found_mode == SECTION_MATCH)
        {
            oss << targetField << fieldSeparator_ << value << '\n';
        }

        {
            std::ofstream ofs(iniFilePath_);
            std::string result = oss.str();
            result.pop_back();
            ofs << result;
        }
    }

public:
    TIniFile(const AnsiString &iniFilePath)
        : iniFilePath_(std::string(iniFilePath.c_str())),
          fieldSeparator_('='),
          commentPrefixList_({"#", ";"})
    {
    }

    TIniFile &SetFieldSep(const char fieldSeparator)
    {
        fieldSeparator_ = fieldSeparator;
        return *this;
    }
    TIniFile &SetCommentPrefixes(const std::vector<std::string> &commentPrefixList)
    {
        commentPrefixList_ = commentPrefixList;
        return *this;
    }

    bool ReadBool(const AnsiString &section, const AnsiString &name, const bool &defaultValue)
    {
        std::string str;
        if (!tryGetField(str, section, name))
            return defaultValue;

        std::transform(str.begin(), str.end(), str.begin(), [](const char c)
                       { return static_cast<char>(::toupper(c)); });

        if (str == "TRUE")
            return true;
        else if (str == "FALSE")
            return false;
        else
            return defaultValue;
    }

    int ReadInteger(const AnsiString &section, const AnsiString &name, const double &defaultValue)
    {
        std::string str;
        if (!tryGetField(str, section, name))
            return defaultValue;

        if (!isConvertableToLong(str))
            return defaultValue;

        try
        {
            auto value = std::stol(str);
            return value;
        }
        catch (...)
        {
            return defaultValue;
        }
    }

    AnsiString ReadString(const AnsiString &section, const AnsiString &name, const AnsiString &defaultValue)
    {
        std::string str;
        if (!tryGetField(str, section, name))
            return defaultValue;

        return AnsiString(str);
    }

    double ReadFloat(const AnsiString &section, const AnsiString &name, const double &defaultValue)
    {
        std::string str;
        if (!tryGetField(str, section, name))
            return defaultValue;

        try
        {
            auto value = std::stod(str);
            return value;
        }
        catch (...)
        {
            return defaultValue;
        }
    }

    void WriteBool(const AnsiString &section, const AnsiString &name, const bool &value)
    {
        std::string result = value ? "true" : "false";
        setField(result, section, name);
    }

    void WriteInteger(const AnsiString &section, const AnsiString &name, const int &value)
    {
        std::stringstream ss;
        ss << value;
        setField(ss.str(), section, name);
    }

    void WriteString(const AnsiString &section, const AnsiString &name, const AnsiString &value)
    {
        setField(value.str(), section, name);
    }
};

#endif

#endif