#include "ui_filter.h"
#include "m8cash.h"
#include "ui_calendar.h" //选择日期

#include <cMzCommon.h>
using namespace cMzCommon;
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

#define MZ_IDC_BTN_START_DATE_SW	202
#define MZ_IDC_BTN_END_DATE_SW		203
#define MZ_IDC_BTN_CATEGORY_SW		204
#define MZ_IDC_BTN_ACCOUNT_SW		205
#define MZ_IDC_BTN_PERSON_SW		206
//////

Ui_FilterWnd::Ui_FilterWnd(void)
{
	optionChanged = FALSE;
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
	m_Caption.SetPos(0,y,GetWidth(),MZM_HEIGHT_HEADINGBAR);
	m_Caption.SetText(LOADSTRING(IDS_STR_CONFIG_FILTER).C_Str());;
	AddUiWin(&m_Caption);

	y+=MZM_HEIGHT_HEADINGBAR;
	m_BtnStartDate.SetPos(0,y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnStartDate.SetText(LOADSTRING(IDS_STR_START_DATE).C_Str());
	m_BtnStartDate.SetID(MZ_IDC_BTN_START_DATE);
    AddUiWin(&m_BtnStartDate);

	y+=MZM_HEIGHT_BUTTONEX;
	m_BtnEndDate.SetPos(0,y,GetWidth(),MZM_HEIGHT_BUTTONEX);
	m_BtnEndDate.SetText(LOADSTRING(IDS_STR_END_DATE).C_Str());
	m_BtnEndDate.SetID(MZ_IDC_BTN_END_DATE);
	AddUiWin(&m_BtnEndDate);

	y+=MZM_HEIGHT_BUTTONEX;
	m_BtnCategory.SetPos(0,y,GetWidth(),MZM_HEIGHT_BUTTONEX);
	m_BtnCategory.SetText(LOADSTRING(IDS_STR_CATEGORY).C_Str());
	m_BtnCategory.SetID(MZ_IDC_BTN_CATEGORY);
	AddUiWin(&m_BtnCategory);

	y+=MZM_HEIGHT_BUTTONEX;
	m_BtnAccount.SetPos(0,y,GetWidth(),MZM_HEIGHT_BUTTONEX);
	m_BtnAccount.SetText(LOADSTRING(IDS_STR_ACCOUNT).C_Str());
	m_BtnAccount.SetID(MZ_IDC_BTN_ACCOUNT);
	AddUiWin(&m_BtnAccount);

	y+=MZM_HEIGHT_BUTTONEX;
	m_BtnPerson.SetPos(0,y,GetWidth(),MZM_HEIGHT_BUTTONEX);
	m_BtnPerson.SetText(LOADSTRING(IDS_STR_PERSON_TITLE).C_Str());
	m_BtnPerson.SetID(MZ_IDC_BTN_PERSON);
	AddUiWin(&m_BtnPerson);

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
		m_BtnStartDate.SetSwitchStatus(FALSE);
		m_BtnStartDate.SetText2(LOADSTRING(IDS_STR_DISABLED).C_Str());
	}else{
		m_BtnStartDate.SetSwitchStatus(TRUE);
		wchar_t sdate[16];
		wsprintf(sdate,L"%04d-%02d-%02d",
			date_s.Date.Year,date_s.Date.Month,date_s.Date.Day);
		m_BtnStartDate.SetText2(sdate);
	}
	m_BtnStartDate.Invalidate();
}

void Ui_FilterWnd::updateEndDate(){
	uRecordDate_t date_e;
	date_e.Value = appconfig.IniFilterEndDate.Get();
	if(date_e.Value == 0){
		m_BtnEndDate.SetSwitchStatus(FALSE);
		m_BtnEndDate.SetText2(LOADSTRING(IDS_STR_DISABLED).C_Str());
	}else{
		m_BtnEndDate.SetSwitchStatus(TRUE);
		wchar_t sdate[16];
		wsprintf(sdate,L"%04d-%02d-%02d",
			date_e.Date.Year,date_e.Date.Month,date_e.Date.Day);
		m_BtnEndDate.SetText2(sdate);
	}
	m_BtnEndDate.Invalidate();
}

void Ui_FilterWnd::updateCategory(){
	DWORD category = appconfig.IniFilterCategory.Get();
	if(category == -1){
		m_BtnCategory.SetSwitchStatus(FALSE);
		m_BtnCategory.SetText2(LOADSTRING(IDS_STR_DISABLED).C_Str());
	}else{
		m_BtnCategory.SetSwitchStatus(TRUE);
		m_BtnCategory.SetText2(cash_db.getCategoryFullNameById(category));
	}
	m_BtnCategory.Invalidate();
}

void Ui_FilterWnd::updateAccount(){
	DWORD account = appconfig.IniFilterAccount.Get();
	if(account == -1){
		m_BtnAccount.SetSwitchStatus(FALSE);
		m_BtnAccount.SetText2(LOADSTRING(IDS_STR_DISABLED).C_Str());
	}else{
		m_BtnAccount.SetSwitchStatus(TRUE);
		m_BtnAccount.SetText2(cash_db.getAccountNameById(account));
	}
	m_BtnAccount.Invalidate();
}

void Ui_FilterWnd::updatePerson(){
	DWORD person = appconfig.IniFilterPerson.Get();
	if(person == -1){
		m_BtnPerson.SetSwitchStatus(FALSE);
		m_BtnPerson.SetText2(LOADSTRING(IDS_STR_DISABLED).C_Str());
	}else{
		m_BtnPerson.SetSwitchStatus(TRUE);
		m_BtnPerson.SetText2(cash_db.getPersonNameById(person));
	}
	m_BtnPerson.Invalidate();
}

void Ui_FilterWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
		case MZ_IDC_BTN_START_DATE + 0x100:
			if(!m_BtnStartDate.GetSwitchStatus()){
				appconfig.IniFilterStartDate.Set(0);
				updateStartDate();
				optionChanged = TRUE;
				break;
			}
		case MZ_IDC_BTN_START_DATE:
			ShowDateDlg();
			updateStartDate();
			break;
		case MZ_IDC_BTN_END_DATE + 0x100:
			if(!m_BtnEndDate.GetSwitchStatus()){
				appconfig.IniFilterEndDate.Set(0);
				updateEndDate();
				optionChanged = TRUE;
				break;
			}
		case MZ_IDC_BTN_END_DATE:
			ShowDateDlg(FALSE);
			updateEndDate();
			break;
		case MZ_IDC_BTN_CATEGORY + 0x100:
			if(!m_BtnCategory.GetSwitchStatus()){
				appconfig.IniFilterCategory.Set(-1);
				updateCategory();
				optionChanged = TRUE;
				break;
			}
		case MZ_IDC_BTN_CATEGORY:
			ShowCategoryDlg();
			updateCategory();
			break;
		case MZ_IDC_BTN_ACCOUNT + 0x100:
			if(!m_BtnAccount.GetSwitchStatus()){
				appconfig.IniFilterAccount.Set(-1);
				updateAccount();
				optionChanged = TRUE;
				break;
			}
		case MZ_IDC_BTN_ACCOUNT:
			ShowAccountDlg();
			updateAccount();
			break;
		case MZ_IDC_BTN_PERSON + 0x100:
			if(!m_BtnPerson.GetSwitchStatus()){
				appconfig.IniFilterPerson.Set(-1);
				updatePerson();
				optionChanged = TRUE;
				break;
			}
		case MZ_IDC_BTN_PERSON:
			ShowPersonDlg();
			updatePerson();
			break;
		case MZ_IDC_TOOLBAR_REMINDER:
        {
            int nIndex = lParam;
			if(nIndex == 0){	//返回
				if(optionChanged){
					EndModal(ID_OK);
				}else{
					EndModal(ID_CANCEL);
				}
				return;
			}
		}
	}
}

LRESULT Ui_FilterWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

void Ui_FilterWnd::ShowDateDlg(BOOL bstart){
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
		if(bstart){
			//日期始末范围判定
			DWORD date_e = appconfig.IniFilterEndDate.Get();
			if(date_e < date.Value && date_e != 0 && date.Value != 0) date.Value = date_e;
			if(appconfig.IniFilterStartDate.Set(date.Value)){
				optionChanged = TRUE;
			}
		}else{
			//日期始末范围判定
			DWORD date_s = appconfig.IniFilterStartDate.Get();
			if(date_s > date.Value && date_s != 0 && date.Value != 0) date.Value = date_s;
			if(appconfig.IniFilterEndDate.Set(date.Value)){
				optionChanged = TRUE;
			}
		}
	}
}

void Ui_FilterWnd::ShowCategoryDlg(){
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
		if(appconfig.IniFilterCategory.Set(selectedId)){
			optionChanged = TRUE;
		}
	}
}

void Ui_FilterWnd::ShowAccountDlg(){
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
		if(appconfig.IniFilterAccount.Set(selectedId)){
			optionChanged = TRUE;
		}
	}
}

void Ui_FilterWnd::ShowPersonDlg(){
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
		if(appconfig.IniFilterPerson.Set(selectedId)){
			optionChanged = TRUE;
		}
	}
}