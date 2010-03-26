#ifndef _UI_EDIT_ACCOUNT_H
#define _UI_EDIT_ACCOUNT_H

// include the MZFC library header file
#include <mzfc_inc.h>
#include "ui_editmoney.h"
// Main window derived from CMzWndEx

class Ui_EditAccountWndEx : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(Ui_EditAccountWndEx);
public:

    Ui_EditAccountWndEx() {
        _mode = 0;
        _editIndex = 0;
		_selectedPidx = 0;
    }

    void setMode(int m) {
        _mode = m;
    }

    void setEditIndex(int i) {
        _editIndex = i;
    }
public:
	UiScrollWin m_ScrollWin;
	UiHeadingBar m_CaptionAccTitle;
    UiSingleLineEdit m_EdtAccName;
	UiHeadingBar m_CaptionAccInit;
    UiEditMoney m_EdtAccInitAmount;
	UiEditKeySwitcher m_EdtAmountMask;
	UiEditKeySwitchButton m_BtnKeyBoard;
	UiHeadingBar m_CaptionAccNote;
#ifdef USE_903SDK
    UiMultiLineEdit m_EdtAccNote;
#else
	UiEdit m_EdtAccNote;
#endif
    UiToolbar_Text m_Toolbar;
	UiButton_Image m_BtnCalc;
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
    LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
    int _mode; //0: new, 1: edit
    int _editIndex;
	int _selectedPidx;	//selected parent id
};


#endif /*_UI_EDIT_ACCOUNT_H*/