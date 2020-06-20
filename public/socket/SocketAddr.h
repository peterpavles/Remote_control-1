#pragma once
#include <WS2tcpip.h>
#include <stdint.h>
#include <string>

using namespace std;


class SocketAddr
{
public:
    SocketAddr();
    SocketAddr(const string ipaddr, const uint16_t port);
    SocketAddr(const uint32_t ipaddr, const uint16_t port);
    SocketAddr(const SocketAddr& other);
    ~SocketAddr();
    
    bool operator==(const SocketAddr& other);
    //sockaddr_in ==> sockaddr
    sockaddr& ToSockaddr() const;
    //获取端口
    uint16_t GetPort() const;
    //获取ip
    uint32_t GetIpAddress() const;
    //将ip端口转换string
    string ToString() const;
    //结构体大小
    uint32_t GetSize() const;
private:
    //ip地址结构体
    sockaddr_in m_sockAddr;
};

