#include "ui_browsecfg.h"

#include "m8cash.h"

#include "ui_report.h"
#include "..\MzCommon\MzCommon.h"
using namespace MzCommon;
#include "..\MzCommon\UiSingleOption.h"

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
	optionChanged = FALSE;
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

	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
	m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_RETURN).C_Str());
	m_Toolbar.EnableLeftArrow(true);
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_MAIN);
    AddUiWin(&m_Toolbar);

	updateUi();

    return TRUE;
}

void Ui_BrowsecfgWnd::updateUi(){
	m_BtnBrowseMode.SetText2(LOADSTRING(MODESTRID[appconfig.IniBrowseMode.Get()]).C_Str());
	m_BtnOrderMode.SetText2(LOADSTRING(ORDERSTRID[appconfig.IniBrowseOrderMode.Get()]).C_Str());
	Invalidate();
	UpdateWindow();
}

void Ui_BrowsecfgWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
		case MZ_IDC_BUTTON_MODE:
			ShowBrowseModeOptionDlg();
			updateUi();
			break;
		case MZ_IDC_BUTTON_ORDER:
			ShowOrderModeOptionDlg();
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
				if(optionChanged){
					// exit the modal dialog
					EndModal(ID_OK);
				}else{
					EndModal(ID_CANCEL);
				}
                return;
            }

		}
            break;
    }
}

void Ui_BrowsecfgWnd::ShowBrowseModeOptionDlg(){
    Ui_SingleOptionWnd dlg;
    for(int i = 0; i < sizeof(MODESTRID)/sizeof(MODESTRID[0]); i++){
		dlg.AppendOptionItem(const_cast<LPTSTR>(LOADSTRING(MODESTRID[i]).C_Str()));
    }
    dlg.SetSelectedIndex(appconfig.IniBrowseMode.Get());
    dlg.SetTitleText(L"设定浏览模式");
    RECT rcWork = MzGetWorkArea();
    dlg.Create(rcWork.left + 40, rcWork.top + 160, RECT_WIDTH(rcWork) - 80, 210 + 70*2,
        m_hWnd, 0, WS_POPUP);
    // set the animation of the window
    dlg.SetAnimateType_Show(MZ_ANIMTYPE_NONE);
    dlg.SetAnimateType_Hide(MZ_ANIMTYPE_FADE);
    int nRet = dlg.DoModal();
    if(nRet == ID_OK){
		if(appconfig.IniBrowseMode.Set(dlg.GetSelectedIndex())){
			optionChanged = TRUE;
		}
        updateUi();
    }
}

void Ui_BrowsecfgWnd::ShowOrderModeOptionDlg(){
    Ui_SingleOptionWnd dlg;
    for(int i = 0; i < sizeof(ORDERSTRID)/sizeof(ORDERSTRID[0]); i++){
		dlg.AppendOptionItem(const_cast<LPTSTR>(LOADSTRING(ORDERSTRID[i]).C_Str()));
    }
    dlg.SetSelectedIndex(appconfig.IniBrowseOrderMode.Get());
    dlg.SetTitleText(L"设定排序方式");
    RECT rcWork = MzGetWorkArea();
    dlg.Create(rcWork.left + 40, rcWork.top + 120, RECT_WIDTH(rcWork) - 80, 210 + 70*3,
        m_hWnd, 0, WS_POPUP);
    // set the animation of the window
    dlg.SetAnimateType_Show(MZ_ANIMTYPE_NONE);
    dlg.SetAnimateType_Hide(MZ_ANIMTYPE_FADE);
    int nRet = dlg.DoModal();
    if(nRet == ID_OK){
		if(appconfig.IniBrowseOrderMode.Set(dlg.GetSelectedIndex())){
			optionChanged = TRUE;
		}
        updateUi();
    }
}