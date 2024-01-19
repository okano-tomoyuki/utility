/**
 * @file process_timer.hpp
 * @author okano tomoyuki (tomoyuki.okano@tsuneishi.com)
 * @brief 
 * @version 0.1
 * @date 2024-01-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef _UTILITY_PROCESS_TIMER_HPP_
#define _UTILITY_PROCESS_TIMER_HPP_

#include <chrono>
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <thread>

#ifdef GLOBAL_USE_BUILD_LIBLARY

namespace Utility
{

/**
 * @class ProcessTimer
 * @brief 自動待機/計測用タイマークラス
 * @note  本クラスは直接コンストラクタを呼び出さず、用途に応じて2種類のFactoryMethodを使用してインスタンス化する。
 * @n @ref create_auto_wait
 * @n @ref create_mesure
 * @n 本ライブラリはSOLIDの原則のうち、単一責任の原則に違反しているため今後変更する可能性がある。
 * @n @link
 * https://qiita.com/baby-degu/items/d058a62f145235a0f007
 * @endlink
 */
class ProcessTimer final
{
using system_clock = std::chrono::system_clock;
public:
    /** @enum WaitType @brief @ref create_auto_wait における待機仕様 */
    enum WaitType { BUSSY, SLEEP };
private:
    int interval_;
    std::string name_;
    system_clock::time_point base_time_;
    enum WaitType wait_type_;
    explicit ProcessTimer(const std::string& name, const int& interval, const enum WaitType& wait_type=BUSSY);
public:

    /**
     * @fn create_auto_wait
     * @brief 一定周期で特定の動作を実行させるための自動待機タイマーを生成するFactory Method
     * 
     * @param int interval 動作周期[ミリ秒]
     * @param enum WaitType wait_type デストラクタ呼出時の待機方法 { BUSSY : スリープせず待機, SLEEP : スリープして待機 }
     * @return ProcessTimer 自動待機タイマーとしてインスタンス化されたProcessTimerインスタンス
     */
    static ProcessTimer create_auto_wait(const int& interval, const enum WaitType& wait_type=BUSSY);

    /**
     * @fn create_mesure
     * @brief 処理時間計測用タイマーインスタンス生成するFactory Method
     * 
     * @param std::string name 処理時間計測用タイマー名 
     * @return ProcessTimer 処理時間計測用タイマーとしてインスタンス化されたProcessTimerインスタンス
     */
    static ProcessTimer create_mesure(const std::string& name);

    /**
     * @fn ~ProcessTimer
     * @brief デストラクタ
     * @note 
     */
    ~ProcessTimer();

    /**
     * @fn restart
     * @brief 計測開始時刻のリセット処理
     */
    void restart();

    /**
     * @fn mesure
     * @brief 計測開始時刻からの経過時間取得
     * @return std::string 計測開始時刻からの経過時間文字列 時:分:秒.ミリ秒
     */
    std::string measure() const;
};

}

#else

namespace Utility
{

class ProcessTimer final
{
using system_clock = std::chrono::system_clock;
public:
    enum WaitType { BUSSY, SLEEP };
private:
    int interval_;
    std::string name_;
    system_clock::time_point base_time_;
    enum WaitType wait_type_;
    explicit ProcessTimer(const std::string& name, const int& interval, const enum WaitType& wait_type=BUSSY)
     : name_(std::string()), interval_(interval), wait_type_(wait_type), base_time_(system_clock::now())
    {}

public:

    static ProcessTimer create_auto_wait(const int& interval, const enum WaitType& wait_type=BUSSY)
    {
        if(interval<=0)
        {
            std::stringstream ss;
            ss << "Exception throw by " << __func__ << ".";
            ss << "If you use auto wait timer instance, interval value must be larger than 0."; 
            throw std::runtime_error(ss.str());
        }
        return ProcessTimer(std::string(), interval, wait_type);
    }

    static ProcessTimer create_mesure(const std::string& name)
    {
        return ProcessTimer(name, 0, BUSSY);
    }

    ~ProcessTimer()
    {
        using std::chrono::duration_cast;
        using std::chrono::milliseconds;

        if(interval_>0)
        {
            if(wait_type_ == BUSSY)
            {
                while(true)
                    if(system_clock::now() > base_time_ + milliseconds{interval_})
                        break;
            }
            else // wait_type_ == SLEEP
            {
                auto waste = milliseconds{interval_ - duration_cast<milliseconds>(system_clock::now()-base_time_).count()};
                std::this_thread::sleep_for(waste);
            }
        }
        else
            std::cout << name_ << " : mesured time " << measure() << std::endl;
    }

    void restart()
    {
        base_time_ = system_clock::now();
    }

    std::string measure() const
    {
        using std::chrono::duration_cast;
        using std::chrono::hours;
        using std::chrono::minutes;
        using std::chrono::seconds;
        using std::chrono::milliseconds;
        using std::chrono::microseconds;

        auto current_time = system_clock::now();
        auto pass_hour = duration_cast<hours>((current_time - base_time_)).count();
        auto pass_min  = duration_cast<minutes>((current_time - base_time_)).count();
        auto pass_sec  = duration_cast<seconds>((current_time - base_time_)).count();
        auto pass_msec = duration_cast<milliseconds>((current_time - base_time_)).count();

        std::stringstream ss;
        ss << pass_hour << ":" 
            << pass_min - pass_hour * 60 << ":" 
            << pass_sec - pass_min  * 60 << "."
            << std::string(3-std::to_string(pass_msec-pass_sec*1000).length(),'0') << pass_msec-pass_sec*1000;

        return ss.str();
    }
};

}

#endif

#endif