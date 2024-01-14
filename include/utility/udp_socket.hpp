#ifndef _UTILITY_UDP_SOCKET_HPP_
#define _UTILITY_UDP_SOCKET_HPP_

#include <iostream> // cout endl
#include <cstdlib>  // atoi
#include <sstream>
#include <string>
#include <vector>
#include <tuple>
#include <cstring>
#include <iomanip>

#ifdef __unix__
#include <sys/types.h>
#include <sys/socket.h> // socket, bind, sendto, recv
#include <netinet/in.h> // sockaddr_in
#include <arpa/inet.h>  // inet_addr
#include <unistd.h>     // close
#else
#include <conio.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#ifdef GLOBAL_USE_HEADER_ONLY

#else

namespace Utility
{

class UdpSocket final
{

#ifdef __unix__
using SockLen = socklen_t;
using Sock = int;
    void set_sockaddr_ipv4(struct sockaddr_in &addr, const in_addr_t &type) { addr.sin_addr.s_addr = type; }
#else
using SockLen = int;
using Sock = SOCKET;
    void set_sockaddr_ipv4(struct sockaddr_in &addr, const unsigned long &type) { addr.sin_addr.S_un.S_addr = type; }
#endif

private: 
    Sock sock_;                                         /**! ソケットインスタンス   */
    std::vector<struct sockaddr_in> addr_;              /**! 送信先アドレス        */

public:

    UdpSocket()
    {
#ifndef __unix__
        WSADATA wsa_data;
        int e = WSAStartup(MAKEWORD(2, 0), &wsa_data);
        if (e != 0)
            throw std::runtime_error("failed to initialize Winsock DLL");
#endif
        sock_ = socket(AF_INET, SOCK_DGRAM, 0);
    }

    ~UdpSocket()
    {
#ifdef __unix__
        close(sock_);
#else
        closesocket(sock_);
        WSACleanup();
#endif
    }

    /**! 受信ポートを設定 */
    UdpSocket& set_listen_port(const int& port)
    {
        struct sockaddr_in addr_in;
        addr_in.sin_family = AF_INET;
        addr_in.sin_port = htons(port);
        set_sockaddr_ipv4(addr_in, INADDR_ANY);
        bind(sock_, (struct sockaddr *)&addr_in, sizeof(addr_in));
        return *this;
    }

    /**! 送信先アドレス・ポートを設定 */
    UdpSocket& set_target_ports(const std::vector<std::tuple<std::string, int>>& targets)
    {
        for (const auto &t : targets)
        {
            std::string target_ip = std::get<0>(t);
            int target_port = std::get<1>(t);
            struct sockaddr_in ad;
            ad.sin_family = AF_INET;
            ad.sin_port = htons(target_port);
            set_sockaddr_ipv4(ad, inet_addr(target_ip.c_str()));
            if (target_ip == "255.255.255.255")
            {
                int yes = 1;
                setsockopt(sock_, SOL_SOCKET, SO_BROADCAST, (char *)&yes, sizeof(yes));
            }
            addr_.push_back(ad);
        }
        return *this;
    }

    /**! 送信先ポートを設定(ローカルホスト用) */
    UdpSocket& set_target_ports(const std::vector<int>& target_ports)
    {
        std::vector<std::tuple<std::string, int>> targets;
        for (const auto &p : target_ports)
        {
            std::tuple<std::string, int> target;
            std::get<0>(target) = "127.0.0.1";
            std::get<1>(target) = p;
            targets.push_back(target);
        }
        set_target_ports(targets);
        return *this;
    }

    /**! 受信タイムアウト設定 */
    UdpSocket& set_timeout(const double& timeout_msec)
    {
        if (timeout_msec <= 0)
            throw std::runtime_error("timeout must be lager than 0.");

        int sec = timeout_msec / 1000.;
        int u_sec = 1000000 * (timeout_msec / 1000. - (int)(timeout_msec / 1000.));
#ifdef __unix__
        struct timeval tv {sec, u_sec};
        if (setsockopt(sock_, SOL_SOCKET, SO_RCVTIMEO, (char *)(&tv), sizeof(tv)) < 0)
            throw std::runtime_error("time out setting is unknown");
#else
        DWORD dw = timeout_msec;
        if (setsockopt(sock_, SOL_SOCKET, SO_RCVTIMEO, (char *)(&dw), sizeof(dw)) < 0)
            throw std::runtime_error("time out setting is unknown");
#endif
        return *this;
    }

    /**! 固定長データをchar*データとして送信 */
    bool try_write(const char *buffer, const int &len) const
    {
        if (addr_.size() < 1)
            std::runtime_error("No target is setted.");
        for (auto addr : addr_)
            if (sendto(sock_, buffer, len, 0, (struct sockaddr *)&addr, sizeof(addr)) != len)
                return false;
        return true;
    }

    /**! 構造体データを送信 */
    template <typename T>
    bool try_write(const T& data) const
    {
        if (addr_.size() < 1)
            std::runtime_error("No target is setted.");
        for (auto addr : addr_)
            if (sendto(sock_, &data, sizeof(data), 0, (struct sockaddr *)&addr, sizeof(addr)) != sizeof(data))
                return false;
        return true;
    }

    /**! 可変長データをstringとして取得 */
    bool try_read_string(std::string& buffer, const int& buffer_capacity=1024) const
    {
        char* tmp = new char[buffer_capacity];
        int len;
        if(len = recv(sock_, tmp, buffer_capacity, 0) < 1)
        {
            delete[] tmp;
            return false;
        }
        buffer = std::string(tmp, len);
        delete[] tmp;
        return true;
    }

    /**! 固定長データをchar*として受信 */
    bool try_read(char *buffer, const int &len) const
    {
        char *tmp = new char[len];
        int remain = len;
        int completed = 0;
        while (completed < len)
        {
            int r = recv(sock_, tmp + completed, remain, 0);
            if (r < 1)
            {
                delete[] tmp;
                return false;
            }
            completed += r;
            remain -= r;
        };
        std::memcpy(buffer, tmp, len);
        delete[] tmp;
        return true;
    }

    /**! 構造体データを受信 */
    template <typename T>
    bool try_read(T &data) const
    {
        int len = sizeof(T);
        char *tmp = new char[len];
        int remain = len;
        int completed = 0;
        while (completed < len)
        {
            int r = recv(sock_, tmp + completed, remain, 0);
            if (r < 1)
            {
                delete[] tmp;
                return false;
            }
            completed += r;
            remain -= r;
        };
        std::memcpy(&data, tmp, len);
        delete[] tmp;
        return true;
    }
};

}

#endif

#endif // _UTILITY_UDP_SOCKET_HPP_