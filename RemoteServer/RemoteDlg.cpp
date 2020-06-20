// RemoteDlg.cpp: 实现文件
//

#include "pch.h"
#include "RemoteServer.h"
#include "RemoteDlg.h"
#include "afxdialogex.h"


// RemoteDlg 对话框

IMPLEMENT_DYNAMIC(RemoteDlg, CDialogEx)

RemoteDlg::RemoteDlg(ItemData* item, CWnd* pParent /*=nullptr*/)
	: RemoteAll(REM_DLG, pParent)
    , m_item(item)
    , m_x(0)
    , m_y(0)
{

}

RemoteDlg::~RemoteDlg()
{
    
}

void RemoteDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PRO, m_pro);
    DDX_Control(pDX, IDC_SCROLLBAR3, m_yBar);
    DDX_Control(pDX, IDC_SCROLLBAR4, m_xBar);
}


BEGIN_MESSAGE_MAP(RemoteDlg, CDialogEx)
    ON_WM_CLOSE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_HSCROLL()
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_WM_SIZE()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_RBUTTONDBLCLK()
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// RemoteDlg 消息处理程序


void RemoteDlg::OnClose()
{
    Server_Head head;
    head.cmd = CMD::STOPPRINTSCREEN;
    head.dataLen = 0;
    m_item->m_socket->Send((char*)& head, sizeof(head));
    CDialogEx::OnCancel();
}


int RemoteDlg::GetRecv(char* buff, int buffLen)
{
    int totalSize = buffLen;
    int curSize = 0;
    int tmpSize = 0;
    while (true) {
        tmpSize = m_item->m_socket->Recv(buff+curSize, totalSize-curSize);
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

void RemoteDlg::DealCmd(Server_Head& head)
{
    switch (head.cmd) {
        
    }
}

BOOL RemoteDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_item->m_dlg = this;

    CString str("远程桌面 \\\\");
    str += m_item->m_socket->addr.ToString().c_str();
    SetWindowText(str);

    CRect r;
    GetClientRect(r);
    m_pro.SetWindowPos(NULL , 0, 0, r.Width()-20, r.Height()-20,SWP_NOZORDER);

    m_yBar.SetScrollRange(0, 100);
    m_xBar.SetScrollRange(0, 100);

    Server_Head head;
    head.cmd = CMD::PRINTSCREEN;
    head.dataLen = 0;
    m_item->m_socket->Send((char*)& head, sizeof(head));

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void RemoteDlg::PostNcDestroy()
{
    m_item->m_dlg = NULL;
    //窗口销毁释放自己
    delete this;
    CDialogEx::PostNcDestroy();
}

//处理截图命令
void RemoteDlg::DoData(Server_Head& head)
{
    switch (head.cmd)
    {
    case CMD::PRINTSCREEN:
    {
        Screen_Info wh;
       
        m_item->m_socket->Recv((char*)& wh, sizeof(wh));

        int len = head.dataLen;
        if (len == 0) {
            break;
        }
        char* buf = new char[len];
        if (buf == NULL) {
            break;
        }
       //m_item->m_socket->Recv(buf, len);
        if (GetRecv(buf, len) == 1) {
            StartShow(wh,buf, len);
        }
        delete[] buf;
        break;
    }
    default:
        break;
    }
}

//显示截图到窗口
void RemoteDlg::StartShow(Screen_Info& wh,char* pBuf, int len)
{
    //解压图片
    uLongf outSize = wh.dataSize;
    char* pPic = new char[outSize];
    uncompress((unsigned char*)pPic, &outSize, (unsigned char*)pBuf, len);
    
    HDC dc = ::GetDC(m_pro.m_hWnd);

    HDC hMemory = CreateCompatibleDC(dc);
    HBITMAP hBitmap = CreateCompatibleBitmap(dc,
        wh.width,
        wh.height);
    SelectObject(hMemory, hBitmap);
    SetBitmapBits(hBitmap, outSize, pPic);

    //
    CRect rect;
    m_pro.GetWindowRect(rect);

    //更新基准点
    m_showX = (wh.width - rect.Width()) * (m_x * 0.01);
    m_showY = (wh.height - rect.Height()) * (m_y * 0.01);



    //拷贝位图数据
    BitBlt(dc, 0, 0, rect.Width(), rect.Height(),  hMemory, m_showX,
        m_showY, SRCCOPY);
    /*StretchBlt(dc, 0, 0, 
        rect.Width(),
        rect.Height(),
          hMemory, 
          0, 
          0, 
          wh.width, 
          wh.height, 
          SRCCOPY);*/
    delete[] pPic;
    DeleteDC(dc);
    DeleteDC(hMemory);
    DeleteObject(hBitmap);
}


//鼠标左键按下
void RemoteDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    SendMouseEvent(MOUSE::LBUTTONDOWN, point);
}

//鼠标左键抬起
void RemoteDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    SendMouseEvent(MOUSE::LBUTTONUP, point);
}

//水平滚动条消息
void RemoteDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    
    if (&m_xBar == pScrollBar) {
        switch (nSBCode)
        {
        case SB_THUMBPOSITION:
            m_xBar.SetScrollPos(nPos);
            m_x = nPos;
            break;
        default:
            break;
        }

    }

    CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

//垂直滚动条消息
void RemoteDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{

    if (&m_yBar == pScrollBar) {
        switch (nSBCode)
        {
        case SB_THUMBPOSITION:
            m_yBar.SetScrollPos(nPos);
            m_y = nPos;
            break;
        default:
            break;
        }
        
    }

    CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}

//窗口改变处理函数
void RemoteDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);

    if (nType != SIZE_MINIMIZED && m_pro.m_hWnd != NULL)  //判断窗口是不是最小化了，因为窗口最小化之后 ，窗口的长和宽会变成0，当前一次变化的时就会出现除以0的错误操作
    {
        CRect rect;
        m_pro.GetClientRect(rect);
        rect.right = cx-20;
        rect.bottom = cy-20;
        m_pro.MoveWindow(rect);

        m_yBar.GetClientRect(rect);
        rect.left = cx - 18;
        rect.right = cx;
        rect.bottom = cy-18;
        m_yBar.MoveWindow(rect);

        m_xBar.GetClientRect(rect);
        rect.top = cy - 18;
        rect.bottom = cy;
        rect.right = cx-18;
        m_xBar.MoveWindow(rect);
    }
    
}

//鼠标右键按下
void RemoteDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
    SendMouseEvent(MOUSE::RBUTTONDOWN, point);
}

//鼠标右键抬起
void RemoteDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
    SendMouseEvent(MOUSE::RBUTTONUP, point);
}

//鼠标左键双击
void RemoteDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    SendMouseEvent(MOUSE::LBUTTONDOUBLE, point);
}

//鼠标右键双击
void RemoteDlg::OnRButtonDblClk(UINT nFlags, CPoint point)
{
    SendMouseEvent(MOUSE::RBUTTONDOUBLE, point);
}

//发送鼠标事件
void RemoteDlg::SendMouseEvent(int cmd, CPoint point)
{
    MouseEvent me;
    me.mouseLeg = cmd;
    me.x = point.x + m_showX;
    me.y = point.y + m_showY;

    CString str;
    str.Format(_T("x:%d y:%d"), m_showX, m_showY);
    OutputDebugString(str);

    Server_Head head;
    head.cmd = CMD::MOUSEEVENT;
    head.dataLen = sizeof(me);

    m_item->m_socket->Send((char*)& head, sizeof(head));
    m_item->m_socket->Send((char*)& me, sizeof(me));
}

//鼠标移动消息
void RemoteDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    SendMouseEvent(MOUSE::MOUSEMOVE, point);
}

//处理消息
BOOL RemoteDlg::PreTranslateMessage(MSG* pMsg)
{
    if(pMsg->message == WM_KEYDOWN) // 键盘按下
    {
        SendKeyEvent(KEY::KEYDOWN, pMsg->wParam);
        return true;
    }else if(pMsg->message == WM_KEYUP){    //键盘抬起
        SendKeyEvent(KEY::KEYUP, pMsg->wParam);
        return true;
    }

    return CDialogEx::PreTranslateMessage(pMsg);
}


void RemoteDlg::SendKeyEvent(int keyCmd, UINT c)
{
    Server_Head head;
    head.cmd = CMD::KEYEVENT;
    head.dataLen = sizeof(UINT);

    KeysEvent key;
    key.keyLeg = keyCmd;
    key.data = c;

    m_item->m_socket->Send((char*)& head, sizeof(head));
    m_item->m_socket->Send((char*)& key, sizeof(key));
}
