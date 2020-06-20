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

//�����׽���
bool Socket::Create(int af, int type, int protocol)
{
    m_socket = socket(af, type, protocol);
    if (m_socket == INVALID_SOCKET) {
        return false;
    }
    return true;
}

//��IP��ַ�Ͷ˿�
bool Socket::Bind(const SocketAddr& addr)
{ 
    if (bind(m_socket, &addr.ToSockaddr(), sizeof(addr)) != SOCKET_ERROR) {
        return true;
    }
    return false;
}

/*udp*/
//��������
int Socket::SendTo(const char* buffer, int length, const SocketAddr& addr)
{
    return sendto(m_socket, buffer, length, 0, &addr.ToSockaddr(), addr.GetSize());
}
//��������
int Socket::RecvFrom(char* buffer, int length, SocketAddr& addr)
{
    int len = addr.GetSize();
    return recvfrom(m_socket, buffer, length, 0, &addr.ToSockaddr(), &len);
}

/*tcp*/
//����
bool Socket::Listen(int maxConn)
{
    if (listen(m_socket, maxConn) != SOCKET_ERROR) {
        return true;
    }
    return false;
}

//�ȴ��ͷ�������
//�˺���������������ʹ���߳�ִ�д˺���
bool Socket::Accept(Socket& socket)
{
    int len = socket.addr.GetSize();
    socket.m_socket = accept(m_socket, &socket.addr.ToSockaddr(), &len);
    if (socket.m_socket == INVALID_SOCKET) {
        return false;
    }
    return true;
}

//�ͻ������ӷ�����
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

//��������
int Socket::Send(const char* buf, int bufSize)
{
    return send(m_socket, buf, bufSize, 0);
}

//��������
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








//��ʼ��socket�࣬�������
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


