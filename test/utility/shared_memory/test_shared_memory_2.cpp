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

    // 共有メモリインスタンスを生成
    auto sm = SharedMemory(SM_DATA_PATH, sizeof(Sample));
    
    // 共有メモリの構造体ポインタ変数を取得
    auto sample_ptr = sm.get<Sample>();

    // 終了をキャッチするまで継続
    while(!Signal::end_sign)
    {
        auto pt = ProcessTimer::create_auto_wait(500);
        if(sm.wait_for_single_object(sm.mutex(), 30))
        {
            std::cout << "read  success" << std::endl;

            sample_ptr->d_data += 1.1;
            std::strcpy(sample_ptr->update_time, DateTime().to_str("yyyy-mm-ddThh:nn:ss.zzzZ").c_str());

            std::cout << "update_time : " << sample_ptr->update_time << std::endl;
            std::cout << "d_data      : " << sample_ptr->d_data << std::endl;

            std::cout << "write success" << std::endl;

            sm.release_mutex(sm.mutex());
        }
        else
        {
            std::cout << "read  failed" << std::endl;
            std::cout << "write failed" << std::endl;
        }
    }
}