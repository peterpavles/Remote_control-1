#pragma once
#include "SocketAddr.h"

class Socket
{
public:
    Socket();
    Socket(Socket& other);
    Socket(int socket);
    ~Socket();
public:
    //�����׽���
    bool Create(int af, int type, int protocol);
    //��IP
    bool Bind(const SocketAddr& addr);

    /*udp*/
    //��������
    int SendTo(const char* buffer, int length, const SocketAddr& addr);
    //��������
    int RecvFrom(char* buffer, int length, SocketAddr& addr);

    /*tcp*/
    //�������������
    bool Listen(int maxConn);
    //��������
    bool Accept(Socket& socket);
    //��������
    bool Connect(SocketAddr serverAddr);
    //�Ͽ�����
    bool CloseSocket();
    //��������
    int Send(const char* buf, int bufSize);
    //��������
    int Recv(char* buf, int bufSize);
    //��ȡ������Ϣ
    string GetErroInfo();
    //��ȡ�������
    int GetErroCode();

private:
    int m_socket;
public:
    SocketAddr addr;//�����׽��ֶ�Ӧip���������˿ɺ���
};



//��ʼ��Socket
class SocketInit
{
private:
    SocketInit();
public:
    ~SocketInit();

private:
    static SocketInit m_self;
};