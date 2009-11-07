#include "ui_config.h"
#include "ui_accounts.h"
#include "ui_categories.h"
#include "ui_about.h"
#include "ui_imexport.h"
#include "ui_password.h"
#include "ui_backup.h"
#include "m8cash.h"
#include "..\MzCommon\MzCommon.h"
using namespace MzCommon;
#include "..\MzCommon\UiSingleOption.h"

MZ_IMPLEMENT_DYNAMIC(Ui_ConfigWnd)

#define MZ_IDC_TOOLBAR_CONFIG 101
#define MZ_IDC_CONFIG_SCROLLWIN 104
#define MZ_IDC_BUTTON_CFG_RESET 105
#define MZ_IDC_BUTTON_CFG_IMPORT 106
#define MZ_IDC_BUTTON_CFG_EXPORT 107
#define MZ_IDC_BUTTON_SET_PASSWORD 108
#define MZ_IDC_BUTTON_CFG_BACKUP 109
#define MZ_IDC_BUTTON_CFG_OPTIMIZE	110
#define MZ_IDC_BUTTON_CFG_CATLEVEL	111
#define MZ_IDC_BTN_AUTOLOCK 112

#define IDC_PPM_CANCEL	100

BOOL Ui_ConfigWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 0;
    m_ScrollWin.SetPos(0, 0, GetWidth(), GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR);
    m_ScrollWin.SetID(MZ_IDC_CONFIG_SCROLLWIN);
    m_ScrollWin.EnableScrollBarV(true);
    AddUiWin(&m_ScrollWin);

    //set the right image of the extended button m_BtnSetting1
    m_BtnExport.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnExport.SetText(LOADSTRING(IDS_STR_IMPORT).C_Str());
    m_BtnExport.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_BtnExport.SetID(MZ_IDC_BUTTON_CFG_IMPORT);
    m_ScrollWin.AddChild(&m_BtnExport);
    m_BtnExport.SetImage2(imgArrow);
    m_BtnExport.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnExport.SetShowImage2(true);

	m_BtnExport.SetImage1(pimg[IDB_PNG_EXPORT - IDB_PNG_CALC]);
	m_BtnExport.SetImageWidth1(pimg[IDB_PNG_EXPORT - IDB_PNG_CALC]->GetImageWidth());
	m_BtnExport.SetShowImage1(true);

    y += MZM_HEIGHT_BUTTONEX;
    m_BtnImport.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnImport.SetText(LOADSTRING(IDS_STR_EXPORT).C_Str());
    m_BtnImport.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_BtnImport.SetID(MZ_IDC_BUTTON_CFG_EXPORT);
    m_ScrollWin.AddChild(&m_BtnImport);
    m_BtnImport.SetImage2(imgArrow);
    m_BtnImport.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnImport.SetShowImage2(true);

	m_BtnImport.SetImage1(pimg[IDB_PNG_IMPORT - IDB_PNG_CALC]);
	m_BtnImport.SetImageWidth1(pimg[IDB_PNG_IMPORT - IDB_PNG_CALC]->GetImageWidth());
	m_BtnImport.SetShowImage1(true);

    y += MZM_HEIGHT_BUTTONEX;
    m_BtnBackup.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnBackup.SetText(LOADSTRING(IDS_STR_BACKUP_DB).C_Str());
    m_BtnBackup.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_BtnBackup.SetID(MZ_IDC_BUTTON_CFG_BACKUP);
	m_BtnBackup.SetTextMaxLen(0);
    m_ScrollWin.AddChild(&m_BtnBackup);
    m_BtnBackup.SetImage2(imgArrow);
    m_BtnBackup.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnBackup.SetShowImage2(true);

	m_BtnBackup.SetImage1(pimg[IDB_PNG_BACKUP - IDB_PNG_CALC]);
	m_BtnBackup.SetImageWidth1(pimg[IDB_PNG_BACKUP - IDB_PNG_CALC]->GetImageWidth());
	m_BtnBackup.SetShowImage1(true);

	y += MZM_HEIGHT_BUTTONEX;
    m_BtnSetPassword.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnSetPassword.SetText(LOADSTRING(IDS_STR_SETUP_PWD).C_Str());
    m_BtnSetPassword.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_BtnSetPassword.SetID(MZ_IDC_BUTTON_SET_PASSWORD);
    m_BtnSetPassword.SetImage2(imgArrow);
    m_BtnSetPassword.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnSetPassword.SetShowImage2(true);
	m_BtnSetPassword.SetTextMaxLen(0);
    m_ScrollWin.AddChild(&m_BtnSetPassword);

	m_BtnSetPassword.SetImage1(pimg[IDB_PNG_ENCRYPT - IDB_PNG_CALC]);
	m_BtnSetPassword.SetImageWidth1(pimg[IDB_PNG_ENCRYPT - IDB_PNG_CALC]->GetImageWidth());
	m_BtnSetPassword.SetShowImage1(true);

	y += MZM_HEIGHT_BUTTONEX;
	c_expend_level = appconfig.IniCategoryExpandLevel.Get();

    m_BtnCategoryExpand.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnCategoryExpand.SetText(LOADSTRING(IDS_STR_CONFIG_CATEGORY_EXPAND).C_Str());
    m_BtnCategoryExpand.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_BtnCategoryExpand.SetID(MZ_IDC_BUTTON_CFG_CATLEVEL);
    m_BtnCategoryExpand.SetImage2(imgArrow);
    m_BtnCategoryExpand.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnCategoryExpand.SetShowImage2(true);
	m_BtnCategoryExpand.SetTextMaxLen(0);
	wchar_t strlevel[2] = {c_expend_level + 0x30, 0};
	m_BtnCategoryExpand.SetText2(strlevel);
    m_ScrollWin.AddChild(&m_BtnCategoryExpand);

	m_BtnCategoryExpand.SetImage1(pimg[IDB_PNG_CATEGORY - IDB_PNG_CALC]);
	m_BtnCategoryExpand.SetImageWidth1(pimg[IDB_PNG_CATEGORY - IDB_PNG_CALC]->GetImageWidth());
	m_BtnCategoryExpand.SetShowImage1(true);

	y += MZM_HEIGHT_BUTTONEX;
    m_BtnResetAll.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnResetAll.SetTextMaxLen(0);
    m_BtnResetAll.SetText(LOADSTRING(IDS_STR_RECOVER_ALARM).C_Str());
    m_BtnResetAll.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_BtnResetAll.SetID(MZ_IDC_BUTTON_CFG_RESET);
    m_ScrollWin.AddChild(&m_BtnResetAll);

	m_BtnResetAll.SetImage1(pimg[IDB_PNG_RELOADALERT - IDB_PNG_CALC]);
	m_BtnResetAll.SetImageWidth1(pimg[IDB_PNG_RELOADALERT - IDB_PNG_CALC]->GetImageWidth());
	m_BtnResetAll.SetShowImage1(true);

	y += MZM_HEIGHT_BUTTONEX;
    m_BtnOptimize.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnOptimize.SetTextMaxLen(0);
    m_BtnOptimize.SetText(LOADSTRING(IDS_STR_OPTDB).C_Str());
    m_BtnOptimize.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_BtnOptimize.SetID(MZ_IDC_BUTTON_CFG_OPTIMIZE);
    m_ScrollWin.AddChild(&m_BtnOptimize);

	m_BtnOptimize.SetImage1(pimg[IDB_PNG_OPTIMIZE - IDB_PNG_BEGIN]);
	m_BtnOptimize.SetImageWidth1(pimg[IDB_PNG_OPTIMIZE - IDB_PNG_BEGIN]->GetImageWidth());
	m_BtnOptimize.SetShowImage1(true);

	y+=MZM_HEIGHT_BUTTONEX;
	m_BtnAutoLock.SetPos(0,y,GetWidth(),MZM_HEIGHT_BUTTONEX);
	m_BtnAutoLock.SetText(LOADSTRING(IDS_STR_CONFIG_AUTOLOCK).C_Str());
    m_BtnAutoLock.SetImage1(pimg[IDB_PNG_LOGIN - IDB_PNG_CALC]);
    m_BtnAutoLock.SetImageWidth1(pimg[IDB_PNG_LOGIN - IDB_PNG_CALC]->GetImageWidth());
	m_BtnAutoLock.SetID(MZ_IDC_BTN_AUTOLOCK);
    m_BtnAutoLock.SetShowImage1(true);
	
	m_ScrollWin.AddChild(&m_BtnAutoLock);

	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_RETURN).C_Str());
    m_Toolbar.EnableLeftArrow(true);
    m_Toolbar.SetButton(2, true, true, LOADSTRING(IDS_STR_ABOUT).C_Str());
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_CONFIG);
    AddUiWin(&m_Toolbar);

	updateAutolock();
    return TRUE;
}

void Ui_ConfigWnd::updateAutolock(){
	if(g_bencypt){
		m_BtnAutoLock.SetVisible(true);
		DWORD lock = appconfig.IniAutolock.Get();
		WORD locktype = LOWORD(lock);
		if(locktype != 0){
			m_BtnAutoLock.SetSwitchStatus(TRUE);
			WORD locktimeout = HIWORD(lock);
			if(locktimeout > 1){
				wchar_t s[16];
				if(locktimeout < 60){
					wsprintf(s,L"%d %s",locktimeout,LOADSTRING(IDS_STR_SECONDS).C_Str());
				}else{
					wsprintf(s,L"%d %s",locktimeout/60,LOADSTRING(IDS_STR_MIN).C_Str());
				}
				m_BtnAutoLock.SetText2(s);
			}else{
				m_BtnAutoLock.SetText2(LOADSTRING(IDS_STR_AUTOLOCK_IMM).C_Str());
			}	
		}else{
			m_BtnAutoLock.SetSwitchStatus(FALSE);
			m_BtnAutoLock.SetText2(LOADSTRING(IDS_STR_DISABLED).C_Str());
		}
	}else{
		m_BtnAutoLock.SetVisible(false);
	}
	m_BtnAutoLock.Invalidate();

}

void Ui_ConfigWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    int nRet;
    RECT rcWork;
    Ui_AccountsWnd accountdlg;
    Ui_CategoriesWnd catdlg;
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
        case MZ_IDC_BUTTON_CFG_EXPORT:
		{
			Ui_ImExportWnd dlg;
            rcWork = MzGetWorkArea();
			dlg.setMode(false);
            dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
                    m_hWnd, 0, WS_POPUP);
            // set the animation of the window
            dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
            dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
            nRet = dlg.DoModal();
            if (nRet == ID_OK) {
                //...
            }
			break;
		}
        case MZ_IDC_BUTTON_CFG_IMPORT:
		{
			Ui_ImExportWnd dlg;
            rcWork = MzGetWorkArea();
			dlg.setMode(true);
            dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
                    m_hWnd, 0, WS_POPUP);
            // set the animation of the window
            dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
            dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
            nRet = dlg.DoModal();
            if (nRet == ID_OK) {
                //...
            }
			break;
		}
        case MZ_IDC_BUTTON_CFG_BACKUP:
		{
			Ui_BackupWnd dlg;
            rcWork = MzGetWorkArea();
            dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
                    m_hWnd, 0, WS_POPUP);
            // set the animation of the window
            dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
            dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
            nRet = dlg.DoModal();
            if (nRet == ID_OK) {
                //...
            }
			break;
		}
        case MZ_IDC_TOOLBAR_CONFIG:
        {
            int nIndex = lParam;
            if (nIndex == 0) {
                // exit the modal dialog
                EndModal(ID_CANCEL);
                return;
            }

            if (nIndex == 2) {//about
                //about
				UI_AboutWnd dlg;
				rcWork = MzGetWorkArea();
				dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
						m_hWnd, 0, WS_POPUP);
				// set the animation of the window
				dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
				dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
				nRet = dlg.DoModal();
                return;
            }
        }
        case MZ_IDC_BUTTON_SET_PASSWORD:
		{
			Ui_PasswordWnd dlg;
            rcWork = MzGetWorkArea();
			dlg.setMode(1);
            dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
                    m_hWnd, 0, WS_POPUP);
            // set the animation of the window
            dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
            dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
            nRet = dlg.DoModal();
            if (nRet == ID_OK) {
				//
				updateAutolock();
            }
			break;
		}
		case MZ_IDC_BUTTON_CFG_RESET:
		{
			if(MzMessageBoxEx(m_hWnd,
				LOADSTRING(IDS_STR_WARN_ALARM_RESTORE).C_Str(),
				LOADSTRING(IDS_STR_CONFIRM).C_Str(),MZ_YESNO,false) == 1){
				cash_reminder.clearReminder();
				cash_reminder.loadReminderList();
				cash_reminder.recoverReminder();
				cash_reminder.saveReminderList();
				MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_ALARM_RESTORE_S).C_Str());
			}
			break;
		}
		case MZ_IDC_BUTTON_CFG_OPTIMIZE:
		{
			MzBeginWaitDlg(m_hWnd);
			cash_db.reorgDatebase();
			MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_DB_OPT_S).C_Str());
			MzEndWaitDlg();
			break;
		}
		case MZ_IDC_BUTTON_CFG_CATLEVEL:
		{
			c_expend_level++;
			if(c_expend_level > 2) c_expend_level = 0;
			wchar_t strlevel[2] = {c_expend_level + 0x30, 0};
			m_BtnCategoryExpand.SetText2(strlevel);
			m_BtnCategoryExpand.Invalidate();
			appconfig.IniCategoryExpandLevel.Set(c_expend_level);
			break;
		}
		case MZ_IDC_BTN_AUTOLOCK + 0x100:
			if(appconfig.IniAutolock.Get()){
				appconfig.IniAutolock.Set(0);
				updateAutolock();
				break;
			}
		case MZ_IDC_BTN_AUTOLOCK:
		{
			Ui_SingleOptionWnd dlg;
			UINT AutoLockTimeOut[] = {1,15,30,60,300};
			wchar_t sTimeout[16];
			for(int i = 0; i < sizeof(AutoLockTimeOut)/sizeof(AutoLockTimeOut[0]); i++){
				if(AutoLockTimeOut[i] <= 1){
					wsprintf(sTimeout,L"%s",LOADSTRING(IDS_STR_AUTOLOCK_IMM).C_Str());
				}else{
					if(AutoLockTimeOut[i] < 60){
						wsprintf(sTimeout,L"%d %s",AutoLockTimeOut[i],LOADSTRING(IDS_STR_SECONDS).C_Str());
					}else{
						wsprintf(sTimeout,L"%d %s",AutoLockTimeOut[i]/60,LOADSTRING(IDS_STR_MIN).C_Str());
					}
				}
				dlg.AppendOptionItem(sTimeout);
			}
			DWORD lock = appconfig.IniAutolock.Get();
			WORD locktype = LOWORD(lock);
			WORD locktimeout = HIWORD(lock);
			int selidx = 0;
			if(locktype != 0){
				for(int i = 0; i < sizeof(AutoLockTimeOut)/sizeof(AutoLockTimeOut[0]); i++){
					if(locktimeout == AutoLockTimeOut[i]){
						selidx = i;
						break;
					}
				}
			}

			dlg.SetSelectedIndex(selidx);
			dlg.SetTitleText(LOADSTRING(IDS_STR_AUTOLOCK_TIP).C_Str());
			RECT rcWork = MzGetWorkArea();
			dlg.Create(rcWork.left + 40, rcWork.top + 120, RECT_WIDTH(rcWork) - 80, 210 + 70*3,
				m_hWnd, 0, WS_POPUP);
			// set the animation of the window
			dlg.SetAnimateType_Show(MZ_ANIMTYPE_NONE);
			dlg.SetAnimateType_Hide(MZ_ANIMTYPE_FADE);
			int nRet = dlg.DoModal();
			if(nRet == ID_OK){
				appconfig.IniAutolock.Set(MAKELONG(1,AutoLockTimeOut[dlg.GetSelectedIndex()]));
				updateAutolock();
			}
			break;
		}
    }
}