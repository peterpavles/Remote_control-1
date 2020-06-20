
// RemoteClientDlg.cpp: 实现文件
//

#include <io.h>
#include "pch.h"
#include "framework.h"
#include "RemoteClient.h"
#include "RemoteClientDlg.h"
#include "afxdialogex.h"
#include "../public/zlib/zlib.h";
#pragma comment(lib, "../public/zlib/zlib.lib")

#define MAKE_i64(hi, lo) ((LONGLONG(DWORD(hi) & 0xffffffff) << 32 ) | LONGLONG(DWORD(lo) & 0xffffffff))
#define GB(x) (x.HighPart << 2) + (x.LowPart >> 20) / 1024.0   // 转换为GB单位显示
#define SIZE 4069

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRemoteClientDlg 对话框



CRemoteClientDlg::CRemoteClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REMOTECLIENT_DIALOG, pParent)
    , m_client(NULL)
    , ThreadOut(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CRemoteClientDlg::~CRemoteClientDlg()
{
}

void CRemoteClientDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_IPADDRESS1, m_ip);
    DDX_Control(pDX, IDC_EDIT1, m_port);
    DDX_Control(pDX, IDC_BUTTON1, m_btnConnect);
    DDX_Control(pDX, IDC_BUTTON2, m_btnDisconnect);
}

BEGIN_MESSAGE_MAP(CRemoteClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_BUTTON1, &CRemoteClientDlg::OnBnClickedConnect)
    ON_BN_CLICKED(IDC_BUTTON2, &CRemoteClientDlg::OnBnClickedDisconnect)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CRemoteClientDlg 消息处理程序

BOOL CRemoteClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

    m_client = new Socket;

    m_btnDisconnect.EnableWindow(false);
    m_ip.SetAddress(127, 0, 0, 1);
    m_port.SetLimitText(5);
    m_port.SetWindowTextW(_T("6697"));

    //创建管道
    //管道输入结构体
    SECURITY_ATTRIBUTES sa;
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    sa.nLength = sizeof(sa);
    //子写父读管道
    ::CreatePipe(&this->read, &this->CWrite, &sa, 0);
    //父写子读管道
    ::CreatePipe(&this->CRead, &this->write, &sa, 0);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRemoteClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CRemoteClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRemoteClientDlg::OnCancel()
{
}


void CRemoteClientDlg::OnOK()
{
}


void CRemoteClientDlg::OnClose()
{
    ThreadOut = true;
    KillTimer(CMDTIMER);
    EndDialog(0);
}


void CRemoteClientDlg::OnBnClickedConnect()
{
    CString port;
    m_port.GetWindowText(port);
    int nPort = _ttoi(port);
    if (nPort < 1024 || nPort > 65535) {
        MessageBox(_T("请输入一个 1024 到 65535 之间的整数")
            , _T("错误"), MB_ICONERROR);
        return;
    }
    DWORD ip;
    m_ip.GetAddress(ip);
    SocketAddr addr(ip, nPort);

    if (!m_client->Create(AF_INET, SOCK_STREAM, IPPROTO_TCP)) {
        MessageBox(_T("套接字创建失败！"), _T("错误"), MB_ICONERROR);
        return;
    }

    if (!m_client->Connect(addr)) {
        CString str(m_client->GetErroInfo().c_str()/*"与服务器 "*/);
       /* str += addr.ToString().c_str();
        str += " 连接失败！";*/
        
        MessageBox(str, _T("错误"), MB_ICONERROR);
        return;
    }
    ThreadOut = false;
    //创建线程
    ::CreateThread(NULL, 0, CRemoteClientDlg::RecvThread, this, 0, NULL);


    m_ip.EnableWindow(false);
    m_port.EnableWindow(false);
    m_btnConnect.EnableWindow(false);
    m_btnDisconnect.EnableWindow(true);

}


void CRemoteClientDlg::OnBnClickedDisconnect()
{
    if (!m_client->CloseSocket()) {
        MessageBox(_T("无法与服务器断开连接！"), _T("错误"), MB_ICONERROR);
        return;
    }
    ThreadOut = true;
    KillTimer(CMDTIMER);
    //程序退出终止cmd进程
    ::TerminateProcess(this->hCmd, 0);
    m_ip.EnableWindow(true);
    m_port.EnableWindow(true);
    m_btnConnect.EnableWindow(true);
    m_btnDisconnect.EnableWindow(false);
}

int CRemoteClientDlg::GetRecv(char* buff, int buffLen)
{
    int totalSize = buffLen;
    int curSize = 0;
    int tmpSize = 0;
    while (true) {
        tmpSize = m_client->Recv(buff + curSize, totalSize - curSize);
        if (tmpSize == SOCKET_ERROR) {
            return -1;
        }
        else if (tmpSize == 0) {
            return 0;
        }
        curSize += tmpSize;
        if (curSize == totalSize) {
            return 1;
        }
    }
}

DWORD __stdcall CRemoteClientDlg::RecvThread(LPVOID lpParameter)
{
    CRemoteClientDlg* dlg = (CRemoteClientDlg*)lpParameter;
    Server_Head head;

    while (true) {
        if (dlg->ThreadOut) {
            dlg->KillTimer(TIMERID);
            break;
        }
        int ret = dlg->m_client->Recv((char*)& head, sizeof(head));
        if (ret == SOCKET_ERROR) {
            if (dlg->m_client->GetErroCode() == WSAECONNRESET) {
                dlg->m_ip.EnableWindow(true);
                dlg->m_port.EnableWindow(true);
                dlg->m_btnConnect.EnableWindow(true);
                dlg->m_btnDisconnect.EnableWindow(false);
                dlg->KillTimer(TIMERID);
                dlg->KillTimer(CMDTIMER);
                AfxMessageBox(_T("与服务器断开连接！"));
                break;
            }
            continue;
        }
        else if (ret == 0) {
            dlg->m_ip.EnableWindow(true);
            dlg->m_port.EnableWindow(true);
            dlg->m_btnConnect.EnableWindow(true);
            dlg->m_btnDisconnect.EnableWindow(false);
            dlg->KillTimer(TIMERID);
            dlg->KillTimer(CMDTIMER);
            AfxMessageBox(_T("与服务器断开连接！"));
            break;
        }

        dlg->DoData(head);
    }
    return 0;
}




void CRemoteClientDlg::DoData(Server_Head& head)
{
    switch (head.cmd)
    {
    case CMD::PRINTSCREEN:
        SetTimer(1, 30, NULL);
        break;
    case CMD::STOPPRINTSCREEN:
        KillTimer(TIMERID);
        break;
    case CMD::MOUSEEVENT:
        DoMouseEvent();
        break;
    case CMD::KEYEVENT:
        DoKeyEvent();
        break;
    case CMD::STARTCMD:
        StartCmd();
        break;
    case CMD::STOPCMD:
        KillTimer(CMDTIMER);
        break;
    case CMD::CMDLINE:
        DoCmdLine(head.dataLen);
        break;
    case CMD::CMDCTRL:
        CmdCtrl(head.dataLen);
        break;
    case CMD::DISKINFO:
        DoDiskInfo();
        break;
    case CMD::FILEDIR:
        DoFileInfo(head.dataLen);
        break;
    case CMD::DOWNFILE:
        DoDownFile(head.dataLen);
        break;
    case CMD::UPFILE:
        DoUpFile(head.dataLen);
        break;
    default:
        break;
    }
}


void CRemoteClientDlg::StartPS()
{
    int width = ::GetSystemMetrics(SM_CXSCREEN);
    int height = ::GetSystemMetrics(SM_CYSCREEN);
    //获取桌面dc
    HDC dc = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
    //2.创建内存兼容dc
    HDC hMemory = CreateCompatibleDC(dc);
    //创建兼容位图
    HBITMAP hBitmap = CreateCompatibleBitmap(dc,
        GetDeviceCaps(dc, HORZRES),
        GetDeviceCaps(dc, VERTRES));

    SelectObject(hMemory, hBitmap);
    //拷贝位图数据
    BitBlt(hMemory,0,0, width,height, dc, 0,  0,SRCCOPY);

    //发送图片
    //获取图片像素信息
    int size = width * height * sizeof(COLORREF);
    char* data = new char[size];
    GetBitmapBits(hBitmap, size, data);

    //压缩图片
    uLongf zipSize = size;
    char* pZip = new char[size];
    compress((unsigned char*)pZip, &zipSize, (unsigned char*)data, size);
    delete[] data;

    //填写图片信息
    Screen_Info wh;
    wh.width = width;
    wh.height = height;
    wh.dataSize = size;

    //填写头部信息
    Server_Head head;
    head.cmd = CMD::PRINTSCREEN;
    head.dataLen = zipSize;
    m_client->Send((char*)&head, sizeof(head)); //发送头部
    m_client->Send((char*)& wh, sizeof(wh));    //发送图片信息
    m_client->Send(pZip, zipSize);              //发送压缩后的图片数据

    delete[] pZip;
    DeleteDC(dc);
    DeleteDC(hMemory);
    DeleteObject(hBitmap);
}


void CRemoteClientDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == TIMERID) {
        StartPS();
    }
    else if (nIDEvent == CMDTIMER){
        ReadCmd();
    }
}


void CRemoteClientDlg::DoMouseEvent()
{
    MouseEvent me;
    m_client->Recv((char*)& me, sizeof(me));

    switch (me.mouseLeg)
    {
    case MOUSE::LBUTTONDOWN:
    {
        ::SetCursorPos(me.x, me.y);
        ::mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
        break;
    }
    case MOUSE::LBUTTONUP:
    {
        ::SetCursorPos(me.x, me.y);
        ::mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, NULL);
        break;
    }
    case MOUSE::RBUTTONDOWN:
    {
        ::SetCursorPos(me.x, me.y);
        ::mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, NULL);
        break;
    }
    case MOUSE::RBUTTONUP:
    {
        ::SetCursorPos(me.x, me.y);
        ::mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, NULL);
        break;
    }
    case MOUSE::LBUTTONDOUBLE:
        ::SetCursorPos(me.x, me.y);
        ::mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
        ::mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, NULL);
        ::mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
        ::mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, NULL);
        break;
    case MOUSE::RBUTTONDOUBLE:
        ::SetCursorPos(me.x, me.y);
        ::mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, NULL);
        ::mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, NULL);
        ::mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, NULL);
        ::mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, NULL);
        break;
    case MOUSE::MOUSEMOVE:
        ::SetCursorPos(me.x, me.y);
    default:
        break;
    }
}


void CRemoteClientDlg::DoKeyEvent()
{
    KeysEvent ke;
    m_client->Recv((char*)& ke, sizeof(ke));

    switch (ke.keyLeg)
    {
    case KEY::KEYDOWN:
        keybd_event(ke.data, 0, 0, 0); //按下
        
        break;
    case KEY::KEYUP:
        keybd_event(ke.data, 0, KEYEVENTF_KEYUP, 0); //弹起
        break;
    default:
        break;
    }
}


void CRemoteClientDlg::StartCmd()
{
    SetTimer(CMDTIMER, 16, NULL);
    //输入结构体
    STARTUPINFOA si = { 0 };
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.hStdInput = this->CRead;
    si.hStdOutput = this->CWrite;
    si.hStdError = this->CWrite;
    si.wShowWindow = SW_HIDE;

    //输出结构体
    PROCESS_INFORMATION pi = { 0 };

    BOOL bRet = ::CreateProcessA(NULL,
        "cmd",
        NULL,
        NULL,
        TRUE,
        0,
        NULL,
        NULL,
        &si,
        &pi);
    if (!bRet)
    {
        MessageBox(_T("进程创建失败"), _T("错误"), MB_ICONERROR);
    }
    else {
        
        //进程初始化成功保存进程句柄和进程ID
        this->hCmd = pi.hProcess;
        this->proID = pi.dwProcessId;
    }
}


void CRemoteClientDlg::ReadCmd()
{
    //读取正确执行的返回信息
    DWORD dwAail = 0;
    DWORD readF = 0;
    //读取管道数据，读取后不会清楚数据，用来判断管道是否存在数据
    ::PeekNamedPipe(this->read, NULL, 0, NULL, &dwAail, NULL);
    if (dwAail > 0)
    {
        //从管道读取数据
        char szBuffRead[0x1000] = { 0 };
        //读取管道数据
        ::ReadFile(this->read, szBuffRead, sizeof(szBuffRead), &readF, NULL);
        Server_Head head;
        head.cmd = CMD::CMDLINE;
        head.dataLen = strlen(szBuffRead);
        m_client->Send((char*)&head, sizeof(head));
        m_client->Send(szBuffRead, head.dataLen);
    }
}


void CRemoteClientDlg::DoCmdLine(int dataLen)
{
    char* buf = new char[dataLen + 1]();
    m_client->Recv(buf, dataLen);
    DWORD writeF = 0;
    if (buf != NULL || dataLen > 0) {
        //写入管道api
        ::WriteFile(this->write, buf, dataLen, &writeF, NULL);
    }
}



void CRemoteClientDlg::PostNcDestroy()
{
    
    if (m_client != NULL) {
        delete m_client;
        //程序退出终止cmd进程
        ::TerminateProcess(this->hCmd, 0);
    }
    CDialogEx::PostNcDestroy();
}


void CRemoteClientDlg::CmdCtrl(int len)
{
    if (len <= 0) {
        return;
    }

    char* data = new char[len + 1]();
    m_client->Recv(data, len);
    CString ctrl(data);
    if (ctrl == "Ctrl+C") {
        //连接到进程控制台
        AttachConsole(this->proID);
        //禁用Control+C处理我们的应用程序
        SetConsoleCtrlHandler(NULL, TRUE);
        //生成Control+ C事件  

        GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
        SetConsoleCtrlHandler(NULL, FALSE);
         
    }
}


void CRemoteClientDlg::DoDiskInfo()
{
    char buf[MAX_PATH] = { 0 };
    GetLogicalDriveStringsA(MAX_PATH, buf);
    for (char* s = buf; *s; s += strlen(s) + 1) {
        GetDiskInfo(s);
    }
}


void CRemoteClientDlg::GetDiskInfo(char* s)
{
    Disk_Info info = { 0 };
    strncpy(info.disk, s, strlen(s));

    //获取磁盘类型
    UINT uDriverType = GetDriveTypeA(s);
    const char* buf = NULL;
    switch (uDriverType) {
    case DRIVE_UNKNOWN:buf = "未知的磁盘类型"; break;
    case DRIVE_NO_ROOT_DIR: buf = "路径无效"; break;
    case DRIVE_REMOVABLE: buf = "可移动磁盘"; break;
    case DRIVE_FIXED: buf = "固定磁盘"; break;
    case DRIVE_REMOTE: buf = "网络磁盘"; break;
    case DRIVE_CDROM: buf = "光驱"; break;
    case DRIVE_RAMDISK: buf = "内存映射盘"; break;
    default:
        break;
    }
    if (buf != NULL) {
        strncpy(info.type, buf, strlen(buf));
    }
    
    ULARGE_INTEGER available, total, free; //ULARGE_INTEGER  一个64位的无符号整型值
    //获取盘符容量信息
    if(GetDiskFreeSpaceExA(s, (ULARGE_INTEGER*)&available, (ULARGE_INTEGER*)&total, (ULARGE_INTEGER*)&free)) {
        info.total = GB(total);
        info.available = GB(available);
        info.free = GB(free);
    }
    
    Server_Head head;
    head.cmd = CMD::DISKINFO;
    head.dataLen = sizeof(info);
    m_client->Send((char*)&head, sizeof(head));
    m_client->Send((char*)&info, head.dataLen);
}


void CRemoteClientDlg::DoFileInfo(int len)
{
    if (len == 0) {
        return;
    }
    char* data = new char[len + 1]();
    m_client->Recv(data, len);

  
    intptr_t handle;
    _finddata_t findData;

    string path(data);
    path += "*";

    handle = _findfirst(path.c_str(), &findData);    // 查找目录中的第一个文件
    if (handle == -1)
    {
        return;
    }

    char* buf = NULL;

    do
    {
        
        string str;

        if (findData.attrib & _A_SUBDIR) {
            if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0) {
                continue;
            }
            // 是否是子目录并且不为"."或".."
            //文件类型
            str = "文件夹";
        }   
        else {
            SHFILEINFOA F_info;
            if (SHGetFileInfoA(findData.name,
                FILE_ATTRIBUTE_NORMAL,
                &F_info,
                sizeof(F_info),
                SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES))
            {
                str = F_info.szTypeName;
            }
        }

        //文件
        buf = new char[sizeof(File_Info) + strlen(findData.name)]();
        File_Info* fileInfo = (File_Info*)buf;

        //将时间值转换为结构体
        struct tm* date = gmtime(&findData.time_write);
        //格式时间
        strftime(fileInfo->date, sizeof(fileInfo->date), "%Y/%m/%d %H:%M:%S", date);
        //文件大小
        fileInfo->size = findData.size;
        //文件类型
        strncpy(fileInfo->type, str.c_str(), strlen(str.c_str()));
        //文件名
        strncpy(fileInfo->name, findData.name, strlen(findData.name));


        Server_Head head;
        head.cmd = CMD::FILEDIR;
        head.dataLen = sizeof(File_Info) + strlen(findData.name);

        m_client->Send((char*)&head, sizeof(head));
        m_client->Send((char*)fileInfo, head.dataLen);

        delete[] fileInfo;

    } while (_findnext(handle, &findData) == 0);    // 查找目录中的下一个文件

    _findclose(handle);    // 关闭搜索句柄

    delete[] data;
}


void CRemoteClientDlg::DoDownFile(int len)
{
    char* data = new char[len + 1]();
    m_client->Recv(data, len);

    HANDLE File = ::CreateFileA(data, GENERIC_READ, //打开的文件为只读权限
        FILE_SHARE_DELETE, //设置文件的共享权限可以删
        NULL,
        OPEN_EXISTING,//打开一个以存在的文件
        FILE_ATTRIBUTE_NORMAL,//该文件未设置其他属性
        NULL);
    //如果源文件打开失败则退出
    if (File == INVALID_HANDLE_VALUE) {
        return;
    }
    delete[] data;
    //获取文件大小
    DWORD fileSizeH;
    DWORD fileSizeL = GetFileSize(File, &fileSizeH);
    LONGLONG fileSize = MAKE_i64(fileSizeH, fileSizeL);//获取文件大小



    char szBuff[SIZE] = { 0 }; //定义一个缓冲区存放读取的数据

    DWORD readSize; //用于存放实际读取到的数据大小
    DWORD curSize = 0;


    Server_Head head;
    head.cmd = CMD::DOWNFILE;
    head.dataLen = 0;
   

    //Down_File df = { 0 };
    char* d = new char[sizeof(head) +  1000 + sizeof(int)]();
    int cursize = 0;
    while (ReadFile(File, szBuff, 1000, &readSize, NULL)) {  
        
        curSize += readSize;
       

        //拷贝数据头
        head.dataLen = readSize + sizeof(int);
        memcpy(d, &head, sizeof(head));
        //拷贝数据
        cursize = (curSize - 0.0) / (fileSize - 0.0) * 100;
        memcpy(d + sizeof(head), &cursize, sizeof(int)); //拷贝以拷贝的进度
        memcpy(d + sizeof(head)+sizeof(int), szBuff, readSize);//拷贝数据
        

        m_client->Send(d, sizeof(head) + readSize+sizeof(int));
        if (readSize < 1000) break;
    }
    delete[] d;
    CloseHandle(File);
    
}


void CRemoteClientDlg::DoUpFile(int len)
{
    char* path = new char[len]();
    m_client->Recv(path, len);

    //打开目标文件
    HANDLE file = CreateFileA(path,
        GENERIC_WRITE,
        FILE_SHARE_DELETE,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    bool isOk = false;
    //如果目标文件打开失败则退出
    if (file == INVALID_HANDLE_VALUE) {
       
        return;
    }
   
    //
    DWORD writeSize = 0;

    Down_File df = { 0 };
    while (true)
    {
        m_client->Recv((char*)&df, sizeof(df));
        WriteFile(file, df.data, df.dataSize, &writeSize, NULL);
        if (df.dataSize < 1000) break;
    }
    CloseHandle(file);

    delete[] path;
}

DWORD __stdcall CRemoteClientDlg::ThreadUpProc(LPVOID lpParameter)
{
    return 0;
}

DWORD __stdcall CRemoteClientDlg::ThreadDownProc(LPVOID lpParameter)
{
    return 0;
}
