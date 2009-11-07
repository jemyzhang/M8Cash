#ifndef _UI_BROWSE_H
#define _UI_BROWSE_H

// include the MZFC library header file
#include <mzfc_inc.h>
#include "ui_browsecfg.h"
#include "cashdb.h"

class RecordList : public UiList {
public:
    // override the DrawItem member function to do your own drawing of the list
	RecordList() { idlist = 0; plist_record = 0;}
    void DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate);
	void setupList(int* i);
	void clearList();
	void setupDrawMode(Ui_BrowsecfgWnd::BROWSEMODE_t bt,Ui_BrowsecfgWnd::BROWSEORDERMODE_t ot) {
		_browseMode = bt;
		_orderMode = ot;
	}
protected:
private:
	int* idlist;
	CASH_RECORD_ptr *plist_record;	//»º´æ
	Ui_BrowsecfgWnd::BROWSEMODE_t _browseMode;
	Ui_BrowsecfgWnd::BROWSEORDERMODE_t _orderMode;
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
	UiCaption m_FilterBar;
	UiButton m_btnFilter;
#ifdef USE_903SDK
    UiMultiLineEdit m_StaticBar;
#endif
#ifdef USE_926SDK
    UiEdit m_StaticBar;
#endif

	UiCaption m_summary;

    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler

    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
	LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	int* idarray;

	Ui_BrowsecfgWnd::BROWSEMODE_t _browseMode;
	Ui_BrowsecfgWnd::BROWSEORDERMODE_t _orderMode;

	int sel_recordID;

	CMzWndEx* uiEditor;
};


#endif /*_UI_BROWSE_H*/
