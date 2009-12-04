#ifndef _UI_MAIN_H
#define _UI_MAIN_H

// include the MZFC library header file
#include <mzfc_inc.h>
#include "ui_editmoney.h"
#include "ui_reminder.h"
#include "UiEditKeySwitcher.h"
#include "ui_password.h"

// Main window derived from CMzWndEx

class Ui_MainWnd : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(Ui_MainWnd);
public:
	Ui_MainWnd();
	~Ui_MainWnd();
	void updateText();
	void setEditMode(bool e){ _isEditMode = e; }
	void setEditIndex(int i){ _editIndex = i; }
	void initEditText();
	void setupDUi();	//dynamic ui
	void OnTimer(UINT nIDEvent);	//autolock timeout count
public:
    UiScrollWin m_ScrollWin;
    UiToolbar_Text m_Toolbar;
//    UiButton m_BtnAdd;
	UiCaption m_lblTitle;

	UiButton_Image m_BtnAdd;
	UiButton_Image m_BtnCalc;
	UiEditKeySwitchButton m_BtnKeyBoard;

	UiStatic m_lblAmount;
    UiButtonEx m_BtnDate;
    UiButtonEx m_BtnAccounts;
    UiButtonEx m_BtnToAccounts;

    UiButtonEx m_BtnCategory;
    UiEditMoney m_EdtAmountYuan;
	UiEditKeySwitcher m_EdtAmountMask;
    UiCaption m_lblDetail;
    UiCaption m_lblNote;
#ifdef USE_903SDK
    UiMultiLineEdit m_EdtNote;
#else
    UiEdit m_EdtNote;
#endif
    UiButtonEx m_BtnReminder;
	UiButtonEx m_BtnPerson;
private:
	int _selectedAccountIndex;
	int _selectedToAccountIndex;
	int _selectedCategoryIndex;
	int _selectedPersonIndex;
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC window messages handler
    LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);

	bool OnShellMessage(UINT message, WPARAM wParam, LPARAM lParam);

	void ShowBrowseDlg();
private:
	Ui_PasswordWnd *pdlgpwd;
	bool _isEditMode;
	int _editIndex;
	bool _readyAutolock;
private:
	ReminderInfo_t reminderinfo;
	bool _isReminderEnabled;
	bool checkReminderOnOff(bool isUpdate = false, int recordId = 0);
};


#endif /*_UI_MAIN_H*/