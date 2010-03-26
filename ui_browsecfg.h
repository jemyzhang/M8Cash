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
    UiButtonEx m_BtnReportOut;	//��֧������
    UiButtonEx m_BtnReportIn;	//�����뱨��
    UiButtonEx m_BtnReportYInOut;	//�������֧���Ա�ͼ
    UiButtonEx m_BtnBrowseMode;	//���ģʽ
    UiButtonEx m_BtnOrderMode;	//����ʽ
	UiList	m_DetailList;	//�����б�

protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
private:
	BOOL optionChanged;	//�Ƿ�ѡ���иı䣬�ı��ID_OK����
};


#endif /*_UI_BROWSECFG_H*/