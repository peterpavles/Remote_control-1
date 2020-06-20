#pragma once


// MyEdit

class MyEdit : public CEdit
{
	DECLARE_DYNAMIC(MyEdit)

public:
	MyEdit();
	virtual ~MyEdit();

protected:
	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    CString m_data;
    CString m_oldData;
    ItemData* m_item;
    afx_msg void OnEnChange();
    int m_curPos;
};


