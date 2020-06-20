
// RemoteServerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "RemoteServer.h"
#include "RemoteServerDlg.h"
#include "afxdialogex.h"
#include "RemoteDlg.h"
#include "CmdDlg.h"
#include "FileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRemoteServerDlg 对话框



CRemoteServerDlg::CRemoteServerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REMOTESERVER_DIALOG, pParent)
    , m_listenSocket(NULL)
    , m_myLog("log.txt")
    , listenThreadIsOut(false)
{
    m_myLog.ClearLog();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CRemoteServerDlg::~CRemoteServerDlg()
{
    if (m_listenSocket != NULL) {
        delete m_listenSocket;
        m_listenSocket = NULL;
    }
}

void CRemoteServerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE1, m_tree);
}

BEGIN_MESSAGE_MAP(CRemoteServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_CLOSE()
//    ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &CRemoteServerDlg::OnNMDblclkTree1)
    ON_MESSAGE(DELITEM, &CRemoteServerDlg::OnDelitem)
    ON_NOTIFY(NM_RCLICK, IDC_TREE1, &CRemoteServerDlg::OnNMRClickTree1)
    ON_COMMAND(ID_SDF_123, &CRemoteServerDlg::OnRemoteDesktop)
    ON_COMMAND(ID_32775, &CRemoteServerDlg::OnRecmoteCmd)
    ON_COMMAND(ID_32776, &CRemoteServerDlg::OnRemoteFile)
END_MESSAGE_MAP()


// CRemoteServerDlg 消息处理程序

BOOL CRemoteServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

    m_tree.InsertItem(_T("所有"),NULL);

    ServerStartDlg dlg;
    if (dlg.DoModal() == IDCLOSE) {
        listenThreadIsOut = true;
        EndDialog(0);
    }
    //创建初始化socket
    m_listenSocket = new Socket;
    if (!m_listenSocket->Create(AF_INET, SOCK_STREAM, IPPROTO_TCP)) {
        m_myLog.PrintLog(m_listenSocket->GetErroInfo());
        MessageBox(_T("套接字创建失败！"), _T("错误"), MB_ICONERROR);
        EndDialog(0);
    }
    //绑定IP地址和端口
    if (!m_listenSocket->Bind(SocketAddr(dlg.m_ip, dlg.m_port))) {
        m_myLog.PrintLog(m_listenSocket->GetErroInfo());
        MessageBox(_T("IP地址和端口绑定失败！"), _T("错误"), MB_ICONERROR);
        EndDialog(0);
    }
    //设置监听
    if (!m_listenSocket->Listen(SOMAXCONN)) {
        m_myLog.PrintLog(m_listenSocket->GetErroInfo());
        CString str(SocketAddr(dlg.m_ip, dlg.m_port).ToString().c_str());
        str += " 监听失败！";
        MessageBox(str, _T("错误"), MB_ICONERROR);
        EndDialog(0);
    }

    

    //启动线程监听socket线程
    ::CreateThread(NULL, 0, CRemoteServerDlg::ListSocket, 
        this, 0, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRemoteServerDlg::OnPaint()
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
HCURSOR CRemoteServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRemoteServerDlg::OnCancel()
{
}


void CRemoteServerDlg::OnOK()
{
}


void CRemoteServerDlg::OnClose()
{
    CDialogEx::OnCancel();
}

DWORD __stdcall CRemoteServerDlg::ListSocket(LPVOID lpParameter)
{
    CRemoteServerDlg* listenDlg = (CRemoteServerDlg*)lpParameter;

    Socket tmp;
    while (true) {
        if (listenDlg->listenThreadIsOut) {
            break;
        }
        if (!listenDlg->m_listenSocket->Accept(tmp)) {
            continue;
        }
        Socket* client = new Socket(tmp);
        //在树控件中新增项
        HTREEITEM treeTmp = listenDlg->m_tree.InsertItem(
            CString(client->addr.ToString().c_str()),
            listenDlg->m_tree.GetRootItem());

        ItemData* item = new ItemData(client, treeTmp, listenDlg);


        listenDlg->m_tree.SetItemData(treeTmp, (DWORD)item);
        ::CreateThread(NULL, 0, CRemoteServerDlg::ClientSocket, item, 0, NULL);
    }
    return 0;
}

DWORD __stdcall CRemoteServerDlg::ClientSocket(LPVOID lpParameter)
{
    ItemData* item = (ItemData*)lpParameter;
    Server_Head head;
    while (true) {

        int ret = item->m_socket->Recv((char*)& head, sizeof(head));
        if (ret == SOCKET_ERROR) {
            if (item->m_socket->GetErroCode() == WSAECONNRESET) {
                if (item->m_dlg != NULL) {
                    item->m_dlg->OnClose();
                }
                item->m_parent->PostMessage(DELITEM, (WPARAM)item->m_treeTmp, 0);
                delete item->m_socket;
                delete item;
                break;
            }
            continue;
        }
        else if (ret == 0) {
            if (item->m_dlg != NULL) {
                item->m_dlg->OnClose();
            }
            item->m_parent->PostMessage(DELITEM, (WPARAM)item->m_treeTmp, 0);
            delete item->m_socket;
            delete item;
            break;
        }
        //*****
        if (item->m_dlg != NULL) {
            item->m_dlg->DoData(head);
        }
    }

    return 0;
}



//void CRemoteServerDlg::OnNMDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult)
//{
//    HTREEITEM  item;
//    item = m_tree.GetSelectedItem();//加入此句获得选中的节点;
//    if (item == m_tree.GetRootItem()) {
//        return;
//    }
//
//    
//    *pResult = 0;
//}


void CRemoteServerDlg::PostNcDestroy()
{
    listenThreadIsOut = true;
    CDialogEx::PostNcDestroy();
}


afx_msg LRESULT CRemoteServerDlg::OnDelitem(WPARAM wParam, LPARAM lParam)
{
    HTREEITEM item = (HTREEITEM)wParam;
    m_tree.DeleteItem(item);
    return 0;
}


void CRemoteServerDlg::DoData(Server_Head& head)
{
    // TODO: 在此处添加实现代码.
}


void CRemoteServerDlg::OnNMRClickTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
    //获取相对的鼠标位置
    CPoint _point(GetMessagePos());
    m_tree.ScreenToClient(&_point);
    //设置选中节点
    HTREEITEM hItem = m_tree.HitTest(_point);
    if (hItem) {
        m_tree.SelectItem(hItem);
    }
    else {
        return;
    }

    HTREEITEM  item;
    item = m_tree.GetSelectedItem();//获得选中的节点;
    if (item == m_tree.GetRootItem()) {
        return;
    }

    //获取当前鼠标位置
    CPoint point;
    GetCursorPos(&point);

    //加载菜单
    CMenu menu;
    menu.LoadMenu(RBTN_MENU);
    //获取菜单的第一个子菜单
    CMenu* pMenu = menu.GetSubMenu(0);
    //显示弹式菜单
    pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this, NULL);

    *pResult = 0;
}


void CRemoteServerDlg::OnRemoteDesktop()
{
    HTREEITEM  item;
    item = m_tree.GetSelectedItem();//获得选中的节点;
    ItemData* itemData = (ItemData*)m_tree.GetItemData(item);//获取节点附加信息
    RemoteDlg* dlg = new RemoteDlg(itemData, this);//创建远程桌面对话框
    dlg->Create(REM_DLG);
    dlg->ShowWindow(SW_SHOW);//显示远程桌面对话框
}


void CRemoteServerDlg::OnRecmoteCmd()
{
    HTREEITEM  item;
    item = m_tree.GetSelectedItem();//获得选中的节点;
    ItemData* itemData = (ItemData*)m_tree.GetItemData(item);//获取节点附加信息
    CmdDlg* dlg = new CmdDlg(itemData,this);//创建远程桌面对话框
    dlg->Create(CMD_DLG);
    dlg->ShowWindow(SW_SHOW);//显示远程桌面对话框
}


void CRemoteServerDlg::OnRemoteFile()
{
    HTREEITEM  item;
    item = m_tree.GetSelectedItem();//获得选中的节点;
    ItemData* itemData = (ItemData*)m_tree.GetItemData(item);//获取节点附加信息
    FileDlg* dlg = new FileDlg(itemData, this);//创建远程桌面对话框
    dlg->Create(FILE_DLG);
    dlg->ShowWindow(SW_SHOW);//显示远程桌面对话框
}
