#pragma once
#include "RemoteAll.h"

// RemoteDlg 对话框

class RemoteDlg : public RemoteAll
{
	DECLARE_DYNAMIC(RemoteDlg)

public:
	RemoteDlg(ItemData* item, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~RemoteDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = REM_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    ItemData* m_item;
    int m_x;
    int m_y;
public:
    afx_msg void OnClose();
    int GetRecv(char* buff, int buffLen);
    void DealCmd(Server_Head& head);
    virtual BOOL OnInitDialog();
    virtual void PostNcDestroy();
    void DoData(Server_Head& head);
    void StartShow(Screen_Info& wh,char* pBuf, int len);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    CStatic m_pro;
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    CScrollBar m_yBar;
    CScrollBar m_xBar;
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
    void SendMouseEvent(int cmd, CPoint point);
    int m_showX;
    int m_showY;
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    void SendKeyEvent(int keyCmd, UINT c);
};
