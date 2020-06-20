// MyEdit.cpp: 实现文件
//

#include "pch.h"
#include "RemoteServer.h"
#include "MyEdit.h"
#include "CmdDlg.h"


// MyEdit

IMPLEMENT_DYNAMIC(MyEdit, CEdit)

MyEdit::MyEdit()
    : m_item(NULL)
    , m_curPos(0)
{

}

MyEdit::~MyEdit()
{
}


BEGIN_MESSAGE_MAP(MyEdit, CEdit)
    ON_CONTROL_REFLECT(EN_CHANGE, &MyEdit::OnEnChange)
END_MESSAGE_MAP()



// MyEdit 消息处理程序




BOOL MyEdit::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN) {
        SetSel(m_curPos, m_curPos);
        switch (pMsg->wParam)
        {
        case VK_RETURN:
        {
            GetWindowText(m_data);
            CString str = m_data.Right(m_data.GetLength() - m_oldData.GetLength());
            if (str == "cls") {
                m_data = "";
                m_oldData = "";
                CmdDlg* dlg = (CmdDlg*)m_item->m_dlg;
                dlg->m_cmdData = "";
            }

            USES_CONVERSION;
            //函数T2A和W2A均支持ATL和MFC中的字符
            char* pFileName = T2A(str += "\r\n");

            Server_Head head;
            head.cmd = CMD::CMDLINE;
            head.dataLen = strlen(pFileName);

            m_item->m_socket->Send((char*)&head, sizeof(head));
            m_item->m_socket->Send(pFileName, head.dataLen);
            m_oldData = m_data;
            return true;
        }
        case VK_LEFT:
        {
            if (m_curPos > m_oldData.GetLength()) {
                m_curPos--;
                return false;
            }
            return true;
        }
        case VK_RIGHT:
            if (m_curPos < m_data.GetLength()) {
                m_curPos++;
                return false;
            }
            return true;
        case VK_BACK:
            if (m_curPos > m_oldData.GetLength()) {
                m_curPos--;
                return false;
                m_data.Delete(m_curPos, 1);
            }
            return true;
        case VK_DELETE:
            if (m_curPos == m_data.GetLength()) {
               return true;
            }
            m_data.Delete(m_curPos, 1);
            return false;
        case VK_UP:
        case VK_DOWN:
            return true;
        case 'c':
        case 'C':
            if (::GetKeyState(VK_CONTROL) < 0) {
                const char* ctrl = "Ctrl+C";
                Server_Head head;
                head.cmd = CMD::CMDCTRL;
                head.dataLen = strlen(ctrl);
                m_item->m_socket->Send((char*)&head, sizeof(head));
                m_item->m_socket->Send(ctrl, head.dataLen);
            }
            break;
        default:
            break;
        }
    }

    return CEdit::PreTranslateMessage(pMsg);
}


void MyEdit::OnEnChange()
{
    GetWindowText(m_data);
    int nStart;
    int nEnd;
    GetSel(nStart, nEnd);
    if (nStart == nEnd) {
        m_curPos = nStart;
    }
}
