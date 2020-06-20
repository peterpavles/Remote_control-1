
// RemoteClientDlg.h: 头文件
//

#pragma once
#define TIMERID 1
#define CMDTIMER 2

// CRemoteClientDlg 对话框
class CRemoteClientDlg : public CDialogEx
{
// 构造
public:
	CRemoteClientDlg(CWnd* pParent = nullptr);	// 标准构造函数
    ~CRemoteClientDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REMOTECLIENT_DIALOG };
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
    afx_msg void OnBnClickedConnect();
    afx_msg void OnBnClickedDisconnect();

    static DWORD WINAPI RecvThread(LPVOID lpParameter);

    int GetRecv(char* buff, int buffLen);
private:
    CIPAddressCtrl m_ip;
    CEdit m_port;
    CButton m_btnConnect;
    CButton m_btnDisconnect;
    Socket* m_client;
    bool ThreadOut;

    HANDLE hCmd;//保存cmd的进程句柄
    DWORD proID;//保存cmd的进程ID
    HANDLE read;//保存父管道的读句柄
    HANDLE CWrite;//保存子管道的写句柄
    HANDLE CRead;//保存子管道的读绝不
    HANDLE write;//保存父管道的写句柄
public:
    void DoData(Server_Head& head);
    void StartPS();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    void DoMouseEvent();
    void DoKeyEvent();
    void StartCmd();
    void ReadCmd();
    void DoCmdLine(int dataLen);
//    virtual void PostNcDestroy();
    virtual void PostNcDestroy();
    void CmdCtrl(int len);
    void DoDiskInfo();
    void GetDiskInfo(char* s);
    void DoFileInfo(int len);
    void DoDownFile(int len);
    void DoUpFile(int len);

    static DWORD WINAPI ThreadUpProc(LPVOID lpParameter);
    static DWORD WINAPI ThreadDownProc(LPVOID lpParameter);
};
