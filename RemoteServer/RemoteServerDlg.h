
// RemoteServerDlg.h: 头文件
//

#pragma once
#include "ServerStartDlg.h"
#include "MyLog.h"
#include <list>

// CRemoteServerDlg 对话框
class CRemoteServerDlg : public CDialogEx
{
// 构造
public:
	CRemoteServerDlg(CWnd* pParent = nullptr);	// 标准构造函数
    ~CRemoteServerDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REMOTESERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
    virtual void OnCancel();
    virtual void OnOK();
public:
    afx_msg void OnClose();

    static DWORD WINAPI ListSocket(LPVOID lpParameter);

    static DWORD WINAPI ClientSocket(LPVOID lpParameter);

private:
    Socket* m_listenSocket;
    MyLog m_myLog;
    CTreeCtrl m_tree;
    bool listenThreadIsOut;
public:
//    afx_msg void OnNMDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult);
    virtual void PostNcDestroy();
protected:
    afx_msg LRESULT OnDelitem(WPARAM wParam, LPARAM lParam);
public:
    void DoData(Server_Head& head);
    afx_msg void OnNMRClickTree1(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnRemoteDesktop();
    afx_msg void OnRecmoteCmd();
    afx_msg void OnRemoteFile();
};
