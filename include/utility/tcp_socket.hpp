#ifndef _UTILITY_TCP_SOCKET_HPP_
#define _UTILITY_TCP_SOCKET_HPP_

#include <iostream> 
#include <cstdlib>  
#include <sstream>
#include <string>

#ifdef __unix__
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <unistd.h>
#else
#include <conio.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#ifdef GLOBAL_USE_BUILD_LIBLARY

namespace Utility
{

class TcpSocket final
{

#ifdef __unix__
using SockLen = socklen_t;
using Sock = int;
#else
using SockLen = int;
using Sock = SOCKET;
#endif

private:
    Sock sock_;
    bool is_open_;
    explicit TcpSocket(const Sock& sock);

public:

    /**
     * @fn create_server
     * @brief サーバー側インスタンスを生成するFactory Method
     * 
     * @param int port listenするポート番号 
     * @return TcpSocket サーバーとしてインスタンス化されたTcpSocketインスタンス
     */
    static TcpSocket create_server(const int& port);

    /**
     * @fn create_client
     * @brief クライアント側インスタンスを生成するFactory Method
     * 
     * @param std::string ip_address 接続先IP アドレス
     * @param int port 接続先ポート番号
     * @return TcpSocket サーバーとしてインスタンス化されたTcpSocketインスタンス
     */
    static TcpSocket create_client(const std::string& ip_address, const int& port);

    /**
     * @fn template<typename T> try_write(const T& data)
     * @brief 任意の構造体データを送信するメソッド
     * 
     * @tparam T 
     * @param T data 送信する任意の構造体データ 
     * @return true 送信成功
     * @return false 送信失敗
     */
    template<typename T>
    bool try_write(const T& data);

    /**
     * @fn template<typename T> try_read(const T& data)
     * @brief 任意の構造体データを受信するメソッド
     * 
     * @tparam T 
     * @param T data 受信する任意の構造体データ 
     * @return true 受信成功
     * @return false 受信失敗
     */
    template<typename T>
    bool try_read(T& data);

    /**
     * @fn terminate
     * @brief ソケットをクローズする。
     * @note デストラクタを呼び出さずソケットをクローズする必要がある場合に明示的に呼び出す。
     */
    void terminate();
};

}

#else

namespace Utility
{

class TcpSocket final
{

#ifdef __unix__
using SockLen = socklen_t;
using Sock = int;
#else
using SockLen = int;
using Sock = SOCKET;
#endif

private:
    Sock sock_;
    bool is_open_;
    explicit TcpSocket(const Sock& sock)
     : sock_(sock)
    {}
public:

    static TcpSocket create_server(const int& port)
    {
#ifndef __unix__
        WSADATA wsa_data;
        auto ret = WSAStartup(MAKEWORD(2, 0), &wsa_data);
        if(ret != 0)
        {
            std::stringstream ss;
            ss << "TCP Socket initialize failed. Error Code : " << WSAGetLastError() << std::endl;
            throw std::runtime_error(ss.str());
        }
#endif
        Sock sock0, sock;
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
#ifdef __unix__
        addr.sin_addr.s_addr = INADDR_ANY;
#else
        addr.sin_addr.S_un.S_addr = INADDR_ANY;
#endif
        SockLen len = sizeof(struct sockaddr_in);
        sock0 = socket(AF_INET, SOCK_STREAM, 0);
        bind(sock0, (struct sockaddr*)&addr, sizeof(addr));
        listen(sock0, 5);
        struct sockaddr_in client;
        sock = accept(sock0, (struct sockaddr*)&client, &len);
        return TcpSocket(sock);
    }

    static TcpSocket create_client(const std::string& ip_address, const int& port)
    {
#ifndef __unix__
        WSADATA wsaData;
        auto ret = WSAStartup(MAKEWORD(2, 0), &wsaData);
        if (ret != 0) 
        {
            std::stringstream ss;
            ss << "TCP Socket initialize failed. Error Code : " << WSAGetLastError();
            throw std::runtime_error(ss.str());
        }
#endif
        auto sock = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, ip_address.c_str(), &addr.sin_addr.s_addr);
        connect(sock, (struct sockaddr*)&addr, sizeof(addr));
        return TcpSocket(sock);
    }


    /**
     * @fn template<typename T> try_write(const T& data)
     * @brief 任意の構造体データを送信するメソッド
     * 
     * @tparam T 
     * @param T data 送信する任意の構造体データ 
     * @return true 送信成功
     * @return false 送信失敗
     */
    template<typename T>
    bool try_write(const T& data)
    {
        auto ret = send(sock_, (char*)data, sizeof(data), 0);
        if(ret != sizeof(data))
        {
            std::stringstream ss;
#ifdef __unix__
            ss << "TCP Socket send operation failed. Error Code: " << errno << std::endl;
#else
            ss << "TCP Socket send operation failed. Error Code: " << WSALastError() << std::endl;
#endif
            throw std::runtime_error(ss.str());
        }
    }

    /**
     * @fn template<typename T> try_read(const T& data)
     * @brief 任意の構造体データを受信するメソッド
     * 
     * @tparam T 
     * @param T data 受信する任意の構造体データ 
     * @return true 受信成功
     * @return false 受信失敗
     */
    template<typename T>
    bool try_read(T& data);

    /**
     * @fn terminate
     * @brief ソケットをクローズする。
     * @note デストラクタを呼び出さずソケットをクローズする必要がある場合に明示的に呼び出す。
     */
    void terminate();
};

}

#endif

#endif // _UTILITY_TCP_SOCKET_HPP_