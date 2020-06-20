#include "SocketAddr.h"

SocketAddr::SocketAddr()
{
    memset(&this->m_sockAddr, 0, sizeof(this->m_sockAddr));
}

SocketAddr::SocketAddr(const string ipaddr, const uint16_t port)
{
    this->m_sockAddr.sin_family = AF_INET;
    this->m_sockAddr.sin_port = htons(port);
    //inet_pton ��ipת��ΪСβ����long
    inet_pton(AF_INET, ipaddr.c_str(), &this->m_sockAddr.sin_addr);
}

SocketAddr::SocketAddr(const uint32_t ipaddr, const uint16_t port)
{
    this->m_sockAddr.sin_family = AF_INET;
    this->m_sockAddr.sin_port = htons(port);
    this->m_sockAddr.sin_addr.S_un.S_addr = htonl(ipaddr);
}

SocketAddr::SocketAddr(const SocketAddr& other)
{
    memcpy(&this->m_sockAddr, &other, sizeof(other));
}

SocketAddr::~SocketAddr()
{
}

bool SocketAddr::operator==(const SocketAddr& other)
{
    return memcmp(&this->m_sockAddr, &other.m_sockAddr, sizeof(sockaddr_in)) == 0;
}

//sockaddr_in ==> sockaddr
sockaddr& SocketAddr::ToSockaddr() const
{
    return (sockaddr&)this->m_sockAddr;
}

//��ȡ�˿�
uint16_t SocketAddr::GetPort() const
{
    //htonsת����Сβ����unsigned short
    return htons(this->m_sockAddr.sin_port);
}

//��ȡip
uint32_t SocketAddr::GetIpAddress() const
{
    //htonlת����Сβ����unsigned long
    return htonl(this->m_sockAddr.sin_addr.S_un.S_addr);
}

//��ip�˿�ת��string
string SocketAddr::ToString() const
{
    char szIpStr[256] = { 0 };
    //inet_ntop ��ipת��Ϊchar
    inet_ntop(AF_INET, &this->m_sockAddr.sin_addr, szIpStr, sizeof(szIpStr));
    
    string str(szIpStr);
    str += ":";
    str += to_string(this->GetPort());

    return str;
}

uint32_t SocketAddr::GetSize() const
{
    return sizeof(this->m_sockAddr);
}
