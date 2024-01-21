#include <iostream>                     // std::cout, std::endl

#include "utility/core.hpp"             // Utility::sleep 
#include "utility/udp_socket.hpp"       // Utility::UdpSocket
#include "./sample_data.hpp"            // Sample

int main()
{
    // ソケットインスタンスの生成
    auto udp_socket = Utility::UdpSocket();
    
    // 送信先のポート番号を設定
    // 対象のポートが一つの場合であっても必ず{}で囲むようにする。
    udp_socket.set_target_ports({8000, 8001});
    
    // IPも含めて送信対象を指定する場合は以下のように記述する。
    // udp_socket.set_target_ports({{"127.0.0.1", 8000}, {"127.0.0.1", 8001}});

    // 送信用変数のインスタンスを生成
    auto sample_data = Sample();

    while(true)
    {
        // 送信処理 try_write
        // 送信成功時は trueを、失敗時は falseを返す。
        if(udp_socket.try_write(sample_data))
        {
            // 送信成功時の処理を以下に記述する。
            std::cout << "[Send]:OK" << std::endl;
        }
        else
        {
            // 送信失敗時の処理を以下に記述する。
            std::cout << "[Send]:NG" << std::endl;
        }

        // スリープ処理(ミリ秒単位で指定)
        Utility::sleep(500);
    }

    // デストラクト実行時に自動的にsocketはcloseされる。
}