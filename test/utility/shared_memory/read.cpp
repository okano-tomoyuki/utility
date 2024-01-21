#include <iostream>                     // std::cout, std::endl
#include <string>
#include <cstring>                      // std::strcpy
#include <thread>                       // std::thread, std::thread::detach
#include <csignal>                      // signal, SIGINT, SIGERR

#include "utility/shared_memory.hpp"    // Utility::UdpSocket
#include "utility/date_time.hpp"        // Utility::ProcessTimer
#include "./sample_data.hpp"            // Sample

namespace Signal
{
static int end_sign = 0;
void abort_handler(int sig)
{
    end_sign = 1;
    return;
};
}

int main()
{
    using Utility::SharedMemory;
    using Utility::DateTime::ProcessTimer;
    using Utility::DateTime::get_iso_format; // UTC yyyy-mm-ddThh:mi::ss.sssZ 文字列取得関数

    // SharedMemory管理インスタンスの生成
    // 第一引数 : ファイルマッピングオブジェクトのファイルパス
    // 第二引数 : 確保するサイズ(共通変数としたい構造体のサイズ)
    auto shared_memory = SharedMemory(SM_DATA_PATH, sizeof(Sample));

    // 共有したい変数のインスタンス
    auto sample_data = Sample();

    /**! キーボードからの標準入力を非同期で受け取る。 */
    int input;
    std::thread th([&](){
        while(true)
        {
            std::cin >> input;
        }
    });
    th.detach();

    // キーボード割り込み「Ctrl + C」でも終了処理を行う。
    if(signal(SIGINT, Signal::abort_handler) == SIG_ERR)
        exit(1);

    while(!Signal::end_sign)
    {
        // キーボードの「1」押下後「Enter」を押下すると無限ループを抜ける。
        if (input == 1)
            break;

        // 周期動作用タイマーインスタンス
        // 第一引数：タイマー周期(ミリ秒)
        // コンストラクト⇒デストラクトの実行時間を比較し、
        // 設定周期に満たない場合自動で待機する。
        auto timer = ProcessTimer(500);  

        // 書込処理 try_read
        // 第一引数 : 読込変数(参照orポインタ)
        // 第二引数 : タイムアウト(ミリ秒)
        // 書込成功時は trueを、失敗時は falseを返す。
        if(shared_memory.try_read(sample_data, 30))
        {
            std::cout << "[Read]: OK" << std::endl;
            std::cout << "update_time : " << sample_data.update_time << std::endl;
            std::cout << "data        : " << sample_data.d_data      << std::endl;   
        }
        else
        {
            std::cout << "[Read]: NG" << std::endl;
            std::cout << "update_time : " << sample_data.update_time << std::endl;
            std::cout << "data        : " << sample_data.d_data      << std::endl; 
        }

    }

}