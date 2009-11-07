#include "ui_filter.h"
#include "m8cash.h"
#include "ui_calendar.h" //选择日期

#include "..\MzCommon\MzCommon.h"
using namespace MzCommon;
#include "ui_accounts.h"
#include "ui_categories.h"
#include "ui_person.h"

MZ_IMPLEMENT_DYNAMIC(Ui_FilterWnd)

#define MZ_IDC_TOOLBAR_REMINDER 101

#define MZ_IDC_BTN_START_DATE	102
#define MZ_IDC_BTN_END_DATE		103
#define MZ_IDC_BTN_CATEGORY		104
#define MZ_IDC_BTN_ACCOUNT		105
#define MZ_IDC_BTN_PERSON		106

//////

Ui_FilterWnd::Ui_FilterWnd(void)
{
}

Ui_FilterWnd::~Ui_FilterWnd(void)
{
}

BOOL Ui_FilterWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 0;
	m_Caption.SetPos(0,y,GetWidth(),MZM_HEIGHT_CAPTION);
	m_Caption.SetText(LOADSTRING(IDS_STR_CONFIG_FILTER).C_Str());;
	AddUiWin(&m_Caption);

	y+=MZM_HEIGHT_CAPTION;
	m_BtnStartDate.SetPos(0,y, GetWidth() - 120, MZM_HEIGHT_BUTTONEX);
	m_BtnStartDate.SetText(LOADSTRING(IDS_STR_START_DATE).C_Str());
    m_BtnStartDate.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnStartDate.SetTextMaxLen(0);
	m_BtnStartDate.SetEnable(false);
    AddUiWin(&m_BtnStartDate);

	m_BtnStartDateSW.SetPos(GetWidth() - 120,y, 120, MZM_HEIGHT_BUTTONEX);
    m_BtnStartDateSW.SetButtonType(MZC_BUTTON_SWITCH);
	m_BtnStartDateSW.SetButtonMode(MZC_BUTTON_MODE_HOLD);
	m_BtnStartDateSW.SetID(MZ_IDC_BTN_START_DATE);
    AddUiWin(&m_BtnStartDateSW);

	y+=MZM_HEIGHT_BUTTONEX;
	m_BtnEndDate.SetPos(0,y,GetWidth() - 120,MZM_HEIGHT_BUTTONEX);
    m_BtnEndDate.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnEndDate.SetText(LOADSTRING(IDS_STR_END_DATE).C_Str());
	m_BtnEndDate.SetEnable(false);
	AddUiWin(&m_BtnEndDate);

	m_BtnEndDateSW.SetPos(GetWidth() - 120,y, 120, MZM_HEIGHT_BUTTONEX);
    m_BtnEndDateSW.SetButtonType(MZC_BUTTON_SWITCH);
	m_BtnEndDateSW.SetButtonMode(MZC_BUTTON_MODE_HOLD);
	m_BtnEndDateSW.SetID(MZ_IDC_BTN_END_DATE);
    AddUiWin(&m_BtnEndDateSW);

	y+=MZM_HEIGHT_BUTTONEX;
	m_BtnCategory.SetPos(0,y,GetWidth() - 120,MZM_HEIGHT_BUTTONEX);
    m_BtnCategory.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnCategory.SetText(LOADSTRING(IDS_STR_CATEGORY).C_Str());
	m_BtnCategory.SetEnable(false);
	AddUiWin(&m_BtnCategory);

	m_BtnCategorySW.SetPos(GetWidth() - 120,y, 120, MZM_HEIGHT_BUTTONEX);
    m_BtnCategorySW.SetButtonType(MZC_BUTTON_SWITCH);
	m_BtnCategorySW.SetButtonMode(MZC_BUTTON_MODE_HOLD);
	m_BtnCategorySW.SetID(MZ_IDC_BTN_CATEGORY);
    AddUiWin(&m_BtnCategorySW);

	y+=MZM_HEIGHT_BUTTONEX;
	m_BtnAccount.SetPos(0,y,GetWidth() - 120,MZM_HEIGHT_BUTTONEX);
    m_BtnAccount.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnAccount.SetText(LOADSTRING(IDS_STR_ACCOUNT).C_Str());
	m_BtnAccount.SetEnable(false);
	AddUiWin(&m_BtnAccount);

	m_BtnAccountSW.SetPos(GetWidth() - 120,y, 120, MZM_HEIGHT_BUTTONEX);
    m_BtnAccountSW.SetButtonType(MZC_BUTTON_SWITCH);
	m_BtnAccountSW.SetButtonMode(MZC_BUTTON_MODE_HOLD);
	m_BtnAccountSW.SetID(MZ_IDC_BTN_ACCOUNT);
    AddUiWin(&m_BtnAccountSW);

	y+=MZM_HEIGHT_BUTTONEX;
	m_BtnPerson.SetPos(0,y,GetWidth() - 120,MZM_HEIGHT_BUTTONEX);
    m_BtnPerson.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnPerson.SetText(LOADSTRING(IDS_STR_PERSON_TITLE).C_Str());
	m_BtnPerson.SetEnable(false);
	AddUiWin(&m_BtnPerson);

	m_BtnPersonSW.SetPos(GetWidth() - 120,y, 120, MZM_HEIGHT_BUTTONEX);
    m_BtnPersonSW.SetButtonType(MZC_BUTTON_SWITCH);
	m_BtnPersonSW.SetButtonMode(MZC_BUTTON_MODE_HOLD);
	m_BtnPersonSW.SetID(MZ_IDC_BTN_PERSON);
    AddUiWin(&m_BtnPersonSW);

	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_RETURN).C_Str());
	m_Toolbar.EnableLeftArrow(true);
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_REMINDER);
    AddUiWin(&m_Toolbar);
	updateUi();
    return TRUE;
}

void Ui_FilterWnd::updateStartDate(){
	uRecordDate_t date_s;
	date_s.Value = appconfig.IniFilterStartDate.Get();
	if(date_s.Value == 0){
		m_BtnStartDateSW.SetState(MZCS_BUTTON_NORMAL);
		m_BtnStartDate.SetText2(LOADSTRING(IDS_STR_DISABLED).C_Str());
	}else{
		m_BtnStartDateSW.SetState(MZCS_BUTTON_PRESSED);
		wchar_t sdate[16];
		wsprintf(sdate,L"%04d-%02d-%02d",
			date_s.Date.Year,date_s.Date.Month,date_s.Date.Day);
		m_BtnStartDate.SetText2(sdate);
	}
	m_BtnStartDate.Invalidate();
	m_BtnStartDate.Update();
	m_BtnStartDateSW.Invalidate();
	m_BtnStartDateSW.Update();
}

void Ui_FilterWnd::updateEndDate(){
	uRecordDate_t date_e;
	date_e.Value = appconfig.IniFilterEndDate.Get();
	if(date_e.Value == 0){
		m_BtnEndDateSW.SetState(MZCS_BUTTON_NORMAL);
		m_BtnEndDate.SetText2(LOADSTRING(IDS_STR_DISABLED).C_Str());
	}else{
		m_BtnEndDateSW.SetState(MZCS_BUTTON_PRESSED);
		wchar_t sdate[16];
		wsprintf(sdate,L"%04d-%02d-%02d",
			date_e.Date.Year,date_e.Date.Month,date_e.Date.Day);
		m_BtnEndDate.SetText2(sdate);
	}
	m_BtnEndDate.Invalidate();
	m_BtnEndDate.Update();
	m_BtnEndDateSW.Invalidate();
	m_BtnEndDateSW.Update();
}

void Ui_FilterWnd::updateCategory(){
	DWORD category = appconfig.IniFilterCategory.Get();
	if(category == -1){
		m_BtnCategorySW.SetState(MZCS_BUTTON_NORMAL);
		m_BtnCategory.SetText2(LOADSTRING(IDS_STR_DISABLED).C_Str());
	}else{
		m_BtnCategorySW.SetState(MZCS_BUTTON_PRESSED);
		m_BtnCategory.SetText2(cash_db.getCategoryFullNameById(category));
	}
	m_BtnCategory.Invalidate();
	m_BtnCategory.Update();
	m_BtnCategorySW.Invalidate();
	m_BtnCategorySW.Update();
}

void Ui_FilterWnd::updateAccount(){
	DWORD account = appconfig.IniFilterAccount.Get();
	if(account == -1){
		m_BtnAccountSW.SetState(MZCS_BUTTON_NORMAL);
		m_BtnAccount.SetText2(LOADSTRING(IDS_STR_DISABLED).C_Str());
	}else{
		m_BtnAccountSW.SetState(MZCS_BUTTON_PRESSED);
		m_BtnAccount.SetText2(cash_db.getAccountNameById(account));
	}
	m_BtnAccount.Invalidate();
	m_BtnAccount.Update();
	m_BtnAccountSW.Invalidate();
	m_BtnAccountSW.Update();
}

void Ui_FilterWnd::updatePerson(){
	DWORD person = appconfig.IniFilterPerson.Get();
	if(person == -1){
		m_BtnPersonSW.SetState(MZCS_BUTTON_NORMAL);
		m_BtnPerson.SetText2(LOADSTRING(IDS_STR_DISABLED).C_Str());
	}else{
		m_BtnPersonSW.SetState(MZCS_BUTTON_PRESSED);
		m_BtnPerson.SetText2(cash_db.getPersonNameById(person));
	}
	m_BtnPerson.Invalidate();
	m_BtnPerson.Update();
	m_BtnPersonSW.Invalidate();
	m_BtnPersonSW.Update();
}

void Ui_FilterWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
		case MZ_IDC_BTN_START_DATE:
		{
			if(m_BtnStartDateSW.GetState() != MZCS_BUTTON_PRESSED){
				appconfig.IniFilterStartDate.Set(0);
			}else{
				Ui_CalendarWnd calendardlg;
				RECT rcWork = MzGetWorkArea();
				calendardlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
						m_hWnd, 0, WS_POPUP);
				// set the animation of the window
				calendardlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
				calendardlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
				int nRet = calendardlg.DoModal();
				if (nRet == ID_OK) {
					uRecordDate_t date;
					DWORD y,m,d;
					swscanf(calendardlg.getDate().C_Str(),L"%04d-%02d-%02d",
						&y,&m,&d);
					date.Date.Year = y;
					date.Date.Month = m;
					date.Date.Day = d;
					//日期始末范围判定
					DWORD date_e = appconfig.IniFilterEndDate.Get();
					if(date_e < date.Value && date_e != 0 && date.Value != 0) date.Value = date_e;

					appconfig.IniFilterStartDate.Set(date.Value);
				}
			}
			updateStartDate();
			break;
		}
		case MZ_IDC_BTN_END_DATE:
		{
			if(m_BtnEndDateSW.GetState() != MZCS_BUTTON_PRESSED){
				appconfig.IniFilterEndDate.Set(0);
			}else{
				Ui_CalendarWnd calendardlg;
				RECT rcWork = MzGetWorkArea();
				calendardlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
						m_hWnd, 0, WS_POPUP);
				// set the animation of the window
				calendardlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
				calendardlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
				int nRet = calendardlg.DoModal();
				if (nRet == ID_OK) {
					uRecordDate_t date;
					DWORD y,m,d;
					swscanf(calendardlg.getDate().C_Str(),L"%04d-%02d-%02d",
						&y,&m,&d);
					date.Date.Year = y;
					date.Date.Month = m;
					date.Date.Day = d;
					//日期始末范围判定
					DWORD date_s = appconfig.IniFilterStartDate.Get();
					if(date_s > date.Value && date_s != 0 && date.Value != 0) date.Value = date_s;
					appconfig.IniFilterEndDate.Set(date.Value);
				}
			}
			updateEndDate();
			break;
		}
		case MZ_IDC_BTN_CATEGORY:
		{
			if(m_BtnCategorySW.GetState() != MZCS_BUTTON_PRESSED){
				appconfig.IniFilterCategory.Set(-1);
			}else{
				Ui_CategoriesWnd dlg;
				RECT rcWork;
				rcWork = MzGetWorkArea();
				dlg.setMode(1);
				dlg.setNodeDepth(2);
				dlg.setHideCategory(-1);
				dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
						m_hWnd, 0, WS_POPUP);
				// set the animation of the window
				dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
				dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
				int nRet = dlg.DoModal();
				if (nRet == ID_OK) {
					int selectedId = dlg.getSelectionIndex();
					appconfig.IniFilterCategory.Set(selectedId);
				}
			}
			updateCategory();
			break;
		}
		case MZ_IDC_BTN_ACCOUNT:
		{
			if(m_BtnAccountSW.GetState() != MZCS_BUTTON_PRESSED){
				appconfig.IniFilterAccount.Set(-1);
			}else{
				Ui_AccountsWnd dlg;
				RECT rcWork;
				rcWork = MzGetWorkArea();
				dlg.setMode(1);
				dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
						m_hWnd, 0, WS_POPUP);
				// set the animation of the window
				dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
				dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
				int nRet = dlg.DoModal();
				if (nRet == ID_OK) {
					int selectedId = dlg.getSelectionIndex();
					appconfig.IniFilterAccount.Set(selectedId);
				}
			}
			updateAccount();
			break;
		}
		case MZ_IDC_BTN_PERSON:
		{
			if(m_BtnPersonSW.GetState() != MZCS_BUTTON_PRESSED){
				appconfig.IniFilterPerson.Set(-1);
			}else{
				Ui_PersonsWnd dlg;
				RECT rcWork;
				rcWork = MzGetWorkArea();
				dlg.setMode(1);
				dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
						m_hWnd, 0, WS_POPUP);
				// set the animation of the window
				dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
				dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
				int nRet = dlg.DoModal();
				if (nRet == ID_OK) {
					int selectedId = dlg.getSelectionIndex();
					appconfig.IniFilterPerson.Set(selectedId);
				}
			}
			updatePerson();
			break;
		}
		case MZ_IDC_TOOLBAR_REMINDER:
        {
            int nIndex = lParam;
			if(nIndex == 0){	//返回
				EndModal(ID_OK);
				return;
			}
		}
	}
}

LRESULT Ui_FilterWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}
