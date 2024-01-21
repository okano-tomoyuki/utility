#include <iostream>                     // std::cout, std::endl

#include "utility/core.hpp"             // Utility::sleep 
#include "utility/udp_socket.hpp"       // Utility::UdpSocket
#include "./sample_data.hpp"            // Sample

int main()
{
    // ソケットインスタンスの生成
    auto udp_socket = Utility::UdpSocket();
    
    // 受信時のポート番号を設定 8001番ポートを指定
    udp_socket.set_listen_port(8001);

    // 受信タイムアウトを設定(ミリ秒で指定)
    udp_socket.set_timeout(3000);

    // 受信用変数のインスタンスを生成
    auto sample_data = Sample();

    while(true)
    {
        // 受信処理 try_read
        // 受信成功時は trueを、失敗時は falseを返す。
        if(udp_socket.try_read(sample_data))
        {
            // 受信成功時の処理を以下に記述する。
            std::cout << "[Recv]: OK" << std::endl;
        }
        else
        {
            // 受信失敗時の処理を以下に記述する。
            std::cout << "[Recv]: NG" << std::endl;
        }

        // スリープ処理(ミリ秒単位で指定)
        Utility::sleep(500);
    }

    // デストラクト実行時に自動的にsocketはcloseされる。
}