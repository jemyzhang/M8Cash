#include "ui_reminder.h"
#include <notify.h>
#include "ReadWriteIni.h"
#include "m8cash.h"
#include "ui_calendar.h" //选择日期

#include <MzCommon.h>
using namespace MzCommon;

MZ_IMPLEMENT_DYNAMIC(Ui_CashReminderWnd)

#define MZ_IDC_TOOLBAR_REMINDER 101
#define MZ_IDC_Edit_HOUR 102
#define MZ_IDC_Edit_MIN 103
#define MZ_IDC_BTN_REPEAT 104
#define MZ_IDC_LIST_TYPE 105
#define MZ_IDC_BTN_REMINDER 106
#define MZ_IDC_S_DATE	107

#ifdef _DEBUG
#define DEFAULT_REMINDER_INI		L"Program Files\\M8Cash\\reminder.ini"
#else
#define DEFAULT_REMINDER_INI		L"\\Disk\\Programs\\M8Cash\\reminder.ini"
#endif

CashReminder::CashReminder()
{
	bool ret;
	//默认提醒配置文件路径
	wchar_t currpath[MAX_PATH];
	if(File::GetCurrentPath(currpath)){
		wsprintf(ini_reminder,L"%sreminder.ini",currpath);
	}else{
		wsprintf(ini_reminder,DEFAULT_REMINDER_INI);
	}
	//创建配置文件
	if(!File::FileExists(ini_reminder)){
		ret = IniCreateFile(ini_reminder);
	}
}

CashReminder::~CashReminder(){
}

HANDLE CashReminder::Notify(SYSTEMTIME st, HANDLE hNotification)
{                
    TCHAR szExeName[MAX_PATH];  //需要运行的程序
    TCHAR szSound[MAX_PATH];    //需要播放的声音
    TCHAR Title[128]=L"标题";   //对话框标题，Ｍ８上没用
    TCHAR Text[128]=L"内容";    //对话框内容，Ｍ８上没用

    //取自身文件名
    ::GetModuleFileNameW(NULL,szExeName, MAX_PATH); 
    //同名音乐
    lstrcpy(szSound,szExeName);
    lstrcpy(szSound+ lstrlen(szSound) - 4, TEXT(".no"));//正常应该是．ｗａｖ，这里故意搞个不存在的文件，取消系统提醒声音，自己启动后根据情况播放。
            
    CE_USER_NOTIFICATION g_ceun; 
    memset (&g_ceun,0,sizeof(g_ceun));
    g_ceun.ActionFlags=(PUN_VIBRATE);
    g_ceun.pwszDialogTitle=Title;
    g_ceun.pwszDialogText=Text;
    g_ceun.pwszSound=szSound; 
    g_ceun.nMaxSound=sizeof(szSound);

    return CeSetUserNotification(hNotification,szExeName,&st,&g_ceun);
}

void CashReminder::loadReminderList(){
	int size = 0;
	if(IniReadInt(L"ReminderInfo",L"nSize",(DWORD*)&size,ini_reminder) && ( size > 0)){
		int cnt = 0;
		for(int i = 0; i < size; i++){
			ReminderInfo_ptr r = new ReminderInfo_t;
			wchar_t header[16];
			wsprintf(header,L"Reminder%d",cnt+1);
			IniReadInt(header,L"EventID",(DWORD*)&r->eventId,ini_reminder);
			IniReadInt(header,L"RecordID",(DWORD*)&r->recordId,ini_reminder);
			IniReadInt(header,L"RepeatType",(DWORD*)&r->type,ini_reminder);
			wchar_t* pdt;
			IniReadString(header,L"EventText",&pdt,ini_reminder);
			r->text = pdt;
			delete [] pdt;
			IniReadString(header,L"DateTime",&pdt,ini_reminder);
			swscanf(pdt,L"%04d%02d%02d%02d%02d",
			&r->rdtm.wYear,&r->rdtm.wMonth,&r->rdtm.wDay,
			&r->rdtm.wHour,&r->rdtm.wMinute);
			delete[] pdt;
			list_reminder.push_back(r);
			cnt++;
		}
	}
}

void CashReminder::saveReminderList(){
	File::DelFile(ini_reminder);	//删除旧文件
	IniCreateFile(ini_reminder);
	//写入配置大小
	IniWriteInt(L"ReminderInfo",L"nSize",list_reminder.size(),ini_reminder);
	//写入配置
	list<ReminderInfo_ptr>::iterator i = list_reminder.begin();
	int cnt = 0;
	for(;i != list_reminder.end(); i++){
		ReminderInfo_ptr r = *i;
		wchar_t header[16];
		wsprintf(header,L"Reminder%d",cnt+1);
		IniWriteInt(header,L"EventID",r->eventId,ini_reminder);
		IniWriteInt(header,L"RecordID",r->recordId,ini_reminder);
		IniWriteInt(header,L"RepeatType",r->type,ini_reminder);
		wchar_t datetime[16];
		wsprintf(datetime,L"%04d%02d%02d%02d%02d",
			r->rdtm.wYear,r->rdtm.wMonth,r->rdtm.wDay,
			r->rdtm.wHour,r->rdtm.wMinute);
		IniWriteString(header,L"DateTime",datetime,ini_reminder);
		IniWriteString(header,L"EventText",r->text,ini_reminder);
		cnt++;
	}
}

bool CashReminder::refreshReminder(ReminderInfo_ptr r){
	SYSTEMTIME now;
	GetLocalTime(&now);
	if(DateTime::compareDate(r->rdtm,now) != 1){ //已过去的时间
		return false;
	}

	if(r->eventId){	//已存在
		ReminderInfo_ptr oldr = getReminderByEventId(r->eventId);
		if(oldr != r){
			oldr->eventId = r->eventId;
			oldr->rdtm = r->rdtm;
			oldr->recordId = r->recordId;
			oldr->type = r->type;
			oldr->text = r->text;
		}
		oldr->eventId = (unsigned int)Notify(r->rdtm,(HANDLE)r->eventId);
		return true;
	}
	r->eventId = (unsigned int)Notify(r->rdtm);
	if(r->eventId){
		ReminderInfo_ptr pr = new ReminderInfo_t;
		pr->eventId = r->eventId;
		pr->rdtm = r->rdtm;
		pr->recordId = r->recordId;
		pr->type = r->type;
		pr->text = r->text;
		list_reminder.push_back(pr);
		return true;
	}else{
		return false;
	}
}

bool CashReminder::setNextReminder(ReminderInfo_ptr r){
	bool ret = false;
	if(r == 0) return ret;

	if(r->eventId){
		deleteReminder(r);
	}
	r->eventId = 0;

	SYSTEMTIME now;
	GetLocalTime(&now);
	r->rdtm.wYear = now.wYear;
	r->rdtm.wMonth = now.wMonth;
	r->rdtm.wDay = now.wDay;
	switch(r->type){
		case REPEAT_DAILY:
			DateTime::OneDayDate(r->rdtm);
			break;
		case REPEAT_WEEKLY:
			for(int i = 0; i < 7; i++){
				DateTime::OneDayDate(r->rdtm);
			}
			break;
		case REPEAT_MONTHLY:
			int y,m;
			DateTime::getNextDate(y,m);
			r->rdtm.wYear = y;
			r->rdtm.wMonth = m;
			break;
		case REPEAT_YEARLY:
			r->rdtm.wYear++;
			break;
		case REPEAT_NONE:
		default:
			return false;
	}
	ret = refreshReminder(r);
	return ret;
}

ReminderInfo_ptr CashReminder::deleteReminder(ReminderInfo_ptr r){
	ReminderInfo_ptr r_evnt;
	if(r->eventId){
		r_evnt = getReminderByEventId(r->eventId);
		CeClearUserNotification((HANDLE)r->eventId);
	}else{
		r_evnt = getReminderByEventId(r->recordId);
	}
	if(r_evnt){
		list_reminder.remove(r_evnt);
	}
	return r_evnt;
}

void CashReminder::clearReminder(){
	list<ReminderInfo_ptr>::iterator i = list_reminder.begin();
	for(; i != list_reminder.end(); i++){
		ReminderInfo_ptr r = *i;
		deleteReminder(r);
	}
}

void CashReminder::recoverReminder(){
	list<ReminderInfo_ptr>::iterator i = list_reminder.begin();
	for(; i != list_reminder.end(); i++){
		ReminderInfo_ptr r = *i;
		if(!refreshReminder(r)){
			setNextReminder(r);
		}
	}
}

ReminderInfo_ptr CashReminder::getReminderByEventId(int id){
	list<ReminderInfo_ptr>::iterator i = list_reminder.begin();
	for(; i != list_reminder.end(); i++){
		ReminderInfo_ptr r = *i;
		if(r->eventId == id){
			return r;
		}
	}
	return 0;
}

ReminderInfo_ptr CashReminder::getReminderByRecordId(int id){
	list<ReminderInfo_ptr>::iterator i = list_reminder.begin();
	for(; i != list_reminder.end(); i++){
		ReminderInfo_ptr r = *i;
		if(r->recordId == id){
			return r;
		}
	}
	return 0;
}

//////

Ui_CashReminderWnd::Ui_CashReminderWnd(void)
{
	r = 0;
}

Ui_CashReminderWnd::~Ui_CashReminderWnd(void)
{
}

void Ui_CashReminderWnd::updateUi(){
    wchar_t s[16];
	if(m_EdtHour.GetText().Length() == 0){
		wsprintf(s, L"%02d", r->rdtm.wHour);
		m_EdtHour.SetText(s);
	}
	if(m_EdtMin.GetText().Length() == 0){
		wsprintf(s, L"%02d", r->rdtm.wMinute);
		m_EdtMin.SetText(s);
	}
	if(r->rdtm.wYear == 0 || 
		r->rdtm.wMonth == 0 ||
		r->rdtm.wDay == 0){
			r->reset();
	}
	wsprintf(s,LOADSTRING(IDS_STR_YYYYMMDD).C_Str(),
		r->rdtm.wYear,r->rdtm.wMonth,r->rdtm.wDay);
	m_StaticDate.SetText(s);

	if(m_BtnReminderSW.GetState()){
		m_BtnReminder.SetText2(LOADSTRING(IDS_STR_ENABLED).C_Str());
		m_BtnRepeat.SetVisible(true);
		m_BtnRepeatSW.SetVisible(true);
		m_EdtMin.SetVisible(true);
		m_EdtHour.SetVisible(true);
		m_StaticDate.SetVisible(true);
		if(r->type == REPEAT_NONE){
			m_BtnRepeatSW.SetState(MZCS_BUTTON_NORMAL);
			m_BtnRepeat.SetText2(LOADSTRING(IDS_STR_DISABLED).C_Str());
			m_ListRepeatType.SetVisible(false);
		}else{
			m_BtnRepeatSW.SetState(MZCS_BUTTON_PRESSED);
			m_BtnRepeat.SetText2(LOADSTRING(IDS_STR_ENABLED).C_Str());
			m_ListRepeatType.SetVisible(true);
			m_ListRepeatType.SetSelectedIndex((r->type - 1));
		}
	}else{
		m_BtnReminder.SetText2(LOADSTRING(IDS_STR_DISABLED).C_Str());
		m_BtnRepeat.SetVisible(false);
		m_BtnRepeatSW.SetVisible(false);
		m_ListRepeatType.SetVisible(false);
		m_EdtMin.SetVisible(false);
		m_EdtHour.SetVisible(false);
		m_StaticDate.SetVisible(false);
	}
	m_BtnReminder.Invalidate();
	m_EdtHour.Invalidate();
	m_EdtMin.Invalidate();
	m_BtnRepeat.Invalidate();
	m_BtnRepeatSW.Invalidate();
	m_ListRepeatType.Invalidate();
	m_StaticDate.Invalidate();
}

bool Ui_CashReminderWnd::checkDateText(){
	bool ret = true;
	int hour = _wtoi(m_EdtHour.GetText().C_Str());
	int min = _wtoi(m_EdtMin.GetText().C_Str());

	if(hour > 23 || hour < 0) {
		m_EdtHour.SetTextColor(RGB(255,0,0));
		ret = false;
	}else{
		m_EdtHour.SetTextColor(RGB(0,0,0));
	}
	if(min > 59 || min < 0) {
		m_EdtMin.SetTextColor(RGB(255,0,0));
		ret = false;
	}else{
		m_EdtMin.SetTextColor(RGB(0,0,0));
	}

	m_EdtHour.Invalidate();
	m_EdtMin.Invalidate();
	return ret;
}

UINT REPEATSTRID[] = {
	IDS_STR_EVERY_DAY,
	IDS_STR_EVERY_WEEK,
	IDS_STR_EVERY_MONTH,
	IDS_STR_EVERY_YEAR,
};

BOOL Ui_CashReminderWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 0;
	m_BtnReminder.SetPos(0,y, GetWidth() - 120, MZM_HEIGHT_BUTTONEX);
	m_BtnReminder.SetText(LOADSTRING(IDS_STR_ALARM).C_Str());
    m_BtnReminder.SetButtonType(MZC_BUTTON_LINE_NONE);
	m_BtnReminder.SetTextMaxLen(0);
	m_BtnReminder.SetEnable(false);
    AddUiWin(&m_BtnReminder);

	m_BtnReminderSW.SetPos(GetWidth() - 120,y, 120, MZM_HEIGHT_BUTTONEX);
    m_BtnReminderSW.SetButtonType(MZC_BUTTON_SWITCH);
	m_BtnReminderSW.SetButtonMode(MZC_BUTTON_MODE_HOLD);
	m_BtnReminderSW.SetID(MZ_IDC_BTN_REMINDER);
    AddUiWin(&m_BtnReminderSW);

	y+=MZM_HEIGHT_BUTTONEX;
	int x = 0;
    m_StaticDate.SetPos(x, y, GetWidth()/2, MZM_HEIGHT_SINGLELINE_EDIT);
	m_StaticDate.SetID(MZ_IDC_S_DATE);
	m_StaticDate.SetButtonType(MZC_BUTTON_RECT_NOFILL);
	m_StaticDate.SetTextColor(RGB(0,0,0));
    AddUiWin(&m_StaticDate);

	x += GetWidth()/2;
	m_EdtHour.SetPos(x, y, GetWidth()/4, MZM_HEIGHT_SINGLELINE_EDIT);
    m_EdtHour.SetMaxChars(2);
    m_EdtHour.SetSipMode(IM_SIP_MODE_DIGIT, MZM_HEIGHT_TEXT_TOOLBAR);
    m_EdtHour.SetTip2(LOADSTRING(IDS_STR_HOUR).C_Str(),true);
    m_EdtHour.SetRightInvalid(80);
	m_EdtHour.SetID(MZ_IDC_Edit_HOUR);
	m_EdtHour.EnableNotifyMessage(true);
    AddUiWin(&m_EdtHour);

	x += GetWidth()/4;
    m_EdtMin.SetPos(x, y, GetWidth()/4, MZM_HEIGHT_SINGLELINE_EDIT);
    m_EdtMin.SetMaxChars(2);
    m_EdtMin.SetSipMode(IM_SIP_MODE_DIGIT, MZM_HEIGHT_TEXT_TOOLBAR);
    m_EdtMin.SetTip2(LOADSTRING(IDS_STR_MIN).C_Str(),true);
    m_EdtMin.SetRightInvalid(80);
	m_EdtMin.SetID(MZ_IDC_Edit_MIN);
	m_EdtMin.EnableNotifyMessage(true);
    AddUiWin(&m_EdtMin);

	y+=MZM_HEIGHT_SINGLELINE_EDIT;
	m_BtnRepeat.SetPos(0,y,GetWidth() - 120,MZM_HEIGHT_BUTTONEX);
    m_BtnRepeat.SetButtonType(MZC_BUTTON_LINE_NONE);
	m_BtnRepeat.SetText(LOADSTRING(IDS_STR_CYCLE).C_Str());
	m_BtnRepeat.SetEnable(false);
	AddUiWin(&m_BtnRepeat);

	m_BtnRepeatSW.SetPos(GetWidth() - 120,y, 120, MZM_HEIGHT_BUTTONEX);
    m_BtnRepeatSW.SetButtonType(MZC_BUTTON_SWITCH);
	m_BtnRepeatSW.SetButtonMode(MZC_BUTTON_MODE_HOLD);
	m_BtnRepeatSW.SetID(MZ_IDC_BTN_REPEAT);
    AddUiWin(&m_BtnRepeatSW);

	y+=MZM_HEIGHT_BUTTONEX;
	m_ListRepeatType.SetPos(0,y,GetWidth(),GetHeight() - y - MZM_HEIGHT_TEXT_TOOLBAR);
	for(int i = 0; i < 4; i++){
		CMzString str = LOADSTRING(REPEATSTRID[i]);
		ListItem li;
		li.Text = str;
		m_ListRepeatType.AddItem(li);
	}
	m_ListRepeatType.SetID(MZ_IDC_LIST_TYPE);
	m_ListRepeatType.EnableNotifyMessage(true);
	AddUiWin(&m_ListRepeatType);
	m_ListRepeatType.SetVisible(false);

	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_OK).C_Str());
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_REMINDER);
    AddUiWin(&m_Toolbar);
	updateUi();
    return TRUE;
}

void Ui_CashReminderWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
		case MZ_IDC_S_DATE:
		{
			Ui_CalendarWnd calendardlg;
            RECT rcWork = MzGetWorkArea();
            calendardlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
                    m_hWnd, 0, WS_POPUP);
            // set the animation of the window
            calendardlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
            calendardlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
            int nRet = calendardlg.DoModal();
            if (nRet == ID_OK) {
				int y,m,d;
				swscanf(calendardlg.getDate().C_Str(),L"%04d-%02d-%02d",&y,&m,&d);
				r->rdtm.wYear = y;
				r->rdtm.wMonth = m;
				r->rdtm.wDay = d;
				updateUi();
            }
			break;
		}
		case MZ_IDC_BTN_REMINDER:
		{
			updateUi();
			break;
		}
		case MZ_IDC_BTN_REPEAT:
			if(m_BtnRepeatSW.GetState() == MZCS_BUTTON_NORMAL){
				r->type = REPEAT_NONE;
			}else{
				r->type = REPEAT_MONTHLY;
			}
			m_BtnRepeat.SetFocus(true);
			updateUi();
			break;
        case MZ_IDC_TOOLBAR_REMINDER:
        {
            int nIndex = lParam;
			if(nIndex == 0){	//确定
				//更新
				if(checkDateText()){
					//cash_reminder.flushReminder(r);
					r->rdtm.wHour = _wtoi(m_EdtHour.GetText().C_Str());
					r->rdtm.wMinute = _wtoi(m_EdtMin.GetText().C_Str());
					if(m_BtnRepeatSW.GetState() == MZCS_BUTTON_PRESSED){
						r->type = (REPEAT_t)(m_ListRepeatType.GetSelectedIndex() + 1);
					}else{
						r->type = REPEAT_NONE;
					}
					EndModal(ID_OK);
				}
				return;
			}
		}
	}
}

LRESULT Ui_CashReminderWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
		case MZ_WM_MOUSE_NOTIFY:
		{
			int nID = LOWORD(wParam);
			int nNotify = HIWORD(wParam);
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            if (nID == MZ_IDC_LIST_TYPE && nNotify == MZ_MN_LBUTTONDOWN) {
                if (!m_ListRepeatType.IsMouseDownAtScrolling() && !m_ListRepeatType.IsMouseMoved()) {
					int nIndex = m_ListRepeatType.CalcIndexOfPos(x,y);
					if(nIndex != -1){
						m_ListRepeatType.SetSelectedIndex(nIndex);
						m_ListRepeatType.Invalidate();
					}
				}
                return 0;
            }
			break;
		}
		case MZ_WM_UI_FOCUS:
		{
            int nID = LOWORD(wParam);
            if (nID == MZ_IDC_Edit_MIN || nID == MZ_IDC_Edit_HOUR) {
				checkDateText();
                return 0;
            }
		}
    }
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}
