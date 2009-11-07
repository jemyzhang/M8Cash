#ifndef _UI_ACCOUNTS_H
#define _UI_ACCOUNTS_H

// include the MZFC library header file
#include <mzfc_inc.h>

// Popup window derived from CMzWndEx
class AccountList : public UiList {
public:
    // override the DrawItem member function to do your own drawing of the list
	AccountList() { idlist = 0; }
	void DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate);
	void setupList(int* i) { idlist = i; }
protected:
private:
	int* idlist;
};

class Ui_AccountsWnd : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(Ui_AccountsWnd);
public:
	void setHideAccount(int h){
		_hideAccountId = h;
	}

    int getSelectionIndex() {
        return _selection;
    }

    Ui_AccountsWnd() {
		_mode = 0;
		idarray = 0;
		_hideAccountId = -1;
		_sel_idx = -1;
    }
	//1: selection mode
	void setMode(int m) { _mode = m; }
protected:
    UiToolbar_Text m_Toolbar;
    UiCaption m_Caption1;
    AccountList m_List;

    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
	//update display list
	void updateList();

private:
	int _mode;
    int _selection;
	int _hideAccountId;
	int *idarray;
	int _sel_idx;
};

#endif /*_UI_ACCOUNTS_H*/