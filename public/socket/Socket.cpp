#include "Socket.h"

Socket::Socket()
{
    m_socket = -1;
}

Socket::Socket(Socket& other)
{
    m_socket = other.m_socket;
    addr = other.addr;
}

Socket::Socket(int socket)
{
    m_socket = socket;
}

Socket::~Socket()
{

    if (m_socket > 0) {
        CloseSocket();
        m_socket = -1;
    }
}

//创建套接字
bool Socket::Create(int af, int type, int protocol)
{
    m_socket = socket(af, type, protocol);
    if (m_socket == INVALID_SOCKET) {
        return false;
    }
    return true;
}

//绑定IP地址和端口
bool Socket::Bind(const SocketAddr& addr)
{ 
    if (bind(m_socket, &addr.ToSockaddr(), sizeof(addr)) != SOCKET_ERROR) {
        return true;
    }
    return false;
}

/*udp*/
//发送数据
int Socket::SendTo(const char* buffer, int length, const SocketAddr& addr)
{
    return sendto(m_socket, buffer, length, 0, &addr.ToSockaddr(), addr.GetSize());
}
//接收数据
int Socket::RecvFrom(char* buffer, int length, SocketAddr& addr)
{
    int len = addr.GetSize();
    return recvfrom(m_socket, buffer, length, 0, &addr.ToSockaddr(), &len);
}

/*tcp*/
//监听
bool Socket::Listen(int maxConn)
{
    if (listen(m_socket, maxConn) != SOCKET_ERROR) {
        return true;
    }
    return false;
}

//等待客服端连接
//此函数将阻塞，建议使用线程执行此函数
bool Socket::Accept(Socket& socket)
{
    int len = socket.addr.GetSize();
    socket.m_socket = accept(m_socket, &socket.addr.ToSockaddr(), &len);
    if (socket.m_socket == INVALID_SOCKET) {
        return false;
    }
    return true;
}

//客户端连接服务器
bool Socket::Connect(SocketAddr serverAddr)
{
    if (connect(m_socket, &serverAddr.ToSockaddr(), serverAddr.GetSize()) == SOCKET_ERROR) {
        return false;
    }
    return true;
}

bool Socket::CloseSocket()
{
    shutdown(m_socket, SD_BOTH);
    if (closesocket(m_socket) == SOCKET_ERROR) {
        return false;
    }
    return true;
}

//发送数据
int Socket::Send(const char* buf, int bufSize)
{
    return send(m_socket, buf, bufSize, 0);
}

//接收数据
int Socket::Recv(char* buf, int bufSize)
{
    return recv(m_socket, buf, bufSize, 0);
}

string Socket::GetErroInfo()
{
    int error = GetErroCode();
    LPVOID lpMsgBuf;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPSTR)& lpMsgBuf,
        0,
        NULL);
    return string((char*)lpMsgBuf);
}

int Socket::GetErroCode()
{
    return WSAGetLastError();
}








//初始化socket类，无须调用
SocketInit SocketInit::m_self;

SocketInit::SocketInit()
{
    WSADATA wsaData;
    int iResult;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
   /* if (iResult != 0) {
        SocketError::PrintError("SocketInit::SocketInit");
    }*/
}

SocketInit::~SocketInit()
{
    WSACleanup();
}


