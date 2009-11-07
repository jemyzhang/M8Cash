#include "ui_backup.h"
#include <list>
#include "m8cash.h"
#include "..\MzCommon\MzCommon.h"
using namespace MzCommon;

#define MZ_IDC_TOOLBAR_ACCOUNTS 101
#define MZ_IDC_BACKUP_LIST 102

#define IDC_PPM_CANCEL 108
#define IDC_PPM_DEL		109
#define IDC_PPM_RECOVER 110

MZ_IMPLEMENT_DYNAMIC(Ui_BackupWnd)
#ifdef _DEBUG
#define BACKUP_PARENT_DIR0	L"\\Disk\\User"
#define BACKUP_PARENT_DIR1	L"\\Disk\\User\\Backup"
#define BACKUP_DIR			L"\\Disk\\User\\Backup\\M8Cash"
#else
#define BACKUP_PARENT_DIR0	L"\\Disk\\User"
#define BACKUP_PARENT_DIR1	L"\\Disk\\User\\Backup"
#define BACKUP_DIR			L"\\Disk\\User\\Backup\\M8Cash"
#endif

BOOL Ui_BackupWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
	int y = 0;
	m_Caption1.SetPos(0,y,GetWidth(),MZM_HEIGHT_CAPTION);
	m_Caption1.SetText(LOADSTRING(IDS_STR_BACKUP_RESTORE).C_Str());
	AddUiWin(&m_Caption1);

	y+=MZM_HEIGHT_CAPTION;
    m_List.SetPos(0, y, GetWidth(), GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR - MZM_HEIGHT_CAPTION);
    m_List.SetID(MZ_IDC_BACKUP_LIST);
    m_List.EnableScrollBarV(true);
    m_List.EnableNotifyMessage(true);
    AddUiWin(&m_List);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_RETURN).C_Str());
    m_Toolbar.EnableLeftArrow(true);
	m_Toolbar.SetButton(1, true, false, LOADSTRING(IDS_STR_OPERATE).C_Str());
	m_Toolbar.SetButton(2, true, true, LOADSTRING(IDS_STR_BACKUP).C_Str());
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_ACCOUNTS);
    AddUiWin(&m_Toolbar);

	updateList();
    return TRUE;
}

void Ui_BackupWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
        case MZ_IDC_TOOLBAR_ACCOUNTS:
        {
            int nIndex = lParam;
            if (nIndex == 0) { //返回
                // exit the modal dialog
                EndModal(ID_CANCEL);
                return;
            }
			if (nIndex == 1) { //操作
				// pop out a PopupMenu:
				CPopupMenu ppm;
				struct PopupMenuItemProp pmip;      

				pmip.itemCr = MZC_BUTTON_PELLUCID;
				pmip.itemRetID = IDC_PPM_CANCEL;
				pmip.str = LOADSTRING(IDS_STR_CANCEL).C_Str();
				ppm.AddItem(pmip);  

				pmip.itemCr = MZC_BUTTON_PELLUCID;
				pmip.itemRetID = IDC_PPM_DEL;
				pmip.str = LOADSTRING(IDS_STR_DELETE).C_Str();
				ppm.AddItem(pmip);  

				pmip.itemCr = MZC_BUTTON_PELLUCID;
				pmip.itemRetID = IDC_PPM_RECOVER;
				pmip.str = LOADSTRING(IDS_STR_RESTORE).C_Str();
				ppm.AddItem(pmip);

				RECT rc = MzGetWorkArea();      
				rc.top = rc.bottom - ppm.GetHeight();
				ppm.Create(rc.left,rc.top,RECT_WIDTH(rc),RECT_HEIGHT(rc),m_hWnd,0,WS_POPUP);      
				int nID = ppm.DoModal();
				if(nID == IDC_PPM_RECOVER){	//恢复
					//弹出警告
					if(MzMessageBoxEx(m_hWnd,LOADSTRING(IDS_STR_WARN_RESTORE).C_Str(),L"警告",MZ_YESNO) != 1){
						return;
					}
					//断开数据库
					cash_db.disconnect();
					//恢复数据库
						int nIndex = m_List.GetSelectedIndex();
						if(nIndex == -1) return;
						BOOL nRet = brecover(m_List.GetItem(nIndex)->Text);
						if(nRet){
							//弹出警告
							MzMessageBoxEx(m_hWnd,LOADSTRING(IDS_STR_WARN_RESET).C_Str(),L"成功");
							//退出程序
							PostQuitMessage(0);
						}
				}else if(nID == IDC_PPM_DEL){ //删除
					//删除目录
					int nIndex = m_List.GetSelectedIndex();
					if(nIndex == -1) return;
					BOOL nRet = bdelete(m_List.GetItem(nIndex)->Text);
					if(nRet){
						MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_DELETE_S).C_Str());
						updateList();
					}
				}else{
					return;
				}
			}
            if (nIndex == 2) { //备份
				BOOL nRet = bbackup();
				if (nRet) {
					MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_BACKUP_S).C_Str());
					updateList();
				}
                return;
            }
        }
    }
}

LRESULT Ui_BackupWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case MZ_WM_MOUSE_NOTIFY:
        {
            int nID = LOWORD(wParam);
            int nNotify = HIWORD(wParam);
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            if (nID == MZ_IDC_BACKUP_LIST && nNotify == MZ_MN_LBUTTONDOWN) {
                if (!m_List.IsMouseDownAtScrolling() && !m_List.IsMouseMoved()) {
                    int nIndex = m_List.CalcIndexOfPos(x, y);
                    m_List.SetSelectedIndex(nIndex);
                    m_List.Invalidate();
                    m_List.Update();
			 		m_Toolbar.SetButton(1, true, true, LOADSTRING(IDS_STR_OPERATE).C_Str());
					m_Toolbar.Invalidate();
					m_Toolbar.Update();
                }
                return 0;
            }
            if (nID == MZ_IDC_BACKUP_LIST && nNotify == MZ_MN_MOUSEMOVE) {
                m_List.SetSelectedIndex(-1);
                m_List.Invalidate();
                m_List.Update();
			 	m_Toolbar.SetButton(1, true, false, LOADSTRING(IDS_STR_OPERATE).C_Str());
				m_Toolbar.Invalidate();
				m_Toolbar.Update();
               return 0;
            }
       }
    }
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

void Ui_BackupWnd::updateList(){
	m_List.RemoveAll();

	//获取所有文件夹
	list<CMzString> _dirs;
	int nDir = File::ListDirectory(BACKUP_DIR,_dirs);
	if(nDir == 0){
		return;	//no list
	}

	//排序目录
	list<SYSTEMTIME> dirtimes;
	list<CMzString>::iterator i = _dirs.begin();
	for(; i != _dirs.end();i++){
		CMzString dir = *i;
		SYSTEMTIME s;
		swscanf(dir.C_Str(),L"%04d%02d%02d%02d%02d%02d",
			&s.wYear,&s.wMonth,&s.wDay,&s.wHour,&s.wMinute,&s.wSecond);
		if(s.wYear == 0 || s.wMonth == 0 || s.wDay == 0 ||
			s.wYear > 2100 || s.wMonth > 12 || s.wDay > 31 ||
			s.wHour > 23 || s.wMinute > 59 || s.wSecond > 59){	//无效
			continue;
		}
		if(dirtimes.size() == 0){
			dirtimes.push_back(s);
		}else{
			list<SYSTEMTIME>::iterator it = dirtimes.begin();
			for(;it != dirtimes.end(); it++){
				SYSTEMTIME tm = *it;
				if(DateTime::compareDate(s,tm)>0){
					dirtimes.insert(it,s);
					break;
				}
			}
			if(it == dirtimes.end()){
				dirtimes.push_back(s);
			}
		}
	}

	ListItem li;
	list<SYSTEMTIME>::iterator it = dirtimes.begin();
	for(; it != dirtimes.end(); it++){
		SYSTEMTIME tm = *it;
		wchar_t dispname[32];
		wsprintf(dispname,L"%04d-%02d-%02d %02d:%02d:%02d",
			tm.wYear,tm.wMonth,tm.wDay,tm.wHour,tm.wMinute,tm.wSecond);
		li.Text = dispname;
		m_List.AddItem(li);
	}
	m_List.Invalidate();
	m_List.Update();
}

//恢复前必须断开数据库连接
BOOL Ui_BackupWnd::brecover(CMzString &itemname){
	BOOL nRet = false;
	SYSTEMTIME s;
	swscanf(itemname.C_Str(),L"%04d-%02d-%02d %02d:%02d:%02d",
		&s.wYear,&s.wMonth,&s.wDay,&s.wHour,&s.wMinute,&s.wSecond);

	wchar_t dispname[32];
	wsprintf(dispname,L"%04d%02d%02d%02d%02d%02d",
		s.wYear,s.wMonth,s.wDay,s.wHour,s.wMinute,s.wSecond);

	CMzString fpath = BACKUP_DIR;
	fpath = fpath + L"\\";
	fpath = fpath + dispname;
	//获取当前目录
	wchar_t currpath[320];
	File::GetCurrentPath(currpath);
	//准备文件
	list<CMzString> sfile;
	sfile.push_back(L"cash.db");
	sfile.push_back(L"m8cash.ini");
	sfile.push_back(L"reminder.ini");

	nRet = File::BackupFiles(fpath,currpath,sfile);
	return nRet;
}

BOOL Ui_BackupWnd::bdelete(CMzString &itemname){
	BOOL nRet = false;
	SYSTEMTIME s;
	swscanf(itemname.C_Str(),L"%04d-%02d-%02d %02d:%02d:%02d",
		&s.wYear,&s.wMonth,&s.wDay,&s.wHour,&s.wMinute,&s.wSecond);

	wchar_t dispname[32];
	wsprintf(dispname,L"%04d%02d%02d%02d%02d%02d",
		s.wYear,s.wMonth,s.wDay,s.wHour,s.wMinute,s.wSecond);

	CMzString fpath = BACKUP_DIR;
	fpath = fpath + L"\\";
	fpath = fpath + dispname;
	nRet = File::DelDirectory(fpath.C_Str());
	return nRet;
}

BOOL Ui_BackupWnd::bbackup(){
	BOOL nRet = false;
	//检查备份目录是否存在
	nRet = File::DirectoryExists_New(BACKUP_PARENT_DIR0);
	nRet = File::DirectoryExists_New(BACKUP_PARENT_DIR1);
	//如果不存在则新建该目录
	nRet = File::DirectoryExists_New(BACKUP_DIR);
	//根据日期创建备份目录
	CMzString dir = File::CreateDirectoryByDate(BACKUP_DIR,nRet);
	//准备文件
	list<CMzString> s;
	s.push_back(L"cash.db");
	s.push_back(L"m8cash.ini");
	s.push_back(L"reminder.ini");
	//获取当前目录
	wchar_t currpath[320];
	File::GetCurrentPath(currpath);
	//备份数据
	File::BackupFiles(currpath,dir,s);
	return nRet;
}