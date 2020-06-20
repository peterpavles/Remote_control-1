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
    //创建套接字
    bool Create(int af, int type, int protocol);
    //绑定IP
    bool Bind(const SocketAddr& addr);

    /*udp*/
    //发送数据
    int SendTo(const char* buffer, int length, const SocketAddr& addr);
    //接收数据
    int RecvFrom(char* buffer, int length, SocketAddr& addr);

    /*tcp*/
    //监听最大连接数
    bool Listen(int maxConn);
    //接受连接
    bool Accept(Socket& socket);
    //请求连接
    bool Connect(SocketAddr serverAddr);
    //断开连接
    bool CloseSocket();
    //发送数据
    int Send(const char* buf, int bufSize);
    //接收数据
    int Recv(char* buf, int bufSize);
    //获取错误信息
    string GetErroInfo();
    //获取错误代码
    int GetErroCode();

private:
    int m_socket;
public:
    SocketAddr addr;//保存套接字对应ip，服务器端可忽略
};



//初始化Socket
class SocketInit
{
private:
    SocketInit();
public:
    ~SocketInit();

private:
    static SocketInit m_self;
};