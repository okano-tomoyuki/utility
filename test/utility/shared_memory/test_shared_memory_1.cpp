/**
 * @file test_shared_memory_1.cpp
 * @author okano tomoyuki (tomoyuki.okano@tsuneishi.com)
 * @brief @ref Utility::SharedMemory クラスのテストコード及びクライアントコード例
 * @version 0.1
 * @date 2024-01-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>
#include <csignal>

#include "./sample_data.hpp"
#include "utility/date_time.hpp"
#include "utility/process_timer.hpp"
#include "utility/shared_memory.hpp"


// シグナル管理用namespace
namespace Signal
{
// 終了シグナル 1:終了 0:継続
static int end_sign = 0;

// 終了シグナルキャッチ時のハンドラー
void abort_handler(int sig)
{
    end_sign = 1;
    return;
};
}


int main()
{
    using Utility::SharedMemory;
    using Utility::DateTime;
    using Utility::ProcessTimer;

    // キーボード割り込み「Ctrl + C」でSignal::abort_handlerに通知
    if(signal(SIGINT, Signal::abort_handler) == SIG_ERR)
        exit(1);

    auto sm = SharedMemory(SM_DATA_PATH, sizeof(Sample));
    auto sample = Sample();

    // 終了をキャッチするまで継続
    while(!Signal::end_sign)
    {
        auto pt = ProcessTimer::create_auto_wait(500);
        if(sm.try_read(sample, 30))
        {
            std::cout << "read  success" << std::endl;
            std::cout << "update_time : " << sample.update_time << std::endl;
            std::cout << "d_data      : " << sample.d_data << std::endl;

            sample.d_data += 1.1;
            std::strcpy(sample.update_time, DateTime().to_str("yyyy-mm-ddThh:nn:ss.zzzZ").c_str());

            if(sm.try_write(sample, 30))
                std::cout << "write success" << std::endl;
            else
                std::cout << "write failed" << std::endl;
        }
        else
        {
            std::cout << "read  failed" << std::endl;
            std::cout << "write failed" << std::endl;
        }
    }
}