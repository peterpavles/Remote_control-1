// CmdDlg.cpp: 实现文件
//

#include "pch.h"
#include "RemoteServer.h"
#include "CmdDlg.h"
#include "afxdialogex.h"


// CmdDlg 对话框

IMPLEMENT_DYNAMIC(CmdDlg, CDialogEx)

CmdDlg::CmdDlg(ItemData* item,CWnd* pParent /*=nullptr*/)
	: RemoteAll(CMD_DLG, pParent)
    ,m_item(item)
{

}

CmdDlg::~CmdDlg()
{
}

void CmdDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT1, m_edit);
}


BEGIN_MESSAGE_MAP(CmdDlg, CDialogEx)
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// CmdDlg 消息处理程序


BOOL CmdDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_item->m_dlg = this;

    CString str("远程CMD \\\\");
    str += m_item->m_socket->addr.ToString().c_str();
    SetWindowText(str);

    m_edit.m_item = m_item;

    Server_Head head;
    head.cmd = CMD::STARTCMD;
    head.dataLen = 0;
    m_item->m_socket->Send((char*)&head,sizeof(head));

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CmdDlg::OnClose()
{
    Server_Head head;
    head.cmd = CMD::STOPCMD;
    head.dataLen = 0;
    m_item->m_socket->Send((char*)&head, sizeof(head));
    RemoteAll::OnCancel();
}


void CmdDlg::OnCancel()
{

}


void CmdDlg::OnOK()
{
  
}


void CmdDlg::PostNcDestroy()
{
    m_item->m_dlg = NULL;
    //窗口销毁释放自己
    delete this;
    RemoteAll::PostNcDestroy();
}

void CmdDlg::DoData(Server_Head& head)
{
    char* data = NULL;
    if (head.dataLen > 0) {
        data = new char[head.dataLen + 1]();
    }
    switch (head.cmd)
    {
    case CMD::CMDLINE:
        CmdDoWork(data,head.dataLen);
        break;
    default:
        break;
    }
    if (data != NULL) {
        delete[] data;
        data = NULL;
    }
}


void CmdDlg::CmdDoWork(char* buf, int len)
{
    if (len <= 0 || buf == NULL) {
        return;
    }
    m_item->m_socket->Recv(buf, len);
    m_cmdData += buf;
    m_edit.SetWindowText(m_cmdData);
    int nLength = m_cmdData.GetLength();
    m_edit.SetSel(nLength, nLength, FALSE);
    m_edit.SetFocus();
    m_edit.m_oldData = m_cmdData;
    m_edit.m_curPos = m_cmdData.GetLength();
}


