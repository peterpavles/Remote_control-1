// ServerStartDlg.cpp: 实现文件
//

#include "pch.h"
#include "RemoteServer.h"
#include "ServerStartDlg.h"
#include "afxdialogex.h"


// ServerStartDlg 对话框

IMPLEMENT_DYNAMIC(ServerStartDlg, CDialogEx)

ServerStartDlg::ServerStartDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(SER_START_DLG, pParent)
    , m_port(6697)
    , m_ip(0)
{

}

ServerStartDlg::~ServerStartDlg()
{
}

void ServerStartDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT1, m_port);
    DDX_IPAddress(pDX, IDC_IPADDRESS1, m_ip);
}


BEGIN_MESSAGE_MAP(ServerStartDlg, CDialogEx)
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_BUTTON1, &ServerStartDlg::OnBnClickedStartServer)
END_MESSAGE_MAP()


// ServerStartDlg 消息处理程序


void ServerStartDlg::OnCancel()
{
}


void ServerStartDlg::OnOK()
{
}


void ServerStartDlg::OnClose()
{
    EndDialog(IDCLOSE);
}


void ServerStartDlg::OnBnClickedStartServer()
{
    if (!UpdateData(TRUE)) {
        return;
    }

    if (this->m_port < 1024 || this->m_port > 65535) {
        
        MessageBox(_T("请输入一个 1024 到 65535 之间的整数")
            , _T("错误"), MB_ICONERROR);
        return;
    }

    EndDialog(IDOK);
}
