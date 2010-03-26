#include "ui_main.h"
#include "ui_browse.h"
#include "ui_config.h"
#include "ui_accounts.h"
#include "ui_categories.h"
#include "ui_calendar.h"
#include "m8cash.h"
#include "ui_person.h"

#include <cMzCommon.h>
using namespace cMzCommon;
#include <ShellNotifyMsg.h>


#define MZ_IDC_TOOLBAR_MAIN 101
#define MZ_IDC_SCROLLWIN 102

#define MZ_IDC_BUTTON_DATE 103
#define MZ_IDC_BUTTON_ACCOUNT 104
#define MZ_IDC_BUTTON_CATEGORY 105
#define MZ_IDC_EDIT_YUAN 106
#define MZ_IDC_EDIT_NOTE 107
#define MZ_IDC_BUTTON_ADD 108
#define MZ_IDC_BUTTON_TOACCOUNT 109
#define MZ_IDC_CAPTION_TITLE 110
#define MZ_IDC_BUTTON_REMINDER 111
#define MZ_IDC_BUTTON_CALCULATOR 112
#define MZ_IDC_BUTTON_PERSON	113

MZ_IMPLEMENT_DYNAMIC(Ui_MainWnd)

Ui_MainWnd::Ui_MainWnd(){
	_isReminderEnabled = false;
	_isEditMode = false;
	pdlgpwd = 0;
}

Ui_MainWnd::~Ui_MainWnd(){
	::UnRegisterShellMessage(m_hWnd,WM_MZSH_ENTRY_LOCKPHONE);
	if(pdlgpwd) delete pdlgpwd;
}

BOOL Ui_MainWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 0;
	wchar_t name[128];
	wsprintf(name,L"%s v%s",LOADSTRING(IDS_STR_APPNAME).C_Str(),VER_STRING);
	m_lblTitle.SetPos(0,y,GetWidth(),MZM_HEIGHT_CAPTION/2);
	m_lblTitle.SetText(name);
	m_lblTitle.SetTextSize(m_lblTitle.GetTextSize()/2);
	m_lblTitle.SetTextWeight(FW_BOLD);
	m_lblTitle.SetID(MZ_IDC_CAPTION_TITLE);
	m_lblTitle.EnableNotifyMessage(true);
    AddUiWin(&m_lblTitle);

	y+=MZM_HEIGHT_CAPTION/2;

	m_BtnKeyBoard.SetPos(8, y + 10, 40, 40);
	m_BtnKeyBoard.SetSlotWin(&m_EdtAmountYuan);
	m_BtnKeyBoard.SetButtonImage(pimg[IDB_PNG_COINS - IDB_PNG_BEGIN]);

	m_BtnCalc.SetPos((GetWidth() - 140), y, 60, MZM_HEIGHT_SINGLELINE_EDIT);
	m_BtnCalc.SetButtonType(MZC_BUTTON_DOWNLOAD);
	m_BtnCalc.SetID(MZ_IDC_BUTTON_CALCULATOR);
	m_BtnCalc.SetImage_Normal(pimg[IDB_PNG_CALC - IDB_PNG_BEGIN]);
	m_BtnCalc.SetImage_Pressed(pimg[IDB_PNG_CALC - IDB_PNG_BEGIN]);
	AddUiWin(&m_BtnCalc);
	m_EdtAmountMask.SetSlotWin(&m_EdtAmountYuan);

	m_EdtAmountYuan.SetMaskWin(&m_EdtAmountMask);
	m_EdtAmountYuan.SetPos(5, y, (GetWidth() - 150), MZM_HEIGHT_SINGLELINE_EDIT);
//    m_EdtAmountYuan.SetTip2(LOADSTRING(IDS_STR_AMOUNT).C_Str());
	m_EdtAmountYuan.EnableMinus(false);
	m_EdtAmountYuan.EnableTips(true);
    m_EdtAmountYuan.SetLeftInvalid(50);
	m_EdtAmountYuan.SetID(MZ_IDC_EDIT_YUAN);
    AddUiWin(&m_EdtAmountYuan);

	AddUiWin(&m_EdtAmountMask);
	AddUiWin(&m_BtnKeyBoard);

	m_BtnAdd.SetPos((GetWidth() - 70), y, 60, MZM_HEIGHT_SINGLELINE_EDIT);
	m_BtnAdd.SetButtonType(MZC_BUTTON_DOWNLOAD);
	m_BtnAdd.SetID(MZ_IDC_BUTTON_ADD);
	AddUiWin(&m_BtnAdd);

	y+=MZM_HEIGHT_SINGLELINE_EDIT;
    m_ScrollWin.SetPos(0, y, GetWidth(), GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR);
    m_ScrollWin.SetID(MZ_IDC_SCROLLWIN);
    m_ScrollWin.EnableScrollBarV(true);
    AddUiWin(&m_ScrollWin);

	y = 0;
    m_lblDetail.SetPos(0, y, GetWidth(), MZM_HEIGHT_CAPTION);
	m_lblDetail.SetText(LOADSTRING(IDS_STR_DETAIL_INFO).C_Str());
    m_ScrollWin.AddChild(&m_lblDetail);

    y += MZM_HEIGHT_CAPTION;
    m_BtnDate.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnDate.SetText(LOADSTRING(IDS_STR_DATE).C_Str());
	m_BtnDate.SetText2(DateTime::Date());
    m_BtnDate.SetTextSize2(m_BtnDate.GetTextSize2() - 2);
    m_BtnDate.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_BtnDate.SetImage2(imgArrow);
    m_BtnDate.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnDate.SetShowImage2(true);
    m_BtnDate.SetID(MZ_IDC_BUTTON_DATE);
	m_BtnDate.SetTextMaxLen(0);
    m_ScrollWin.AddChild(&m_BtnDate);

	m_BtnDate.SetImage1(pimg[IDB_PNG_DATE - IDB_PNG_CALC]);
	m_BtnDate.SetShowImage1(true);
	m_BtnDate.SetImageWidth1(pimg[IDB_PNG_DATE - IDB_PNG_CALC]->GetImageWidth());

    y += MZM_HEIGHT_BUTTONEX;
    m_BtnReminder.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnReminder.SetText(LOADSTRING(IDS_STR_ALARM).C_Str());
    m_BtnReminder.SetText2(LOADSTRING(IDS_STR_DISABLED).C_Str());
    m_BtnReminder.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_ScrollWin.AddChild(&m_BtnReminder);
    m_BtnReminder.SetImage2(imgArrow);
    m_BtnReminder.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnReminder.SetShowImage2(true);
    m_BtnReminder.SetID(MZ_IDC_BUTTON_REMINDER);
	m_BtnReminder.SetTextMaxLen(0);

	m_BtnReminder.SetImage1(pimg[IDB_PNG_ALERT - IDB_PNG_BEGIN]);
	m_BtnReminder.SetShowImage1(true);
	m_BtnReminder.SetImageWidth1(pimg[IDB_PNG_ALERT - IDB_PNG_BEGIN]->GetImageWidth());

    y += MZM_HEIGHT_BUTTONEX;
    m_BtnAccounts.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnAccounts.SetText(LOADSTRING(IDS_STR_ACCOUNT).C_Str());
    m_BtnAccounts.SetTextSize2(m_BtnAccounts.GetTextSize2() - 2);
    m_BtnAccounts.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_ScrollWin.AddChild(&m_BtnAccounts);
    m_BtnAccounts.SetImage2(imgArrow);
    m_BtnAccounts.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnAccounts.SetShowImage2(true);
    m_BtnAccounts.SetID(MZ_IDC_BUTTON_ACCOUNT);
	m_BtnAccounts.SetTextMaxLen(0);

	m_BtnAccounts.SetImage1(pimg[IDB_PNG_ACCOUNT - IDB_PNG_CALC]);
	m_BtnAccounts.SetShowImage1(true);
	m_BtnAccounts.SetImageWidth1(pimg[IDB_PNG_ACCOUNT - IDB_PNG_CALC]->GetImageWidth());

    y += MZM_HEIGHT_BUTTONEX;
    m_BtnToAccounts.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnToAccounts.SetText(LOADSTRING(IDS_STR_TO_ACCOUNT).C_Str());
    m_BtnToAccounts.SetTextSize2(m_BtnToAccounts.GetTextSize2() - 2);
    m_BtnToAccounts.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_ScrollWin.AddChild(&m_BtnToAccounts);
    m_BtnToAccounts.SetImage2(imgArrow);
    m_BtnToAccounts.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnToAccounts.SetShowImage2(true);
    m_BtnToAccounts.SetID(MZ_IDC_BUTTON_TOACCOUNT);
	m_BtnToAccounts.SetTextMaxLen(0);

	m_BtnToAccounts.SetImage1(pimg[IDB_PNG_TOACCOUNT - IDB_PNG_CALC]);
	m_BtnToAccounts.SetShowImage1(true);
	m_BtnToAccounts.SetImageWidth1(pimg[IDB_PNG_ACCOUNT - IDB_PNG_CALC]->GetImageWidth());

	y += MZM_HEIGHT_BUTTONEX;
    m_BtnCategory.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnCategory.SetText(LOADSTRING(IDS_STR_CATEGORY).C_Str());
    m_BtnCategory.SetTextSize2(m_BtnDate.GetTextSize2() - 2);
    m_BtnCategory.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_ScrollWin.AddChild(&m_BtnCategory);
    m_BtnCategory.SetImage2(imgArrow);
    m_BtnCategory.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnCategory.SetShowImage2(true);
    m_BtnCategory.SetID(MZ_IDC_BUTTON_CATEGORY);
	m_BtnCategory.SetTextMaxLen(0);

	m_BtnCategory.SetImage1(pimg[IDB_PNG_CATEGORY - IDB_PNG_CALC]);
	m_BtnCategory.SetShowImage1(true);
	m_BtnCategory.SetImageWidth1(pimg[IDB_PNG_ACCOUNT - IDB_PNG_CALC]->GetImageWidth());

    y += MZM_HEIGHT_BUTTONEX;
    m_BtnPerson.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnPerson.SetText(LOADSTRING(IDS_STR_PERSON_TITLE).C_Str());
    m_BtnPerson.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_ScrollWin.AddChild(&m_BtnPerson);
    m_BtnPerson.SetImage2(imgArrow);
    m_BtnPerson.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnPerson.SetShowImage2(true);
    m_BtnPerson.SetID(MZ_IDC_BUTTON_PERSON);
	m_BtnPerson.SetTextMaxLen(0);

	m_BtnPerson.SetImage1(pimg[IDB_PNG_PERSON - IDB_PNG_BEGIN]);
	m_BtnPerson.SetShowImage1(true);
	m_BtnPerson.SetImageWidth1(pimg[IDB_PNG_PERSON - IDB_PNG_BEGIN]->GetImageWidth());

    y += MZM_HEIGHT_BUTTONEX;
    m_lblNote.SetPos(0, y, GetWidth(), MZM_HEIGHT_CAPTION);
    m_lblNote.SetText(LOADSTRING(IDS_STR_NOTE).C_Str());
    m_ScrollWin.AddChild(&m_lblNote);

    y += MZM_HEIGHT_CAPTION;
    m_EdtNote.SetPos(0, y, GetWidth(), GetHeight() - MZM_HEIGHT_CAPTION - MZM_HEIGHT_TEXT_TOOLBAR);
    m_EdtNote.EnableScrollBarV(true);
#ifdef USE_903SDK
	m_EdtNote.SetMaxChars(50);
#else
	m_EdtNote.SetMaxChar(50);
#endif
    m_EdtNote.SetSipMode(IM_SIP_MODE_GEL_PY);
    m_EdtNote.SetID(MZ_IDC_EDIT_NOTE);
    m_ScrollWin.AddChild(&m_EdtNote);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_MAIN);
    AddUiWin(&m_Toolbar);

	_selectedAccountIndex = appconfig.IniSelectedAccountIndex.Get();
	_selectedToAccountIndex = appconfig.IniSelectedToAccountIndex.Get();
	_selectedCategoryIndex = appconfig.IniSelectedCategoryIndex.Get();
	_selectedPersonIndex = appconfig.IniSelectedPersonIndex.Get();
	setupDUi();

	if(_isEditMode){
		initEditText();
	}
	m_EdtAmountYuan.SetEnable(false);

	//锁屏时上锁
	::RegisterShellMessage(m_hWnd,WM_MZSH_ENTRY_LOCKPHONE|WM_MZSH_LEAVE_LOCKPHONE);
	//解锁后cancel timer

    return TRUE;
}

void Ui_MainWnd::setupDUi(){
	if(_isEditMode){
		m_BtnAdd.SetImage_Normal(pimg[IDB_PNG_MODIFY - IDB_PNG_CALC]);
		m_BtnAdd.SetImage_Pressed(pimg[IDB_PNG_MODIFY - IDB_PNG_CALC]);
	}else{
		m_BtnAdd.SetImage_Normal(pimg[IDB_PNG_ADD - IDB_PNG_CALC]);
		m_BtnAdd.SetImage_Pressed(pimg[IDB_PNG_ADD - IDB_PNG_CALC]);
	}
	if(_isEditMode){
		m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_CANCEL).C_Str());
		m_Toolbar.SetButton(1, false, false, LOADSTRING(IDS_STR_CONFIG).C_Str());
		m_Toolbar.SetButton(2, false, false, LOADSTRING(IDS_STR_EXIT).C_Str());
	}else{
		m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_BROWSE).C_Str());
		m_Toolbar.SetButton(1, true, true, LOADSTRING(IDS_STR_CONFIG).C_Str());
		m_Toolbar.SetButton(2, true, true, LOADSTRING(IDS_STR_EXIT).C_Str());
	}
}

bool Ui_MainWnd::checkReminderOnOff(bool isUpdate, int recordId){
	bool ret;
#if 0
	SYSTEMTIME tm;
	swscanf(m_BtnDate.GetText2().C_Str(),L"%04d-%02d-%02d",&tm.wYear,&tm.wMonth,&tm.wDay);
	SYSTEMTIME now;
	GetLocalTime(&now);
	if(DateTime::compareDate(tm,now) == 1){ //未来时间
#endif
		ret = true;
		m_BtnReminder.SetVisible(true);
		if(isUpdate){
			ReminderInfo_ptr r = cash_reminder.getReminderByRecordId(recordId);
			if(r){
				reminderinfo.eventId = r->eventId;
				reminderinfo.rdtm = r->rdtm;
				reminderinfo.text = r->text;
				reminderinfo.type = r->type;
				_isReminderEnabled = true;
			}
		}
		if(_isReminderEnabled){
			m_BtnReminder.SetText2(LOADSTRING(IDS_STR_ENABLED).C_Str());
		}else{
			m_BtnReminder.SetText2(LOADSTRING(IDS_STR_DISABLED).C_Str());
		}
#if 0
	}else{
		ret = false;
		m_BtnReminder.SetVisible(false);
	}
#endif
	return ret;
}

void Ui_MainWnd::initEditText(){
	CASH_TRANSACT_t c;
	if(!cash_db.recordById(_editIndex,&c)){
		return;	//出错了
	}

	m_BtnAccounts.SetText2(cash_db.getAccountNameById(c.accountid));
	m_BtnToAccounts.SetText2(cash_db.getAccountNameById(c.toaccountid));
	m_BtnCategory.SetText2(cash_db.getCategoryFullNameById(c.categoryid));
	m_BtnPerson.SetText2(cash_db.getPersonNameById(c.personid));
	CMzString dateonly = c.date;
	m_BtnDate.SetText2(dateonly.SubStr(0,10).C_Str());
	m_EdtNote.SetText(c.note);
	wchar_t s[32];
	wsprintf(s, L"%d.%02d",c.amount/100,c.amount%100);
	m_EdtAmountYuan.SetText(s);

	_selectedAccountIndex = c.accountid;
	_selectedToAccountIndex = c.toaccountid;
	_selectedCategoryIndex = c.categoryid;
	_selectedPersonIndex = c.personid;

	//提醒
	bool ret = checkReminderOnOff(true,c.transid);

	//转账
	//determine whether display transfer account
	CASH_CATEGORY_ptr ct = cash_db.categoryById(_selectedCategoryIndex);
	if(ct->type == CT_TRANSFER){ //display
		m_BtnToAccounts.SetVisible(true);
		m_BtnToAccounts.SetPos(0,m_BtnAccounts.GetPosY() + MZM_HEIGHT_BUTTONEX,
			0, 0, SP_NOSIZE);
		m_BtnCategory.SetPos(0,m_BtnToAccounts.GetPosY() + MZM_HEIGHT_BUTTONEX,
			0, 0, SP_NOSIZE);
		m_BtnPerson.SetPos(0,m_BtnCategory.GetPosY() + MZM_HEIGHT_BUTTONEX,
			0, 0, SP_NOSIZE);
		m_lblNote.SetPos(0, m_BtnPerson.GetPosY() + MZM_HEIGHT_BUTTONEX, 
			0, 0, SP_NOSIZE);
		m_EdtNote.SetPos(0, m_lblNote.GetPosY() + MZM_HEIGHT_CAPTION, 
			0, 0, SP_NOSIZE);
	}else{
		m_BtnToAccounts.SetVisible(false);
		m_BtnCategory.SetPos(0,m_BtnAccounts.GetPosY() + MZM_HEIGHT_BUTTONEX,
			0, 0, SP_NOSIZE);
		m_BtnPerson.SetPos(0,m_BtnCategory.GetPosY() + MZM_HEIGHT_BUTTONEX,
			0, 0, SP_NOSIZE);
		m_lblNote.SetPos(0,m_BtnPerson.GetPosY() + MZM_HEIGHT_BUTTONEX,
			0, 0, SP_NOSIZE);
		m_EdtNote.SetPos(0, m_lblNote.GetPosY() + MZM_HEIGHT_CAPTION, 
			0, 0, SP_NOSIZE);
	}	
	m_BtnReminder.Invalidate();
	m_BtnToAccounts.Invalidate();
	m_BtnCategory.Invalidate();
	m_BtnAccounts.Invalidate();
	m_BtnPerson.Invalidate();
	m_lblNote.Invalidate();
	m_EdtNote.Invalidate();
}

void Ui_MainWnd::updateText(){
	//set account name
	int last_id = cash_db.getMaxAccountID();
	if(_selectedAccountIndex > last_id){
		_selectedAccountIndex = last_id;
		if(!_isEditMode) appconfig.IniSelectedAccountIndex.Set(_selectedAccountIndex);
	}
	m_BtnAccounts.SetText2(cash_db.getAccountNameById(_selectedAccountIndex));

	//set transfer distination account name
	if(_selectedToAccountIndex > last_id || (_selectedToAccountIndex == _selectedAccountIndex)){
		_selectedToAccountIndex = (_selectedAccountIndex == 0) ? (_selectedAccountIndex + 1) :(_selectedAccountIndex-1);
		if(!_isEditMode) appconfig.IniSelectedToAccountIndex.Set(_selectedToAccountIndex);
	}
	m_BtnToAccounts.SetText2(cash_db.getAccountNameById(_selectedToAccountIndex));

	//set category name
	last_id = cash_db.getMaxCategoryID();
	if(_selectedCategoryIndex > last_id){
		_selectedCategoryIndex = last_id;
		if(!_isEditMode) appconfig.IniSelectedCategoryIndex.Set(_selectedCategoryIndex);
	}
	m_BtnCategory.SetText2(cash_db.getCategoryFullNameById(_selectedCategoryIndex));
	
	//set person name
	last_id = cash_db.getMaxPersonID();
	if(_selectedPersonIndex > last_id){
		_selectedPersonIndex = last_id;
		if(!_isEditMode) appconfig.IniSelectedPersonIndex.Set(_selectedPersonIndex);
	}
	m_BtnPerson.SetText2(cash_db.getPersonNameById(_selectedPersonIndex));

	//提醒
	bool ret = checkReminderOnOff();
#if 0
	if(ret){
		m_BtnAccounts.SetPos(0,m_BtnReminder.GetPosY() + MZM_HEIGHT_BUTTONEX,0,0,SP_NOSIZE);
	}else{
		m_BtnAccounts.SetPos(0,m_BtnDate.GetPosY() + MZM_HEIGHT_BUTTONEX,0,0,SP_NOSIZE);
	}
#endif
	//转账
	//determine whether display transfer account
	CASH_CATEGORY_ptr c = cash_db.categoryById(_selectedCategoryIndex);
	if(c->type == CT_TRANSFER){ //display
		m_BtnToAccounts.SetVisible(true);
		m_BtnToAccounts.SetPos(0,m_BtnAccounts.GetPosY() + MZM_HEIGHT_BUTTONEX,
			0, 0, SP_NOSIZE);
		m_BtnCategory.SetPos(0,m_BtnToAccounts.GetPosY() + MZM_HEIGHT_BUTTONEX,
			0, 0, SP_NOSIZE);
		m_BtnPerson.SetPos(0,m_BtnCategory.GetPosY() + MZM_HEIGHT_BUTTONEX,
			0, 0, SP_NOSIZE);
		m_lblNote.SetPos(0, m_BtnPerson.GetPosY() + MZM_HEIGHT_BUTTONEX, 
			0, 0, SP_NOSIZE);
		m_EdtNote.SetPos(0, m_lblNote.GetPosY() + MZM_HEIGHT_CAPTION, 
			0, 0, SP_NOSIZE);
	}else{
		m_BtnToAccounts.SetVisible(false);
		m_BtnCategory.SetPos(0,m_BtnAccounts.GetPosY() + MZM_HEIGHT_BUTTONEX,
			0, 0, SP_NOSIZE);
		m_BtnPerson.SetPos(0,m_BtnCategory.GetPosY() + MZM_HEIGHT_BUTTONEX,
			0, 0, SP_NOSIZE);
		m_lblNote.SetPos(0,m_BtnPerson.GetPosY() + MZM_HEIGHT_BUTTONEX,
			0, 0, SP_NOSIZE);
		m_EdtNote.SetPos(0, m_lblNote.GetPosY() + MZM_HEIGHT_CAPTION, 
			0, 0, SP_NOSIZE);
	}	
	m_BtnReminder.Invalidate();
	m_BtnToAccounts.Invalidate();
	m_BtnCategory.Invalidate();
	m_BtnAccounts.Invalidate();
	m_lblNote.Invalidate();
	m_EdtNote.Invalidate();
}

LRESULT Ui_MainWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
	if(OnShellMessage(message,wParam,lParam)) return 0;
    switch (message) {
		case MZ_WM_UI_FOCUS:
		{
            int nID = LOWORD(wParam);
			if(nID == MZ_IDC_BUTTON_DATE || 
				nID == MZ_IDC_BUTTON_ACCOUNT ||
				nID == MZ_IDC_BUTTON_CATEGORY ||
				nID == MZ_IDC_BUTTON_ADD ||
				nID == MZ_IDC_BUTTON_TOACCOUNT){
					m_ScrollWin.ScrollTo(UI_SCROLLTO_TOP);
					MzCloseSip();
			}
			if(nID == MZ_IDC_EDIT_YUAN){
				m_ScrollWin.ScrollTo(UI_SCROLLTO_TOP);
			}
			if(nID == MZ_IDC_EDIT_NOTE) {
				m_ScrollWin.ScrollTo(UI_SCROLLTO_BOTTOM);
                return 0;
            }
		}
	}
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

void Ui_MainWnd::ShowBrowseDlg(){
	Ui_BrowseWnd dlg;
	RECT rcWork = MzGetWorkArea();
	dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
			m_hWnd, 0, WS_POPUP);
	// set the animation of the window
	dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_PUSH);
	dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_PUSH);
	dlg.setEditFormHandle(this);
	int nRet = dlg.DoModal();
	if (nRet == ID_OK) {
		//save status
		setEditMode(true);
		setupDUi();
		initEditText();
		Invalidate();
		UpdateWindow();
	}else{
		//restore status
		setEditMode(false);
		setupDUi();
		updateText();
		Invalidate();
		UpdateWindow();
	}
	if(m_EdtAmountYuan.IsEnable()){
		m_EdtAmountYuan.SetEnable(false);
	}
	//m_BtnCategory.SetFocus(true);
}

void Ui_MainWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    int nRet;
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
		case MZ_IDC_BUTTON_CALCULATOR:
			m_EdtAmountYuan.ToggleCalc();
			break;
		case MZ_IDC_BUTTON_REMINDER:
		{
			Ui_CashReminderWnd dlg;
            RECT rcWork = MzGetWorkArea();
			dlg.setEnable(_isReminderEnabled);
			dlg.setupReminderInfo(&reminderinfo);
            dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
                    m_hWnd, 0, WS_POPUP);
            // set the animation of the window
            dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
            dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
            nRet = dlg.DoModal();
            if (nRet == ID_OK) {
				//...
				_isReminderEnabled = dlg.isEnabled();
            }
			updateText();
			m_BtnReminder.SetFocus(true);
			break;
		}
		case MZ_IDC_BUTTON_DATE:
		{
			Ui_CalendarWnd calendardlg;
            RECT rcWork = MzGetWorkArea();
			if(!m_BtnDate.GetText2().IsEmpty()){
				int year,month,day;
				swscanf(m_BtnDate.GetText2().C_Str(),L"%04d-%02d-%02d",&year,&month,&day);
				calendardlg.initDate(year,month,day);
			}
            calendardlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
                    m_hWnd, 0, WS_POPUP);
            // set the animation of the window
            calendardlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
            calendardlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
            nRet = calendardlg.DoModal();
            if (nRet == ID_OK) {
				m_BtnDate.SetText2(calendardlg.getDate().C_Str());
            }
			updateText();
			m_BtnDate.SetFocus(true);
			break;
		}
		case MZ_IDC_BUTTON_TOACCOUNT:
		{
			Ui_AccountsWnd accountdlg;
            RECT rcWork = MzGetWorkArea();
			//accountdlg.setHideAccount(cash_db.accountById(_selectedAccountIndex)->id);
			accountdlg.setHideAccount(-1);
            accountdlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
                    m_hWnd, 0, WS_POPUP);
            // set the animation of the window
            accountdlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
            accountdlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
            nRet = accountdlg.DoModal();
            if (nRet == ID_OK) {
                _selectedToAccountIndex = accountdlg.getSelectionIndex();
				if(!_isEditMode) appconfig.IniSelectedToAccountIndex.Set(_selectedToAccountIndex);
                m_BtnToAccounts.SetText2(cash_db.getAccountNameById(_selectedToAccountIndex));
            }
			m_BtnToAccounts.SetFocus(true);
			break;
		}
        case MZ_IDC_BUTTON_ACCOUNT:
		{
			Ui_AccountsWnd accountdlg;
            RECT rcWork = MzGetWorkArea();
			accountdlg.setHideAccount(-1);
#if 0
			if(cash_db.categoryById(_selectedCategoryIndex)->type == CT_TRANSFER){
				accountdlg.setHideAccount(cash_db.accountById(_selectedToAccountIndex)->id);
			}else{
				accountdlg.setHideAccount(-1);
			}
#endif
            accountdlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
                    m_hWnd, 0, WS_POPUP);
            // set the animation of the window
            accountdlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_BOTTOM_TO_TOP_2);
            accountdlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_TOP_TO_BOTTOM_1);
            nRet = accountdlg.DoModal();
            if (nRet == ID_OK) {
                _selectedAccountIndex = accountdlg.getSelectionIndex();
				if(!_isEditMode) appconfig.IniSelectedAccountIndex.Set(_selectedAccountIndex);
                m_BtnAccounts.SetText2(cash_db.getAccountNameById(_selectedAccountIndex));
            }
			m_BtnAccounts.SetFocus(true);
            break;
		}
        case MZ_IDC_BUTTON_CATEGORY:
		{
			Ui_CategoriesWnd catdlg;
            RECT rcWork = MzGetWorkArea();
			catdlg.setNodeDepth(2);
			catdlg.setHideCategory(-1);
            catdlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
                    m_hWnd, 0, WS_POPUP);
            // set the animation of the window
            catdlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_BOTTOM_TO_TOP_2);
            catdlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_TOP_TO_BOTTOM_1);
            nRet = catdlg.DoModal();
            if (nRet == ID_OK) {
                _selectedCategoryIndex = catdlg.getSelectionIndex();
				updateText();
				if(!_isEditMode) appconfig.IniSelectedCategoryIndex.Set(_selectedCategoryIndex);
            }
			m_BtnCategory.SetFocus(true);
            break;
		}
		case MZ_IDC_BUTTON_PERSON:
		{
			Ui_PersonsWnd dlg;
            RECT rcWork = MzGetWorkArea();
            dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
                    m_hWnd, 0, WS_POPUP);
            // set the animation of the window
            dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_BOTTOM_TO_TOP_2);
            dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_TOP_TO_BOTTOM_1);
            nRet = dlg.DoModal();
            if (nRet == ID_OK) {
                _selectedPersonIndex = dlg.getSelectionIndex();
				updateText();
				if(!_isEditMode) appconfig.IniSelectedPersonIndex.Set(_selectedPersonIndex);
            }
			m_BtnPerson.SetFocus(true);
			break;
		}
		case MZ_IDC_BUTTON_ADD:
		{
			if (_isEditMode) { //更新
				if(m_EdtAmountYuan.isEmpty()){
					MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_ERR_TRANS_AMOUNT).C_Str());
					return;
				}
				CASH_TRANSACT_t c;
				if(!cash_db.recordById(_editIndex, &c)) return;

				c.amount = m_EdtAmountYuan.getInputAmount();
				if(c.amount < 0){
					MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_ERR_TRANS_MINUS).C_Str());
					return;
				}
				c.accountid = cash_db.accountById(_selectedAccountIndex)->id;
				c.categoryid = cash_db.categoryById(_selectedCategoryIndex)->id;
				if(!m_EdtNote.GetText().IsEmpty()){
					C::newstrcpy(&c.note,m_EdtNote.GetText().C_Str());
				}else{
					c.note = L"\0";
				}
				if(cash_db.categoryById(_selectedCategoryIndex)->type == CT_TRANSFER){
					if(_selectedAccountIndex == _selectedToAccountIndex){
						MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_ERR_TRANSFER_ACCOUNT).C_Str());
						m_BtnAccounts.SetFocus(true);
						return;
					}
					c.isTransfer = 1;
					c.toaccountid = cash_db.accountById(_selectedToAccountIndex)->id;
				}else{
					c.isTransfer = 0;
					c.toaccountid = -1;
				}
				c.personid = _selectedPersonIndex;
				CMzString sdate = m_BtnDate.GetText2();
				sdate = sdate + L" "; 
				wchar_t stime[16];
				SYSTEMTIME now;
				GetLocalTime(&now);
				wsprintf(stime,L"%02d:%02d:%02d",now.wHour,now.wMinute,now.wSecond);
				sdate = sdate + stime;
				C::newstrcpy(&c.date,sdate.C_Str());
				cash_db.updateTransaction(&c);
				//提醒
				if(_isReminderEnabled){
					_isReminderEnabled = false;
					reminderinfo.recordId = c.transid;
					wchar_t remindertext[64];
					if(c.isTransfer){	//转账
						wsprintf(remindertext,L"%s→%s %.2f::%s\\n",
							cash_db.accountById(c.accountid)->name,
							cash_db.accountById(c.toaccountid)->name,
							(double)c.amount/100,
							cash_db.categoryById(c.categoryid)->name);
					}else{
						wsprintf(remindertext,L"%s::%.2f::%s\\n",
							cash_db.accountById(c.accountid)->name,
							(double)c.amount/100,
							cash_db.categoryById(c.categoryid)->name);
					}
					reminderinfo.text = remindertext;
					if(c.note){
						reminderinfo.text = reminderinfo.text + c.note;
					}
					if(!cash_reminder.refreshReminder(&reminderinfo)){
						cash_reminder.setNextReminder(&reminderinfo);
					}
					cash_reminder.saveReminderList();
					reminderinfo.reset();
					m_BtnReminder.Invalidate();
				}else{	//查找是否是取消动作
					ReminderInfo_ptr r = cash_reminder.getReminderByRecordId(c.transid);
					if(r){
						cash_reminder.deleteReminder(r);
						cash_reminder.saveReminderList();
					}
				}
				MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_TRANS_UPDATE).C_Str(),1000);
				m_EdtAmountYuan.SetText(L"\0");
				m_EdtAmountYuan.SetEnable(false);
				m_EdtNote.Clear();
				m_EdtAmountYuan.Invalidate();
				m_EdtNote.Invalidate();
				ShowBrowseDlg(); 
				return;
			}else{ //保存
				if(m_EdtAmountYuan.isEmpty()){
					MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_ERR_TRANS_AMOUNT).C_Str());
					return;
				}
				CASH_TRANSACT_t c;
				c.amount = m_EdtAmountYuan.getInputAmount();
				if(c.amount < 0){
					MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_ERR_TRANS_MINUS).C_Str());
					return;
				}
				c.accountid = cash_db.accountById(_selectedAccountIndex)->id;
				c.categoryid = cash_db.categoryById(_selectedCategoryIndex)->id;
				if(!m_EdtNote.GetText().IsEmpty()){
					C::newstrcpy(&c.note,m_EdtNote.GetText().C_Str());
				}else{
					c.note = L"\0";
				}
				if(cash_db.categoryById(_selectedCategoryIndex)->type == CT_TRANSFER){
					if(_selectedAccountIndex == _selectedToAccountIndex){
						MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_ERR_TRANSFER_ACCOUNT).C_Str());
						m_BtnAccounts.SetFocus(true);
						return;
					}
					c.isTransfer = 1;
					c.toaccountid = cash_db.accountById(_selectedToAccountIndex)->id;
				}else{
					c.isTransfer = 0;
					c.toaccountid = -1;
				}
				c.personid = _selectedPersonIndex;
				CMzString sdate = m_BtnDate.GetText2();
				sdate = sdate + L" "; 
				wchar_t stime[16];
				SYSTEMTIME now;
				GetLocalTime(&now);
				wsprintf(stime,L"%02d:%02d:%02d",now.wHour,now.wMinute,now.wSecond);
				sdate = sdate + stime;
				c.date = sdate.C_Str();
				cash_db.appendTransaction(&c);
				//提醒
				if(_isReminderEnabled){
					_isReminderEnabled = false;
					reminderinfo.recordId = c.transid;
					wchar_t remindertext[1024];
					if(c.isTransfer){	//转账
						wsprintf(remindertext,L"%s→%s %.2f::%s\\n",
							cash_db.accountById(c.accountid)->name,
							cash_db.accountById(c.toaccountid)->name,
							(double)c.amount/100,
							cash_db.categoryById(c.categoryid)->name);
					}else{
						wsprintf(remindertext,L"%s::%.2f::%s\\n",
							cash_db.accountById(c.accountid)->name,
							(double)c.amount/100,
							cash_db.categoryById(c.categoryid)->name);
					}
					reminderinfo.text = remindertext;
					if(c.note){
						C::removeWrap(remindertext,c.note);
						reminderinfo.text = reminderinfo.text + remindertext;
					}
					if(!cash_reminder.refreshReminder(&reminderinfo)){
						cash_reminder.setNextReminder(&reminderinfo);
					}
					cash_reminder.saveReminderList();
					reminderinfo.reset();
					m_BtnReminder.Invalidate();
				}
				//保存完毕后清理
				MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_TRANS_ADD).C_Str(),1000);
				m_EdtAmountYuan.SetText(L"\0");
				m_EdtAmountYuan.SetEnable(false);
				m_EdtNote.Clear();
				m_EdtAmountYuan.Invalidate();
				m_EdtNote.Invalidate();
				return;
            }
			break;
		}
        case MZ_IDC_TOOLBAR_MAIN:
        {
            int nIndex = lParam;
            if (nIndex == 2) {
				if(MzMessageBoxEx(m_hWnd,
					LOADSTRING(IDS_STR_CONFIRM_EXIT).C_Str(),
					LOADSTRING(IDS_STR_OK).C_Str(),MZ_YESNO,false) == 1){
					PostQuitMessage(0);
				}
                return;
            }

			if (nIndex == 0) {	//return
				m_EdtAmountYuan.SetText(L"\0");
				m_EdtNote.Clear();
				m_EdtAmountYuan.Invalidate();
				m_EdtNote.Invalidate();
				ShowBrowseDlg();
				return;
			}
            if (nIndex == 1) { //设置
                Ui_ConfigWnd dlg;
                RECT rcWork = MzGetWorkArea();
                dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
                        m_hWnd, 0, WS_POPUP);
                // set the animation of the window
                dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_BOTTOM_TO_TOP_2);
                dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_TOP_TO_BOTTOM_1);
                int nRet = dlg.DoModal();
                if (nRet == ID_OK) {
                    updateText();
                }
				m_BtnCategory.SetFocus(true);
                return;
            }
        }
            break;
    }
}

void Ui_MainWnd::OnTimer(UINT nIDEvent){
	switch(nIDEvent){
		case 0x1001:
		{
			::KillTimer(m_hWnd,nIDEvent);	//stop timer
			if(!_readyAutolock) return;
			_readyAutolock = false;
			cash_db.disconnect();
			if(pdlgpwd == 0){
				pdlgpwd = new Ui_PasswordWnd;
				RECT rcWork = MzGetWorkArea();
				pdlgpwd->setMode(0);
				pdlgpwd->Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
					m_hWnd, 0, WS_POPUP);
			}
			//::MzLeaveLockPhone();
			g_pwddlgshow = true;
			pdlgpwd->Show(true);
			//::MzEntryLockPhone();
			return;
		}
	}
}

bool Ui_MainWnd::OnShellMessage(UINT message, WPARAM wParam, LPARAM lParam){
	if(message == ::GetShellNotifyMsg_EntryLockPhone()){
		DWORD autolock = appconfig.IniAutolock.Get();
		WORD ltype = LOWORD(autolock);
		if(g_bencypt && ltype != 0 && !g_pwddlgshow){
			WORD ltimeout = HIWORD(autolock);
			_readyAutolock = true;
			if(ltimeout <= 1){
				::SetTimer(m_hWnd,0x1001,100,NULL);
			}else{
				::SetTimer(m_hWnd,0x1001,ltimeout*1000,NULL);
			}
			return true;
		}
	}
	if(message == ::GetShellNotifyMsg_LeaveLockPhone()){
		if(_readyAutolock) _readyAutolock = false;
		return true;
	}
	return false;
}