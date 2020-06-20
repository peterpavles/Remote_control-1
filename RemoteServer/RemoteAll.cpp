// RemoteAll.cpp: 实现文件
//

#include "pch.h"
#include "RemoteServer.h"
#include "RemoteAll.h"
#include "afxdialogex.h"


// RemoteAll 对话框

IMPLEMENT_DYNAMIC(RemoteAll, CDialogEx)

RemoteAll::RemoteAll(UINT nIDTemplate,CWnd* pParent /*=nullptr*/)
	: CDialogEx(nIDTemplate, pParent)
{

}

RemoteAll::~RemoteAll()
{
}

void RemoteAll::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(RemoteAll, CDialogEx)
END_MESSAGE_MAP()


// RemoteAll 消息处理程序
