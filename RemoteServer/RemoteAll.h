#pragma once


// RemoteAll 对话框

class RemoteAll : public CDialogEx
{
	DECLARE_DYNAMIC(RemoteAll)

public:
	RemoteAll(UINT nIDTemplate,CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~RemoteAll();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
public:
    virtual void OnClose() = 0;
    virtual void DoData(Server_Head& head) = 0;

	DECLARE_MESSAGE_MAP()
};
