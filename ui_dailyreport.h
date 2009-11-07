#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>

// Popup window derived from CMzWndEx

class DailyRecordList : public UiList {
public:
    // override the DrawItem member function to do your own drawing of the list
	DailyRecordList() { idlist = 0; }
    void DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate);
	void setupList(int* i) { idlist = i; }
protected:
private:
	int* idlist;
};

class Ui_DailyReportWnd : public CMzWndEx
{
    MZ_DECLARE_DYNAMIC(Ui_DailyReportWnd);
public:
	void setDetailIndex(int i);
public:
    UiToolbar_Text m_Toolbar;
    UiCaption m_CaptionDate;	//日期
    UiCaption m_CaptionList;	//列表
    UiCaption m_CaptionDetail;	//详情

	UiScrollWin m_ScrollWinStatics;	// 统计滚动窗
#ifdef USE_903SDK
    UiMultiLineEdit m_TextStatics;		//统计	未知行
#endif
#ifdef USE_926SDK
    UiEdit m_TextStatics;		//统计	未知行
#endif

	DailyRecordList m_ListDetail;		//明细列表

#ifdef USE_903SDK
    UiMultiLineEdit m_TextDetail;		//详情	2行
#endif
#ifdef USE_926SDK
    UiEdit m_TextDetail;		//详情	2行
#endif

#ifdef EN_IMAGE_BTN
	UiButton_Image m_BtnLeft;
	UiButton_Image m_BtnRight;
#endif
	Ui_DailyReportWnd(void);
	~Ui_DailyReportWnd(void);
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
	LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);

	void updateDetail();
	void updateDetailText();
private:
	int _seletedIndex;
	int* idarray; 
private:
	SYSTEMTIME t;
};
