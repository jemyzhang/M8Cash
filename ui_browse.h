#ifndef _UI_BROWSE_H
#define _UI_BROWSE_H

// include the MZFC library header file
#include <mzfc_inc.h>
#include "ui_browsecfg.h"
#include "database.h"

class RecordList : public UiList {
public:
    // override the DrawItem member function to do your own drawing of the list
	RecordList() { idlist = 0; plist_record = 0;}
    void DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate);
	void setupList(int* i);
	void clearList();
protected:
private:
	int* idlist;
	CASH_TRANSACT_ptr *plist_record;	//»º´æ
};

// Popup window derived from CMzWndEx

class Ui_BrowseWnd : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(Ui_BrowseWnd);
public:
	Ui_BrowseWnd();
	~Ui_BrowseWnd();
	void updateUi();
	void refreshUi();
	void setEditFormHandle(CMzWndEx* h) { uiEditor = h; }
protected:
    UiToolbar_Text m_Toolbar;
    RecordList m_RecordList;
	UiHeadingBar m_FilterBar;
	UiButton m_btnFilter;
#ifdef USE_903SDK
    UiMultiLineEdit m_StaticBar;
#else
    UiEdit m_StaticBar;
#endif

	UiHeadingBar m_summary;

    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler

    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
	LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
	void OnTimer(UINT_PTR nIDEvent){
		switch(nIDEvent){
			case 0x8001:
				::KillTimer(m_hWnd,0x8001);
				updateUi();
				break;
		}
	}
private:
	int* idarray;

	int sel_recordID;

	CMzWndEx* uiEditor;
};


#endif /*_UI_BROWSE_H*/
