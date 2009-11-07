#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include <list>

typedef enum RepeatType{
	REPEAT_NONE		=	0,
	REPEAT_YEARLY	=	4,
	REPEAT_MONTHLY	=	3,
	REPEAT_WEEKLY	=	2,
	REPEAT_DAILY	=	1,
}REPEAT_t;

typedef struct ReminderInfo{
	ReminderInfo(unsigned int eid = 0, unsigned int rid = 0,
		WORD y = 0,WORD m = 0, WORD d = 0, WORD h = 12, WORD mm = 0,
		REPEAT_t t = REPEAT_NONE, wchar_t* txt = L"无提醒文本"){
		eventId = eid; recordId = rid;
		rdtm.wYear = y;	rdtm.wMonth = m;
		rdtm.wHour = h; rdtm.wMinute = mm;	rdtm.wSecond = 0;
		type = t;
		text = txt;
	}
	void reset(){
		eventId = 0; recordId = 0;
		SYSTEMTIME now;
		GetLocalTime(&now);
		rdtm.wYear = now.wYear;
		rdtm.wMonth = now.wMonth;
		rdtm.wDay = now.wDay;
		rdtm.wHour = 12; rdtm.wMinute = 0;	rdtm.wSecond = 0;
		type = REPEAT_NONE;
		text = L"无提醒文本";
	}
	int eventId;	//event id(handle)
	int recordId;	//record id
	SYSTEMTIME rdtm;	//reminder date time
	REPEAT_t type;
	CMzString text;		//reminder text
}ReminderInfo_t,*ReminderInfo_ptr;

//提醒读写类
class CashReminder
{
public:
	CashReminder(void);
	~CashReminder(void);
public:
	HANDLE Notify(SYSTEMTIME st, HANDLE hNotification = 0);
public:
	//返回reminder数量
	void setReminderIniPath(wchar_t* p){
		rinipath = p;
	}
	void loadReminderList();
	void saveReminderList();
public:
	//如果循环打开，设置下一个循环点
	bool setNextReminder(ReminderInfo_ptr r);
public:
	//新增或更新配置
	bool refreshReminder(ReminderInfo_ptr r);
	ReminderInfo_ptr deleteReminder(ReminderInfo_ptr r);
public:
	ReminderInfo_ptr getReminderByRecordId(int id);
	ReminderInfo_ptr getReminderByEventId(int id);
public:
	//清除所有提醒
	void clearReminder();
	//恢复所有提醒
	void recoverReminder();
public:
	wchar_t* rinipath;
	list<ReminderInfo_ptr> list_reminder;
};

//提醒设置界面
class Ui_CashReminderWnd : public CMzWndEx 
{
	MZ_DECLARE_DYNAMIC(Ui_CashReminderWnd);
public:
	Ui_CashReminderWnd(void);
	~Ui_CashReminderWnd(void);
	void setupReminderInfo(ReminderInfo_ptr preminder){
		r = preminder;
	}
	void setEnable(bool e){
		m_BtnReminderSW.SetState(e ? MZCS_BUTTON_PRESSED : MZCS_BUTTON_NORMAL);
	}
	bool isEnabled(){
		return (m_BtnReminderSW.GetState() == MZCS_BUTTON_PRESSED);
	}
private:
	void updateUi();
	bool checkDateText();
public:
    UiToolbar_Text m_Toolbar;	//确定
	UiButton m_StaticDate;		//提醒日期
    UiSingleLineEdit m_EdtHour;	//提醒小时
    UiSingleLineEdit m_EdtMin;	//提醒分钟
	UiButtonEx m_BtnRepeat;		//重复按钮 ON/OFF
	UiButtonEx m_BtnRepeatSW;	//重复按钮 ON/OFF
	UiList_2 m_ListRepeatType;	//重复类型
	UiButtonEx m_BtnReminder;
	UiButtonEx m_BtnReminderSW;
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	ReminderInfo_ptr r;
};
