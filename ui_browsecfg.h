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
    UiButtonEx m_BtnReportOut;	//��֧������
    UiButtonEx m_BtnReportIn;	//�����뱨��
    UiButtonEx m_BtnReportYInOut;	//�������֧���Ա�ͼ
    UiButtonEx m_BtnBrowseMode;	//���ģʽ
    UiButtonEx m_BtnOrderMode;	//����ʽ
	UiList	m_DetailList;	//�����б�

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