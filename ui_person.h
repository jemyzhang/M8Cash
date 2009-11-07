#ifndef _UI_PERSON_H
#define _UI_PERSON_H

// include the MZFC library header file
#include <mzfc_inc.h>
#include "cashdb.h"

// Popup window derived from CMzWndEx

class UiNaviButton :
    public UiButton
{
public:
	UiNaviButton(){
		SetButtonMode(MZC_BUTTON_MODE_HOLD);
#ifdef USE_903SDK
		SetButtonType(MZC_BUTTON_LINE_NONE);
#endif
#ifdef USE_926SDK
		SetButtonType(MZC_BUTTON_LINE_RIGHT);
#endif
	}
public:
    virtual int OnLButtonUp(UINT fwKeys, int xPos, int yPos){
        SetState(MZCS_BUTTON_PRESSED);
        Invalidate();
        Update();
        return UiButton::OnLButtonUp(fwKeys,xPos,yPos);
    }
};

class PersonList : public UiList {
public:
    // override the DrawItem member function to do your own drawing of the list
	PersonList() { idlist = 0; }
	void DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate);
	void setupList(int* i) { idlist = i; }
protected:
private:
	int* idlist;
};

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
    UiCaption m_Caption1;
	UiNaviButton m_BtnAll;
	UiNaviButton m_BtnFamily;
	UiNaviButton m_BtnContact;
	UiNaviButton m_BtnInstitution;
    PersonList m_List;

    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
	//update display list
	void updateList();
	void setButtonStatus(CASH_PERSON_TYPE_t t);

private:
	int _mode;
    int _selection;
	int *idarray;
	int _sel_idx;
	CASH_PERSON_TYPE_t _listpersonType;	//-1: all
};

#endif /*_UI_PERSON_H*/