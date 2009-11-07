#include "ui_browsecfg.h"

#include "m8cash.h"

#include "ui_report.h"
#include "..\MzCommon\MzCommon.h"
using namespace MzCommon;

#define MZ_IDC_TOOLBAR_MAIN 101
#define MZ_IDC_SCROLLWIN 102

#define MZ_IDC_BUTTON_MODE 103
#define MZ_IDC_BUTTON_ORDER 104
#define MZ_IDC_BUTTON_REPORT_OUT 105
#define MZ_IDC_BUTTON_REPORT_IN 106
#define MZ_IDC_BUTTON_REPORT_YINOUT 107
#define MZ_IDC_LIST_CONFIG 108

MZ_IMPLEMENT_DYNAMIC(Ui_BrowsecfgWnd)

#define N_BTN_BEFORE 4

UINT MODESTRID[] = {
	IDS_STR_SIMPLE,
	IDS_STR_DETAIL,
};

UINT ORDERSTRID[] = {
	IDS_STR_BYTIME,
	IDS_STR_BYACCOUNT,
	IDS_STR_BYCATEGORY,
};

Ui_BrowsecfgWnd::Ui_BrowsecfgWnd(){
	_viewMode = 0;
	_browseMode = BROWSE_SIMPLE;
	_orderMode = ORDERBYDATE;
}

BOOL Ui_BrowsecfgWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 0;
	m_lblTitle.SetPos(0,y,GetWidth(),MZM_HEIGHT_CAPTION);
	m_lblTitle.SetText(LOADSTRING(IDS_STR_MORE).C_Str());
    AddUiWin(&m_lblTitle);

	y += MZM_HEIGHT_CAPTION;
    m_BtnReportOut.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnReportOut.SetText(LOADSTRING(IDS_STR_MONTHLY_EXREPORT).C_Str());
    m_BtnReportOut.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_BtnReportOut.SetID(MZ_IDC_BUTTON_REPORT_OUT);
	m_BtnReportOut.SetTextMaxLen(0);
    m_BtnReportOut.SetImage2(imgArrow);
    m_BtnReportOut.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnReportOut.SetShowImage2(true);
	m_BtnReportOut.SetImage1(pimg[IDB_PNG_PIECHART - IDB_PNG_CALC]);
	m_BtnReportOut.SetImageWidth1(pimg[IDB_PNG_PIECHART - IDB_PNG_CALC]->GetImageWidth());
	m_BtnReportOut.SetShowImage1(true);
    AddUiWin(&m_BtnReportOut);

	y += MZM_HEIGHT_BUTTONEX;
    m_BtnReportIn.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnReportIn.SetText(LOADSTRING(IDS_STR_MONTHLY_INREPORT).C_Str());
    m_BtnReportIn.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_BtnReportIn.SetID(MZ_IDC_BUTTON_REPORT_IN);
	m_BtnReportIn.SetTextMaxLen(0);
    m_BtnReportIn.SetImage2(imgArrow);
    m_BtnReportIn.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnReportIn.SetShowImage2(true);
	m_BtnReportIn.SetImage1(pimg[IDB_PNG_PIECHART - IDB_PNG_CALC]);
	m_BtnReportIn.SetImageWidth1(pimg[IDB_PNG_PIECHART - IDB_PNG_CALC]->GetImageWidth());
	m_BtnReportIn.SetShowImage1(true);
    AddUiWin(&m_BtnReportIn);

	y += MZM_HEIGHT_BUTTONEX;
    m_BtnReportYInOut.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
	m_BtnReportYInOut.SetTextMaxLen(0);
    m_BtnReportYInOut.SetText(LOADSTRING(IDS_STR_YEARLY_REPORT).C_Str());
	m_BtnReportYInOut.SetDrawTextFormat(DT_LEFT|DT_VCENTER);
    m_BtnReportYInOut.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_BtnReportYInOut.SetID(MZ_IDC_BUTTON_REPORT_YINOUT);
	m_BtnReportYInOut.SetTextMaxLen(0);
    m_BtnReportYInOut.SetImage2(imgArrow);
    m_BtnReportYInOut.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnReportYInOut.SetShowImage2(true);
	m_BtnReportYInOut.SetImage1(pimg[IDB_PNG_LINECHART - IDB_PNG_CALC]);
	m_BtnReportYInOut.SetImageWidth1(pimg[IDB_PNG_LINECHART - IDB_PNG_CALC]->GetImageWidth());
	m_BtnReportYInOut.SetShowImage1(true);
    AddUiWin(&m_BtnReportYInOut);

	y += MZM_HEIGHT_BUTTONEX;
    m_BtnBrowseMode.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnBrowseMode.SetText(LOADSTRING(IDS_STR_BROWSE_MODE).C_Str());
    m_BtnBrowseMode.SetTextSize2(m_BtnBrowseMode.GetTextSize2() - 2);
    m_BtnBrowseMode.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_BtnBrowseMode.SetID(MZ_IDC_BUTTON_MODE);
	m_BtnBrowseMode.SetTextMaxLen(0);
    m_BtnBrowseMode.SetImage2(imgArrow);
    m_BtnBrowseMode.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnBrowseMode.SetShowImage2(true);
    AddUiWin(&m_BtnBrowseMode);

	y += MZM_HEIGHT_BUTTONEX;
    m_BtnOrderMode.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnOrderMode.SetText(LOADSTRING(IDS_STR_ORDER_MODE).C_Str());
    m_BtnOrderMode.SetTextSize2(m_BtnOrderMode.GetTextSize2() - 2);
    m_BtnOrderMode.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_BtnOrderMode.SetID(MZ_IDC_BUTTON_ORDER);
	m_BtnOrderMode.SetTextMaxLen(0);
    //set the right image of the extended button m_BtnSetting1
    m_BtnOrderMode.SetImage2(imgArrow);
    m_BtnOrderMode.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnOrderMode.SetShowImage2(true);
    AddUiWin(&m_BtnOrderMode);

	y+=MZM_HEIGHT_BUTTONEX;
    m_ScrollWin.SetPos(0, y, GetWidth(), GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR);
    m_ScrollWin.SetID(MZ_IDC_SCROLLWIN);
    m_ScrollWin.EnableScrollBarV(true);
    AddUiWin(&m_ScrollWin);

	m_DetailList.SetPos(0, 0, GetWidth(), GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR);
    m_DetailList.SetID(MZ_IDC_LIST_CONFIG);
	m_DetailList.SetItemHeight(50);
	m_DetailList.SetTextSize(m_DetailList.GetTextSize() - 4);
	m_DetailList.SetDrawTextFormat(DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	m_DetailList.EnableScrollBarV(true);
	m_DetailList.EnableNotifyMessage(true);
	m_ScrollWin.AddChild(&m_DetailList);

	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
	m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_RETURN).C_Str());
	m_Toolbar.EnableLeftArrow(true);
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_MAIN);
    AddUiWin(&m_Toolbar);

	updateUi();

    return TRUE;
}

void Ui_BrowsecfgWnd::updateUi(){
	m_BtnBrowseMode.SetText2(LOADSTRING(MODESTRID[_browseMode]).C_Str());
	m_BtnOrderMode.SetText2(LOADSTRING(ORDERSTRID[_orderMode/2]).C_Str());
	if(_viewMode == 0){
		m_ScrollWin.SetVisible(false);
	    m_BtnOrderMode.SetPos(0, MZM_HEIGHT_BUTTONEX*N_BTN_BEFORE + MZM_HEIGHT_CAPTION, GetWidth(), MZM_HEIGHT_BUTTONEX);
	}
	if(_viewMode == 1){
	    ListItem li;
		CMzString str(128);
		m_DetailList.RemoveAll();
		for(int i = 0; i < 2; i++){
			str = LOADSTRING(MODESTRID[i]);
			li.Text = str;
			m_DetailList.AddItem(li);
		}
		m_BtnOrderMode.SetPos(0,MZM_HEIGHT_BUTTONEX*N_BTN_BEFORE + MZM_HEIGHT_CAPTION + m_DetailList.CalcItemHeight(-1)*2, GetWidth(), MZM_HEIGHT_BUTTONEX);
		m_ScrollWin.SetPos(0, MZM_HEIGHT_BUTTONEX*N_BTN_BEFORE + MZM_HEIGHT_CAPTION, GetWidth(), GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR);
        m_DetailList.SetSelectedIndex(_browseMode);
		m_ScrollWin.SetVisible(true);
	}
	if(_viewMode == 2){
	    ListItem li;
		CMzString str(128);
		m_DetailList.RemoveAll();
		for(int i = 0; i < 3; i++){
			str = LOADSTRING(ORDERSTRID[i]);
			li.Text = str;
			m_DetailList.AddItem(li);
		}
	    m_BtnOrderMode.SetPos(0,MZM_HEIGHT_BUTTONEX*N_BTN_BEFORE + MZM_HEIGHT_CAPTION, GetWidth(), MZM_HEIGHT_BUTTONEX);
		m_ScrollWin.SetPos(0, MZM_HEIGHT_BUTTONEX*(N_BTN_BEFORE + 1) + MZM_HEIGHT_CAPTION, GetWidth(), GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR);
        m_DetailList.SetSelectedIndex(_orderMode/2);
		m_ScrollWin.SetVisible(true);
	}
	Invalidate();
	UpdateWindow();
}
//const BROWSEORDERMODE_t order_mode[] = {ORDERBYDATE,ORDERBYACCOUNT,ORDERBYCATEGORY};
LRESULT Ui_BrowsecfgWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case MZ_WM_MOUSE_NOTIFY:
        {
            int nID = LOWORD(wParam);
            int nNotify = HIWORD(wParam);
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            if (nID == MZ_IDC_LIST_CONFIG && nNotify == MZ_MN_LBUTTONDOWN) {
                if (!m_DetailList.IsMouseDownAtScrolling() && !m_DetailList.IsMouseMoved()) {
                    int nIndex = m_DetailList.CalcIndexOfPos(x, y);
					if(nIndex != -1){
						if(_viewMode == 1){
							if(nIndex < 2){
								_browseMode = (BROWSEMODE_t)nIndex;
							}
						}else if(_viewMode == 2){
							if(nIndex < 3){
								_orderMode = (BROWSEORDERMODE_t)(1<<nIndex);
							}
						}
					}
					_viewMode = 0;
					updateUi();
                }
                return 0;
            }
            if (nID == MZ_IDC_LIST_CONFIG && nNotify == MZ_MN_MOUSEMOVE) {
                m_DetailList.SetSelectedIndex(-1);
                m_DetailList.Invalidate();
                m_DetailList.Update();
                return 0;
            }
        }
    }
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

void Ui_BrowsecfgWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
		case MZ_IDC_BUTTON_MODE:
			_viewMode = (_viewMode == 1) ? 0 : 1;
			updateUi();
			break;
		case MZ_IDC_BUTTON_ORDER:
			_viewMode = (_viewMode == 2) ? 0 : 2;
			updateUi();
			break;
		case MZ_IDC_BUTTON_REPORT_OUT:
		{
			Ui_MonthReportWnd dlg;
			RECT rcWork = MzGetWorkArea();
			dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
					m_hWnd, 0, WS_POPUP);
			dlg.setMode(CT_OUTGOING);
			// set the animation of the window
			dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
			dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
			int nRet = dlg.DoModal();
			break;
		}
		case MZ_IDC_BUTTON_REPORT_IN:
		{
			Ui_MonthReportWnd dlg;
			RECT rcWork = MzGetWorkArea();
			dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
					m_hWnd, 0, WS_POPUP);
			dlg.setMode(CT_INCOME);
			// set the animation of the window
			dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
			dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
			int nRet = dlg.DoModal();
			break;
		}
		case MZ_IDC_BUTTON_REPORT_YINOUT:
		{
			Ui_YearlyReportWnd dlg;
			RECT rcWork = MzGetWorkArea();
			dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
					m_hWnd, 0, WS_POPUP);
			// set the animation of the window
			dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
			dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
			int nRet = dlg.DoModal();
			break;
		}
        case MZ_IDC_TOOLBAR_MAIN:
        {
            int nIndex = lParam;
            if (nIndex == 0) {
                // exit the modal dialog
                EndModal(ID_OK);
                return;
            }

		}
            break;
    }
}
