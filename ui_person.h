#ifndef _UI_PERSON_H
#define _UI_PERSON_H

// include the MZFC library header file
#include <mzfc_inc.h>
#include "database.h"

class PersonList : public UiList {
public:
    // override the DrawItem member function to do your own drawing of the list
	PersonList() { idlist = 0; SetItemHeight(60); SetTextSize(GetTextSize() - 10);}
	void DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate);
	void setupList(int* i) { idlist = i; }
protected:
private:
	int* idlist;
};

// Popup window derived from CMzWndEx
class Ui_PersonsWnd : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(Ui_PersonsWnd);
public:
	void setListPersonType(CASH_PERSON_TYPE_t t = PRSN_ALL){
		_listpersonType = t;
	}

    int getSelectionIndex() {
        return _selection;
    }

    Ui_PersonsWnd() {
		_mode = 0;
		idarray = 0;
		_sel_idx = -1;
		_listpersonType = PRSN_ALL;
    }
	//1: selection mode
	void setMode(int m) { _mode = m; }
protected:
    UiToolbar_Text m_Toolbar;
    UiHeadingBar m_Caption;
	UiButtonBar m_PersonTypeBar;
    PersonList m_List;

    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
	//update display list
	void updateList();

    void SetupPersonTypeBar();

private:
	int _mode;
    int _selection;
	int *idarray;
	int _sel_idx;
	CASH_PERSON_TYPE_t _listpersonType;	//-1: all
};

#endif /*_UI_PERSON_H*/