#include "m8cash.h"
#include "ui_reminder.h"
#include "ui_password.h"
#include <MotorVibrate.h>

#include <cMzCommon.h>
using namespace cMzCommon;
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
	bool ret = false;
	if(cash_db.connect()){
		//popup password dialog
		//try if there is a password
		
        if(cash_db.checkpwd(NULL,0)){
            g_bencypt = false;
            ret = true;
        }else{
            g_bencypt = true;
			Ui_PasswordWnd dlg;
			RECT rcWork = MzGetWorkArea();
			dlg.setMode(0);
			dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
				m_MainWnd.m_hWnd, 0, WS_POPUP);
			// set the animation of the window
			int nRet = dlg.DoModal();
			if (nRet == ID_OK) {
				ret = true;
			}
        }
        if(ret){
		    //检查记录版本
		    cash_db.versionUpdate(m_MainWnd.m_hWnd);
		    cash_db.recover();
            cash_db.load();
        }
	}else{
		//检查记录版本
		cash_db.versionUpdate(m_MainWnd.m_hWnd);
		cash_db.recover();
		cash_db.load();
	}
	return ret;
}

BOOL M8CashApp::Init() {
    // Init the COM relative library.
    CoInitializeEx(0, COINIT_MULTITHREADED);
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
	HANDLE m_hCHDle = CreateMutex(NULL,true,L"M8Cash");
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		HWND pWnd=FindWindow(m_MainWnd.GetMzClassName(),NULL);
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
	//SetForegroundWindow(m_MainWnd.m_hWnd);
	//检测密码
		// Create the main window
		RECT rcWork = MzGetWorkArea();
		m_MainWnd.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork), 0, 0, 0);
	if(checkpwd()){
		m_MainWnd.Show();
		m_MainWnd.updateText();
		SetForegroundWindow(m_MainWnd.m_hWnd);
	}

    // return TRUE means init success.
    return TRUE;
}