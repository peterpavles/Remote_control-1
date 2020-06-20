#pragma once
#include "MyEdit.h"

// CmdDlg 对话框

class CmdDlg : public RemoteAll
{
	DECLARE_DYNAMIC(CmdDlg)

public:
	CmdDlg(ItemData* item, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CmdDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = CMD_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    ItemData* m_item;
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnClose();
    virtual void OnCancel();
    virtual void OnOK();
    virtual void PostNcDestroy();
    void DoData(Server_Head& head);
    void CmdDoWork(char* buf, int len);
    MyEdit m_edit;
    CString m_cmdData;
};
