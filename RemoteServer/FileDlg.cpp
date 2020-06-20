// FileDlg.cpp: 实现文件
//

#include "pch.h"
#include "RemoteServer.h"
#include "FileDlg.h"
#include "afxdialogex.h"

#define MAKE_i64(hi, lo) ((LONGLONG(DWORD(hi) & 0xffffffff) << 32 ) | LONGLONG(DWORD(lo) & 0xffffffff))


// FileDlg 对话框

IMPLEMENT_DYNAMIC(FileDlg, CDialogEx)

FileDlg::FileDlg(ItemData* item, CWnd* pParent /*=nullptr*/)
    : RemoteAll(FILE_DLG, pParent)
    , m_item(item)
{

}

FileDlg::~FileDlg()
{
}

void FileDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST2, m_listCtr);
    DDX_Control(pDX, IDC_EDIT1, m_editPath);
    DDX_Control(pDX, FILE_LABLE, m_fileLable);
    DDX_Control(pDX, IDC_PROGRESS1, m_loadBar);
}


BEGIN_MESSAGE_MAP(FileDlg, CDialogEx)
    ON_WM_CLOSE()
    ON_NOTIFY(NM_DBLCLK, IDC_LIST2, &FileDlg::OnNMDblclkList2)
    ON_BN_CLICKED(IDC_BUTTON1, &FileDlg::OnBnClickedGoDir)
    ON_BN_CLICKED(IDC_BUTTON2, &FileDlg::OnBnClickedUpDir)
    ON_NOTIFY(NM_RCLICK, IDC_LIST2, &FileDlg::OnNMRClickList2)
    ON_COMMAND(MENU_ID_DOWN, &FileDlg::OnIdDownFile)
    ON_COMMAND(MENU_ID_UP, &FileDlg::OnIdUpFile)
END_MESSAGE_MAP()


// FileDlg 消息处理程序


void FileDlg::OnCancel()
{
}


void FileDlg::OnOK()
{
}


void FileDlg::OnClose()
{
    EndDialog(0);
}

void FileDlg::DoData(Server_Head& head)
{
    switch (head.cmd)
    {
    case CMD::FILEDIR:
        DoFileData(head.dataLen);
        break;
    case CMD::DISKINFO:
        DoDiskInfo();
        break;
    case CMD::DOWNFILE:
        DoSaveFile(head.dataLen);
        break;
    default:
        break;
    }
}


BOOL FileDlg::OnInitDialog()
{
    RemoteAll::OnInitDialog();

    m_fileLable.ShowWindow(false);
    m_loadBar.ShowWindow(false);
    m_loadBar.SetRange(0, 100);

    m_item->m_dlg = this;

    CString str("文件管理 \\\\");
    str += m_item->m_socket->addr.ToString().c_str();
    SetWindowText(str);

    m_listCtr.SetExtendedStyle(m_listCtr.GetExtendedStyle()|
        LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES); //设置list控件风格

    m_listCtr.InsertColumn(0, _T("名称"));
    m_listCtr.SetColumnWidth(0, 150);
    m_listCtr.InsertColumn(1, _T("修改日期"));
    m_listCtr.SetColumnWidth(1, 150);
    m_listCtr.InsertColumn(2, _T("类型"));
    m_listCtr.SetColumnWidth(2, 100);
    m_listCtr.InsertColumn(3, _T("大小"));
    m_listCtr.SetColumnWidth(3, 100);
    m_listCtr.InsertColumn(4, _T(""));
    m_listCtr.SetColumnWidth(4, 100);

    Server_Head head;
    head.cmd = CMD::DISKINFO;
    head.dataLen = 0;
    m_item->m_socket->Send((char*)&head, sizeof(head));



    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void FileDlg::DoFileData(int len)
{
    char* fileInfo = new char[len + 1]();
    //File_Info info = { 0 };
    m_item->m_socket->Recv(fileInfo, len);
    File_Info* info = (File_Info*)fileInfo;

    LVCOLUMN col = { 0 };
    col.mask = LVCF_TEXT;
    col.pszText = _T("名称");
    m_listCtr.SetColumn(0, &col);
    col.pszText = _T("修改日期");
    m_listCtr.SetColumn(1, &col);
    col.pszText = _T("类型");
    m_listCtr.SetColumn(2, &col);
    col.pszText = _T("大小");
    m_listCtr.SetColumn(3, &col);
    col.pszText = _T("");
    m_listCtr.SetColumn(4, &col);

    int row = m_listCtr.GetItemCount();

    
    CString str;
    if (CString(info->type) == "文件夹") {
        str = "";
        m_listCtr.InsertItem(row, CString(info->name)+="\\");
    }
    else {
        str.Format(_T("%ld KB"), info->size);
        m_listCtr.InsertItem(row, CString(info->name));
    }
    
    m_listCtr.SetItemText(row, 1, CString(info->date));
    m_listCtr.SetItemText(row, 2, CString(info->type));
    m_listCtr.SetItemText(row, 3, str);

    delete[] fileInfo;
}


void FileDlg::DoDiskInfo()
{
    Disk_Info info = { 0 };
    m_item->m_socket->Recv((char*)&info, sizeof(info));
    
    LVCOLUMN col = { 0 };
    col.mask = LVCF_TEXT;
    col.pszText = _T("名称");
    m_listCtr.SetColumn(0, &col);
    col.pszText = _T("类型");
    m_listCtr.SetColumn(1, &col);
    col.pszText = _T("总容量");
    m_listCtr.SetColumn(2, &col);
    col.pszText = _T("已用");
    m_listCtr.SetColumn(3, &col);
    col.pszText = _T("空闲");
    m_listCtr.SetColumn(4, &col);

    int row = m_listCtr.GetItemCount();

    m_listCtr.InsertItem(row, CString(info.disk));
    m_listCtr.SetItemText(row, 1, CString(info.type));
    CString str;
    str.Format(_T("%.2f GB"), info.total);
    m_listCtr.SetItemText(row, 2, str);
    str.Format(_T("% .2f GB"), info.available);
    m_listCtr.SetItemText(row, 3, str);
    str.Format(_T("% .2f GB"), info.free);
    m_listCtr.SetItemText(row, 4, str);

}


void FileDlg::OnNMDblclkList2(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    
    //获取当前选中行
    POSITION pos = m_listCtr.GetFirstSelectedItemPosition();
    int row = m_listCtr.GetNextSelectedItem(pos);

    CString str = m_listCtr.GetItemText(row, 0);

    if (str.Right(1) != "\\") {
        return;
    }
    
    m_curPath += str;

    //声明标识
    USES_CONVERSION;
    //函数T2A和W2A均支持ATL和MFC中的字符
    char* pPath = T2A(m_curPath);

    Server_Head head;
    head.cmd = CMD::FILEDIR;
    head.dataLen = strlen(pPath);

    m_item->m_socket->Send((char*)&head, sizeof(head));
    m_item->m_socket->Send(pPath, head.dataLen);

    m_listCtr.DeleteAllItems(); //清空所有内容
    //设置当前路径
    m_editPath.SetWindowText(m_curPath);
    
    *pResult = 0;
}


void FileDlg::OnBnClickedGoDir()
{
    m_listCtr.DeleteAllItems(); //清空所有内容
    CString str;
    m_editPath.GetWindowText(str);
    if (str.IsEmpty() || str == "") {
        return;
    }
    if (str.Right(1) != "\\") {
        str += "\\";
    }
    m_curPath = str; //更改当前路径
    //声明标识
    USES_CONVERSION;
    //函数T2A和W2A均支持ATL和MFC中的字符
    char* pPath = T2A(str);

    Server_Head head;
    head.cmd = CMD::FILEDIR;
    head.dataLen = strlen(pPath);

    m_item->m_socket->Send((char*)&head, sizeof(head));
    m_item->m_socket->Send(pPath, head.dataLen);

}


void FileDlg::OnBnClickedUpDir()
{
    m_listCtr.DeleteAllItems(); //清空所有内容

    int index = m_curPath.ReverseFind('\\');
    index = m_curPath.Left(index).ReverseFind('\\')+1;

    m_curPath = m_curPath.Left(index);

    m_editPath.SetWindowText(m_curPath);

    Server_Head head;
    if (index <= 0) {
        m_curPath = "";
        head.cmd = CMD::DISKINFO;
        head.dataLen = 0;
        m_item->m_socket->Send((char*)&head, sizeof(head));

        return;
    }

    //声明标识
    USES_CONVERSION;
    //函数T2A和W2A均支持ATL和MFC中的字符
    char* pPath = T2A(m_curPath);

    head.cmd = CMD::FILEDIR;
    head.dataLen = strlen(pPath);

    m_item->m_socket->Send((char*)&head, sizeof(head));
    m_item->m_socket->Send(pPath, head.dataLen);

    
}


void FileDlg::OnNMRClickList2(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    
    POSITION pos = m_listCtr.GetFirstSelectedItemPosition();
    int row = m_listCtr.GetNextSelectedItem(pos);

    //获取当前鼠标位置
    CPoint point;
    GetCursorPos(&point);
    //加载菜单
    CMenu menu;
    menu.LoadMenu(RBTN_FILE_MENU);
    //获取菜单的第一个子菜单
    CMenu* pMenu = menu.GetSubMenu(0);

    if (row < 0) {
        pMenu->EnableMenuItem(MENU_ID_DOWN, MF_DISABLED);
    }
    else {
        CString str = m_listCtr.GetItemText(row, 0);
        if (str.Right(1) == "\\") {
            pMenu->EnableMenuItem(MENU_ID_DOWN, MF_DISABLED);
        }
    }
    //显示弹式菜单
    pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this, NULL);

    *pResult = 0;
}

void FileDlg::OnIdDownFile()
{
    if (!DownFileDlg()) {
        return;
    }

    POSITION pos = m_listCtr.GetFirstSelectedItemPosition();
    int row = m_listCtr.GetNextSelectedItem(pos);

    CString str = m_listCtr.GetItemText(row, 0);
    CString fileName = str;
    str = m_curPath + str;

    //声明标识
    USES_CONVERSION;
    //函数T2A和W2A均支持ATL和MFC中的字符
    char* pPath = T2A(str);

    //获取当前文件名
    m_downFileDir += "\\";
    m_downFileDir += fileName;
    char* pSave = T2A(m_downFileDir);

    if (!CreateMyFile(pSave)) {
        return;
    }

    m_fileLable.SetWindowText(_T("下载中"));
    m_fileLable.ShowWindow(true);
    m_loadBar.ShowWindow(true);

    Server_Head head;
    head.cmd = CMD::DOWNFILE;
    head.dataLen = strlen(pPath);

    m_item->m_socket->Send((char*)&head, sizeof(head));
    m_item->m_socket->Send(pPath, head.dataLen);
}


bool FileDlg::DownFileDlg()
{
    CFileFind finder;
    CString path;
    BOOL fileExist;

    // 配置对话框  
    BROWSEINFO bi;
    ZeroMemory(&bi, sizeof(bi));

    // 获取特定文件夹的LPITEMIDLIST，可以将之理解为HANDLE  
    // 所谓的特定文件夹,你可以用CSIDL_XXX来检索之。  
    LPITEMIDLIST rootLoation;
    SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &rootLoation);
    if (rootLoation != NULL) {
        bi.pidlRoot = rootLoation; // 文件夹对话框之根目录，不指定的话则为我的电脑
    }  
     
    bi.lpszTitle = _T("下载文件"); // 可以不指定  
    // bi.ulFlags = BIF_EDITBOX | BIF_RETURNONLYFSDIRS;  

    // 打开对话框, 有点像DoModal  
    LPITEMIDLIST targetLocation = SHBrowseForFolder(&bi);
    if (targetLocation != NULL) {
        TCHAR targetPath[MAX_PATH];
        SHGetPathFromIDList(targetLocation, targetPath);
        m_downFileDir = targetPath;
        return true;
    }
    return false;
}


void FileDlg::DoSaveFile(int len)
{
    char* data = new char[len];
    
    m_item->m_socket->Recv(data, len); //获取当前大小
    
    int cursize = 0;
    memcpy(&cursize, data, sizeof(int));

    DWORD writeSize = 0;
    WriteFile(m_hSaveFile, data+sizeof(int), len-sizeof(int), &writeSize, NULL);
    m_loadBar.SetPos(cursize);
    
    if (cursize == 100) {
        CloseHandle(m_hSaveFile);
        m_loadBar.SetPos(0);
        m_fileLable.ShowWindow(false);
        m_loadBar.ShowWindow(false);
    }
    delete[] data;
}


bool FileDlg::CreateMyFile(char* pPath)
{
    //打开目标文件
    m_hSaveFile = CreateFileA(pPath,
        GENERIC_WRITE,
        FILE_SHARE_DELETE,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    //如果目标文件打开失败则退出
    if (m_hSaveFile == INVALID_HANDLE_VALUE) {
        return false;
    }
    return true;
}


void FileDlg::OnIdUpFile()
{
    m_fileLable.SetWindowText(_T("上传中"));
    m_fileLable.ShowWindow(true);
    m_loadBar.ShowWindow(true);
    
    BOOL isOpen = TRUE;     //是否打开(否则为保存)  
    
    CFileDialog openFileDlg(isOpen, NULL, NULL, OFN_HIDEREADONLY | OFN_READONLY, NULL, NULL);
    INT_PTR result = openFileDlg.DoModal();
    CString filePath;
    if (result == IDOK) {
        filePath = openFileDlg.GetPathName();
        //声明标识
        USES_CONVERSION;
        //函数T2A和W2A均支持ATL和MFC中的字符
        char* pPath = T2A(filePath);

        HANDLE File = ::CreateFileA(pPath, GENERIC_READ, //打开的文件为只读权限
            FILE_SHARE_DELETE, //设置文件的共享权限可以删
            NULL,
            OPEN_EXISTING,//打开一个以存在的文件
            FILE_ATTRIBUTE_NORMAL,//该文件未设置其他属性
            NULL);
        //如果源文件打开失败则退出
        if (File == INVALID_HANDLE_VALUE) {
            return;
        }
        //获取文件大小
        DWORD fileSizeH;
        DWORD fileSizeL = GetFileSize(File, &fileSizeH);
        LONGLONG fileSize = MAKE_i64(fileSizeH, fileSizeL);//获取文件大小

        
        char* szBuff[1000] = { 0 };
        DWORD readSize = 0;
        LONGLONG curSize = 0;
        int index = filePath.ReverseFind('\\');
        CString fileName = filePath.Right(filePath.GetLength() - index-1);
        CString desAdd = m_curPath += fileName;

        char* pDesAdd = T2A(desAdd);

        Server_Head head;
        head.cmd = CMD::UPFILE;
        head.dataLen = strlen(pDesAdd);
        m_item->m_socket->Send((char*)&head, sizeof(Server_Head));
        m_item->m_socket->Send(pDesAdd, head.dataLen);


        head.dataLen = 0;

        char* d = new char[sizeof(head) + 1000 + sizeof(int)]();
        int cursize = 0;
        while (ReadFile(File, szBuff, 1000, &readSize, NULL)) {

            curSize += readSize;


            //拷贝数据头
            head.dataLen = readSize + sizeof(int);
            memcpy(d, &head, sizeof(head));
            //拷贝数据
            m_loadBar.SetPos((curSize - 0.0) / (fileSize - 0.0) * 100);
            memcpy(d + sizeof(head), &cursize, sizeof(int)); //拷贝以拷贝的进度
            memcpy(d + sizeof(head) + sizeof(int), szBuff, readSize);//拷贝数据


            m_item->m_socket->Send(d, sizeof(head) + readSize + sizeof(int));
            if (readSize < 1000) break;
        }
        delete[] d;
        CloseHandle(File);
    }
    m_loadBar.SetPos(0);
    m_fileLable.ShowWindow(false);
    m_loadBar.ShowWindow(false);
}
