#include "m8cash.h"
#include "ui_reminder.h"
#include "ui_password.h"
#include <MotorVibrate.h>

#include "..\MzCommon\MzCommon.h"
using namespace MzCommon;
// The global variable of the application.
M8CashApp theApp;
wchar_t db_path[256];
clsCASHDB cash_db;
wchar_t ini_reminder[256];
CashReminder cash_reminder;

ImagingHelper *pimg[IDB_PNG_END - IDB_PNG_BEGIN + 1];
ImagingHelper *imgArrow;
HINSTANCE LangresHandle;
HINSTANCE ImgresHandle;
CashConfig appconfig;
bool g_bencypt;	//数据库是否加密
bool g_pwddlgshow;

void M8CashApp::loadImageRes(){
	ImgresHandle = MzGetInstanceHandle();
	for(int i = 0; i < sizeof(pimg) / sizeof(pimg[0]); i++){
		pimg[i] = LOADIMAGE(IDB_PNG_BEGIN + i);
	}
	HINSTANCE MzresHandle = GetMzResModuleHandle();
	imgArrow = ImagingHelper::GetImageObject(MzresHandle, MZRES_IDR_PNG_ARROW_RIGHT, true);
}

bool M8CashApp::checkpwd(){
	wchar_t currpath[128];
	bool ret = true;
	if(File::GetCurrentPath(currpath)){
		wsprintf(db_path,L"%s\\cash.db",currpath);
	}else{
		wsprintf(db_path,DEFAULT_DB);
	}
	if(cash_db.connect(db_path)){
		//popup password dialog
		//try if there is a password
		g_bencypt = false;
		cash_db.decrypt(NULL,0);
		//检查记录版本
		cash_db.versionUpdate(m_pMainWnd->m_hWnd);
		cash_db.recover();
		if(!cash_db.load()){
			g_bencypt = true;
			cash_db.disconnect();
			//cash_db.connect(db_path);
			Ui_PasswordWnd dlg;
			RECT rcWork = MzGetWorkArea();
			dlg.setMode(0);
			dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
				m_pMainWnd->m_hWnd, 0, WS_POPUP);
			// set the animation of the window
			int nRet = dlg.DoModal();
			if (nRet == ID_OK) {
				ret = true;
			}else{
				ret = false;
			}
		}
	}else{
		//检查记录版本
		cash_db.versionUpdate(m_pMainWnd->m_hWnd);
		cash_db.recover();
		cash_db.load();
	}
	return ret;
}

BOOL M8CashApp::Init() {
    // Init the COM relative library.
    CoInitializeEx(0, COINIT_MULTITHREADED);
	m_pMainWnd = NULL;
	//载入资源
	LangresHandle = LoadLibrary(L"language.dll");
	if(LangresHandle){
		isExternLangres = true;
	}else{
		LangresHandle = MzGetInstanceHandle();
		isExternLangres = false;
	}

	// 载入提醒
	cash_reminder.loadReminderList();

	// 判断是否是提醒调用
	LPWSTR str = GetCommandLine();
	//str = L"AppRunToHandleNotification 0x360000B1";
	int handle = 0;
	wchar_t prestr[1024];
	if(lstrlen(str)){
		swscanf(str,L"AppRunToHandleNotification 0x%x",&handle);
		ReminderInfo_ptr p = cash_reminder.getReminderByEventId(handle);
		cash_reminder.setNextReminder(p);
		cash_reminder.saveReminderList();
		wchar_t tmp[1024];
		if(p && p->text.C_Str()){
			C::restoreWrap(tmp,p->text.C_Str());
			wsprintf(prestr,L"M8Cash财务提醒:\n%s",tmp);
		}else{
			wsprintf(prestr,L"M8Cash财务提醒");
		}
		MzSetVibrateOn(MZ_VIBRATE_ON_TIME,MZ_VIBRATE_OFF_TIME);
		//SetBackLightState(true);	//开背光
		//if(IsLockPhoneStatus()){	//解锁
			//MzLeaveLockPhone();
		//}
		while(MzMessageBoxEx(0, prestr, L"Exit", MB_OK) != 1);
		MzSetVibrateOff();
		PostQuitMessage(0);
		return true;
	}
	//正常启动程序
	//检测程序是否已经运行
	m_pMainWnd = new Ui_MainWnd;
	HANDLE m_hCHDle = CreateMutex(NULL,true,L"M8Cash");
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		HWND pWnd=FindWindow(m_pMainWnd->GetMzClassName(),NULL);
		//HWND pWnd=FindWindow(NULL,L"M8Cash");
		if(pWnd)
		{
			SetForegroundWindow(pWnd);
			PostMessage(pWnd,WM_NULL,NULL,NULL);
		}
		PostQuitMessage(0);
		return true; 
	}
	//载入图片
	loadImageRes();
	//SetForegroundWindow(m_pMainWnd->m_hWnd);
	//检测密码
		// Create the main window
		RECT rcWork = MzGetWorkArea();
		m_pMainWnd->Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork), 0, 0, 0);
	if(checkpwd()){
		m_pMainWnd->Show();
		m_pMainWnd->updateText();
		SetForegroundWindow(m_pMainWnd->m_hWnd);
	}

    // return TRUE means init success.
    return TRUE;
}