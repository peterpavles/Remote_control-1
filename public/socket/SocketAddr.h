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
    //��ȡ�˿�
    uint16_t GetPort() const;
    //��ȡip
    uint32_t GetIpAddress() const;
    //��ip�˿�ת��string
    string ToString() const;
    //�ṹ���С
    uint32_t GetSize() const;
private:
    //ip��ַ�ṹ��
    sockaddr_in m_sockAddr;
};

