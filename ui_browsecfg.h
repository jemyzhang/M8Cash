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
		ORDERBYDATE	=	1,
		ORDERBYACCOUNT	=	2,
		ORDERBYCATEGORY	=	4,
	}BROWSEORDERMODE_t;
public:
	Ui_BrowsecfgWnd();
	void updateUi();
	BROWSEMODE_t getBrowseMode(){ 
		return _browseMode;
	}
	BROWSEORDERMODE_t getOrderMode(){ 
		return  _orderMode; 
	}
	void setBrowseMode(BROWSEMODE_t t){ 
		_browseMode = t;
	}
	void setOrderMode(BROWSEORDERMODE_t t){ 
		_orderMode = t; 
	}
public:
    UiScrollWin m_ScrollWin;
    UiToolbar_Text m_Toolbar;
	UiCaption m_lblTitle;
    UiButtonEx m_BtnReportOut;	//月支出报表
    UiButtonEx m_BtnReportIn;	//月收入报表
    UiButtonEx m_BtnReportYInOut;	//年度收入支出对比图
    UiButtonEx m_BtnBrowseMode;	//浏览模式
    UiButtonEx m_BtnOrderMode;	//排序方式
	UiList	m_DetailList;	//配置列表

protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC window messages handler
    LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
private:
	int _viewMode;	//0: normal 1: setting for mode, 2: setting for order
	BROWSEMODE_t _browseMode;
	BROWSEORDERMODE_t _orderMode;
};


#endif /*_UI_BROWSECFG_H*/