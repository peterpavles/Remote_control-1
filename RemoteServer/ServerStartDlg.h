#pragma once

// ServerStartDlg 对话框

class ServerStartDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ServerStartDlg)

public:
	ServerStartDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~ServerStartDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = SER_START_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
    virtual void OnCancel();
    virtual void OnOK();
public:
    afx_msg void OnClose();
    afx_msg void OnBnClickedStartServer();
public:
    int m_port;
    DWORD m_ip;
};
