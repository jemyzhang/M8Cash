#ifndef _UI_BROWSECFG_H
#define _UI_BROWSECFG_H

// include the MZFC library header file
#include <mzfc_inc.h>

// Main window derived from CMzWndEx

class Ui_BrowsecfgWnd : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(Ui_BrowsecfgWnd);
public:
	typedef enum BROWSEMODE{
		BROWSE_SIMPLE = 0,
		BROWSE_DETAIL = 1,
	}BROWSEMODE_t;
	typedef enum BROWSEORDERMODE{
		ORDERBYDATE	=	0,
		ORDERBYACCOUNT	=	1,
		ORDERBYCATEGORY	=	2,
	}BROWSEORDERMODE_t;
public:
	Ui_BrowsecfgWnd();
	void updateUi();
	void ShowBrowseModeOptionDlg();
	void ShowOrderModeOptionDlg();
public:
    UiScrollWin m_ScrollWin;
    UiToolbar_Text m_Toolbar;
	UiHeadingBar m_lblTitle;
    UiButtonEx m_BtnReportOut;	//月支出报表
    UiButtonEx m_BtnReportIn;	//月收入报表
    UiButtonEx m_BtnReportYInOut;	//年度收入支出对比图
    UiButtonEx m_BtnBrowseMode;	//浏览模式
    UiButtonEx m_BtnOrderMode;	//排序方式
	UiList	m_DetailList;	//配置列表

protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
private:
	BOOL optionChanged;	//是否选项有改变，改变就ID_OK返回
};


#endif /*_UI_BROWSECFG_H*/