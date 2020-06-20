#pragma once



// FileDlg 对话框

class FileDlg : public RemoteAll
{
	DECLARE_DYNAMIC(FileDlg)

public:
	FileDlg(ItemData* item,CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~FileDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = FILE_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
    virtual void OnCancel();
    virtual void OnOK();
public:
    ItemData* m_item;

    afx_msg void OnClose();
    void DoData(Server_Head& head);

    virtual BOOL OnInitDialog();
    CListCtrl m_listCtr;
    void DoFileData(int len);
    void DoDiskInfo();
    afx_msg void OnNMDblclkList2(NMHDR* pNMHDR, LRESULT* pResult);
    CString m_curPath;
    CEdit m_editPath;
    afx_msg void OnBnClickedGoDir();
    afx_msg void OnBnClickedUpDir();
    afx_msg void OnNMRClickList2(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnIdDownFile();
    bool DownFileDlg();
    CString m_downFileDir;
    HANDLE m_hSaveFile;
    void DoSaveFile(int len);
    bool CreateMyFile(char* pPath);
    CStatic m_fileLable;
    CProgressCtrl m_loadBar;
    afx_msg void OnIdUpFile();

};
