// pch.cpp: 与预编译标头对应的源文件

#include "pch.h"

// 当使用预编译的头时，需要使用此源文件，编译才能成功。

ItemData::ItemData(Socket* socket, HTREEITEM treeTmp, CWnd* parent)
    :m_socket(socket)
    ,m_treeTmp(treeTmp)
    ,m_dlg(NULL)
    ,m_parent(parent)
{

}

void ItemData::SetDlg(RemoteAll* dlg)
{
    m_dlg = dlg;
}
