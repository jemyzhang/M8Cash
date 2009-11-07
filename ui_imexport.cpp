#include "ui_imexport.h"
#include "ui_processimexport.h"
#include "m8cash.h"
#include "..\MzCommon\MzCommon.h"
using namespace MzCommon;

#include "ui_calendar.h"

MZ_IMPLEMENT_DYNAMIC(Ui_ImExportWnd)
MZ_IMPLEMENT_DYNAMIC(Ui_ExDateRangeWnd)

#define MZ_IDC_TOOLBAR_CONFIG 101
#define MZ_IDC_BUTTON_IMEX_ACCOUNT 102
#define MZ_IDC_BUTTON_IMEX_CATEGORY 103
#define MZ_IDC_BUTTON_IMEX_RECORD 104
#define MZ_IDC_BUTTON_EX_RANGE 105

#define IDC_PPM_QIF 106
#define IDC_PPM_COMMA 107
#define IDC_PPM_TAB 108
#define IDC_PPM_CANCEL 109

BOOL Ui_ExDateRangeWnd::OnInitDialog(){
	// Must all the Init of parent class first!
	if (!CMzWndEx::OnInitDialog()) {
		return FALSE;
	}

	// Then init the controls & other things in the window
	m_bg.SetPos(0,0,GetWidth(),GetHeight());
	AddUiWin(&m_bg);

	int y = 20;
	m_s1.SetPos(10,y + 10,50,50);
	m_s1.SetID(MZ_IDC_BTN_S1);
	m_s1.EnableNotifyMessage(true);
	m_bg.AddChild(&m_s1);

	m_LblAll.SetPos(60,y,GetWidth() - 50,MZM_HEIGHT_BUTTONEX);
	m_LblAll.SetText(LOADSTRING(IDS_STR_EXPORT_CHOOSE_ALL).C_Str());
	m_LblAll.SetDrawTextFormat(DT_LEFT | DT_VCENTER);
	m_LblAll.SetTextColor(RGB(255,255,255));
	m_LblAll.SetID(MZ_IDC_BTN_ALL);
	m_LblAll.EnableNotifyMessage(true);
	m_bg.AddChild(&m_LblAll);

	y += MZM_HEIGHT_BUTTONEX;
	m_s2.SetPos(10,y + 10,50,50);
	m_s2.SetID(MZ_IDC_BTN_S2);
	m_s2.EnableNotifyMessage(true);
	m_bg.AddChild(&m_s2);

	m_EdtFromDate.SetPos(60,y,(GetWidth()-10-60-40)/2,MZM_HEIGHT_BUTTONEX);
	m_EdtFromDate.SetButtonType(MZC_BUTTON_NONE);
	m_EdtFromDate.SetID(MZ_IDC_BTN_EFROM);
	m_bg.AddChild(&m_EdtFromDate);

	m_LblTo.SetPos(60+m_EdtFromDate.GetWidth(),y,40,MZM_HEIGHT_BUTTONEX);
	m_LblTo.SetText(L"～");
	m_LblTo.SetTextColor(RGB(255,255,255));
	m_bg.AddChild(&m_LblTo);

	m_EdtToDate.SetPos(GetWidth() - m_EdtFromDate.GetWidth() - 10,y,m_EdtFromDate.GetWidth(),MZM_HEIGHT_BUTTONEX);
	m_EdtToDate.SetButtonType(MZC_BUTTON_NONE);
	m_EdtToDate.SetID(MZ_IDC_BTN_ETO);
	m_bg.AddChild(&m_EdtToDate);

	y += MZM_HEIGHT_BUTTONEX;
	m_BtnOK.SetPos(0,y,GetWidth()/2,MZM_HEIGHT_BUTTONEX + 20);
	m_BtnOK.SetText(LOADSTRING(IDS_STR_OK).C_Str());
	m_BtnOK.SetID(MZ_IDC_BTN_OK);
	m_bg.AddChild(&m_BtnOK);

	m_BtnCancel.SetPos(GetWidth()/2,y,GetWidth()/2,MZM_HEIGHT_BUTTONEX + 20);
	m_BtnCancel.SetText(LOADSTRING(IDS_STR_CANCEL).C_Str());
	m_BtnCancel.SetID(MZ_IDC_BTN_CANCEL);
	m_bg.AddChild(&m_BtnCancel);

	updateUi();

	return TRUE;
}

void Ui_ExDateRangeWnd::OnMzCommand(WPARAM wParam, LPARAM lParam){
	UINT_PTR id = LOWORD(wParam);
	switch (id) {
		case MZ_IDC_BTN_EFROM:
		{
			Ui_CalendarWnd calendardlg;
            RECT rcWork = MzGetWorkArea();
			calendardlg.initDate(_sdate.Date.Year,_sdate.Date.Month,_sdate.Date.Day);
            calendardlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
                    m_hWnd, 0, WS_POPUP);
            // set the animation of the window
            calendardlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
            calendardlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
            int nRet = calendardlg.DoModal();
            if (nRet == ID_OK) {
				DWORD year,month,day;
				swscanf(calendardlg.getDate().C_Str(),L"%04d-%02d-%02d",
					&year,&month,&day);
				_sdate.Date.Year = year;
				_sdate.Date.Month = month;
				_sdate.Date.Day = day;
				_selIndex = false;
				updateUi();
            }
			break;
		}
		case MZ_IDC_BTN_ETO:
		{
			Ui_CalendarWnd calendardlg;
            RECT rcWork = MzGetWorkArea();
			calendardlg.initDate(_edate.Date.Year,_edate.Date.Month,_edate.Date.Day);
            calendardlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
                    m_hWnd, 0, WS_POPUP);
            // set the animation of the window
            calendardlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
            calendardlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
            int nRet = calendardlg.DoModal();
            if (nRet == ID_OK) {
				DWORD year,month,day;
				swscanf(calendardlg.getDate().C_Str(),L"%04d-%02d-%02d",
					&year,&month,&day);
				_edate.Date.Year = year;
				_edate.Date.Month = month;
				_edate.Date.Day = day;
				_selIndex = false;
				updateUi();
            }
			break;
		}
		case MZ_IDC_BTN_OK:
			if(!_selIndex){
				if(_edate.Value == 0 && _sdate.Value == 0){
					return;
				}
				if(_sdate.Value > _edate.Value){
					if(_sdate.Value != 0){
						_edate.Value = _sdate.Value;
					}else{
						_sdate.Value = _edate.Value;
					}
				}
			}
			EndModal(ID_OK);
			break;
		case MZ_IDC_BTN_CANCEL:
			EndModal(ID_CANCEL);
			break;
	}
	return;
}

Ui_ImExportWnd::Ui_ImExportWnd(void)
{
	_isImport = false;
	//读取导出日期设定
	_bExportAll = appconfig.IniExportAll.Get();
	_StartExportDate.Value = appconfig.IniExportStartDate.Get();
	_EndExportDate.Value = appconfig.IniExportEndDate.Get();
}

Ui_ImExportWnd::~Ui_ImExportWnd(void)
{
}


BOOL Ui_ImExportWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 0;
    m_CaptionTitle.SetPos(0, y, GetWidth(), MZM_HEIGHT_CAPTION);
	if(_isImport){
		m_CaptionTitle.SetText(LOADSTRING(IDS_STR_IMPORT).C_Str());
	}else{
		m_CaptionTitle.SetText(LOADSTRING(IDS_STR_EXPORT).C_Str());
	}
    AddUiWin(&m_CaptionTitle);

    y += MZM_HEIGHT_CAPTION;
    m_BtnImExAccounts.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnImExAccounts.SetText(LOADSTRING(IDS_STR_ACCOUNT).C_Str());
    m_BtnImExAccounts.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_BtnImExAccounts.SetID(MZ_IDC_BUTTON_IMEX_ACCOUNT);

    //set the right image of the extended button m_BtnSetting1
    m_BtnImExAccounts.SetImage2(imgArrow);
    m_BtnImExAccounts.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnImExAccounts.SetShowImage2(true);
    AddUiWin(&m_BtnImExAccounts);

    y += MZM_HEIGHT_BUTTONEX;
    m_BtnImExCategories.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnImExCategories.SetText(LOADSTRING(IDS_STR_CATEGORY).C_Str());
    m_BtnImExCategories.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_BtnImExCategories.SetID(MZ_IDC_BUTTON_IMEX_CATEGORY);

	m_BtnImExCategories.SetImage2(imgArrow);
    m_BtnImExCategories.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnImExCategories.SetShowImage2(true);
    AddUiWin(&m_BtnImExCategories);

    y += MZM_HEIGHT_BUTTONEX;
    m_BtnImExRecords.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnImExRecords.SetText(LOADSTRING(IDS_STR_RECORDS).C_Str());
    m_BtnImExRecords.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    m_BtnImExRecords.SetID(MZ_IDC_BUTTON_IMEX_RECORD);

	m_BtnImExRecords.SetImage2(imgArrow);
    m_BtnImExRecords.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnImExRecords.SetShowImage2(true);
    AddUiWin(&m_BtnImExRecords);

	if(!_isImport){
		y += MZM_HEIGHT_BUTTONEX;
		m_BtnExRange.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
		m_BtnExRange.SetText(LOADSTRING(IDS_STR_DATE_RANGE).C_Str());
		m_BtnExRange.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
		m_BtnExRange.SetID(MZ_IDC_BUTTON_EX_RANGE);

		m_BtnExRange.SetImage2(imgArrow);
		m_BtnExRange.SetImageWidth2(imgArrow->GetImageWidth());
		m_BtnExRange.SetShowImage2(true);
		AddUiWin(&m_BtnExRange);
	}
    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    m_Toolbar.SetButton(0, true, true,LOADSTRING(IDS_STR_RETURN).C_Str());
    m_Toolbar.EnableLeftArrow(true);
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_CONFIG);
    AddUiWin(&m_Toolbar);

	updateButtonRange();

    return TRUE;
}

void Ui_ImExportWnd::updateButtonRange(){
	if(_bExportAll){
		m_BtnExRange.SetText2(LOADSTRING(IDS_STR_ALLDATE).C_Str());
	}else{
		if(_StartExportDate.Value == 0 || _EndExportDate.Value == 0){
			_bExportAll = true;
			m_BtnExRange.SetText2(LOADSTRING(IDS_STR_ALLDATE).C_Str());
		}else{
			wchar_t strDate[32];
			if(_StartExportDate.Value == _EndExportDate.Value){
				wsprintf(strDate,L"%d.%d.%d",
					_StartExportDate.Date.Year,_StartExportDate.Date.Month,_StartExportDate.Date.Day);
			}else{
				wsprintf(strDate,L"%d.%d.%d-%d.%d.%d",
					_StartExportDate.Date.Year,_StartExportDate.Date.Month,_StartExportDate.Date.Day,
					_EndExportDate.Date.Year,_EndExportDate.Date.Month,_EndExportDate.Date.Day);
			}
			m_BtnExRange.SetText2(strDate);
		}
	}
	appconfig.IniExportAll.Set(_bExportAll);
	appconfig.IniExportStartDate.Set(_StartExportDate.Value);
	appconfig.IniExportEndDate.Set(_EndExportDate.Value);

	m_BtnExRange.Invalidate();
	m_BtnExRange.Update();
}

void Ui_ImExportWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
	// pop out a PopupMenu:
	CPopupMenu ppm;
	struct PopupMenuItemProp pmip;      

	pmip.itemCr = MZC_BUTTON_PELLUCID;
	pmip.itemRetID = IDC_PPM_CANCEL;
	pmip.str = LOADSTRING(IDS_STR_CANCEL).C_Str();
	ppm.AddItem(pmip);  

	if(!_isImport){
		pmip.itemCr = MZC_BUTTON_PELLUCID;
		pmip.itemRetID = IDC_PPM_QIF;
		pmip.str = LOADSTRING(IDS_STR_EXPORT_QIF).C_Str();
		ppm.AddItem(pmip);
	}

	pmip.itemCr = MZC_BUTTON_PELLUCID;
	pmip.itemRetID = IDC_PPM_TAB;
	pmip.str = _isImport ? LOADSTRING(IDS_STR_IMPORT_TC).C_Str() : LOADSTRING(IDS_STR_EXPORT_TC).C_Str();
	ppm.AddItem(pmip);  

	pmip.itemCr = MZC_BUTTON_PELLUCID;
	pmip.itemRetID = IDC_PPM_COMMA;
	pmip.str = _isImport ? LOADSTRING(IDS_STR_IMPORT_CC).C_Str() : LOADSTRING(IDS_STR_EXPORT_CC).C_Str();
	ppm.AddItem(pmip);

    switch (id) {
        case MZ_IDC_BUTTON_IMEX_ACCOUNT:
		{
			int mode = 0;
			Ui_ProcessImExport proc;
			mode |= _isImport ? Ui_ProcessImExport::PROCESS_IMPORT : Ui_ProcessImExport::PROCESS_EXPORT;
			mode |= Ui_ProcessImExport::PROCESS_ACCOUNT;

			RECT rc = MzGetWorkArea();      
			rc.top = rc.bottom - ppm.GetHeight();
			ppm.Create(rc.left,rc.top,RECT_WIDTH(rc),RECT_HEIGHT(rc),m_hWnd,0,WS_POPUP);      
			int nID = ppm.DoModal();
			if (nID==IDC_PPM_TAB)
			{
				mode |= Ui_ProcessImExport::PROCESS_FILE_CSV_S;
			}else if(nID == IDC_PPM_COMMA){
				mode |= Ui_ProcessImExport::PROCESS_FILE_CSV_C;
			}else if(nID == IDC_PPM_QIF){
				mode |= Ui_ProcessImExport::PROCESS_FILE_QIF;
			}else{
				return;
			}

			bool ret = proc.excute(mode);
			if(proc.isNoFileSelected()) return;
			wchar_t* msg = proc.getLastErrMsg();
			MzMessageBoxEx(m_hWnd,msg,ret ? LOADSTRING(IDS_STR_SUCCEED).C_Str() : LOADSTRING(IDS_STR_FAILED).C_Str() );
            break;
		}
        case MZ_IDC_BUTTON_IMEX_CATEGORY:
		{
			int mode = 0;
			Ui_ProcessImExport proc;
			mode |= _isImport ? Ui_ProcessImExport::PROCESS_IMPORT : Ui_ProcessImExport::PROCESS_EXPORT;
			mode |= Ui_ProcessImExport::PROCESS_CATEGORY;

			RECT rc = MzGetWorkArea();      
			rc.top = rc.bottom - ppm.GetHeight();
			ppm.Create(rc.left,rc.top,RECT_WIDTH(rc),RECT_HEIGHT(rc),m_hWnd,0,WS_POPUP);      
			int nID = ppm.DoModal();
			if (nID==IDC_PPM_TAB)
			{
				mode |= Ui_ProcessImExport::PROCESS_FILE_CSV_S;
			}else if(nID == IDC_PPM_COMMA){
				mode |= Ui_ProcessImExport::PROCESS_FILE_CSV_C;
			}else if(nID == IDC_PPM_QIF){
				mode |= Ui_ProcessImExport::PROCESS_FILE_QIF;
			}else{
				return;
			}

			bool ret = proc.excute(mode);
			if(proc.isNoFileSelected()) return;
			wchar_t* msg = proc.getLastErrMsg();
			MzMessageBoxEx(m_hWnd,msg,ret ? LOADSTRING(IDS_STR_SUCCEED).C_Str() : LOADSTRING(IDS_STR_FAILED).C_Str());
            break;
		}
        case MZ_IDC_BUTTON_IMEX_RECORD:
		{
			int mode = 0;
			Ui_ProcessImExport proc;
			mode |= _isImport ? Ui_ProcessImExport::PROCESS_IMPORT : Ui_ProcessImExport::PROCESS_EXPORT;
			mode |= Ui_ProcessImExport::PROCESS_RECORD;

			RECT rc = MzGetWorkArea();      
			rc.top = rc.bottom - ppm.GetHeight();
			ppm.Create(rc.left,rc.top,RECT_WIDTH(rc),RECT_HEIGHT(rc),m_hWnd,0,WS_POPUP);      
			int nID = ppm.DoModal();
			if (nID==IDC_PPM_TAB)
			{
				mode |= Ui_ProcessImExport::PROCESS_FILE_CSV_S;
			}else if(nID == IDC_PPM_COMMA){
				mode |= Ui_ProcessImExport::PROCESS_FILE_CSV_C;
			}else if(nID == IDC_PPM_QIF){
				mode |= Ui_ProcessImExport::PROCESS_FILE_QIF;
			}else{
				return;
			}

			//设置时间范围
			proc.setExportDateRange(_bExportAll,
				_StartExportDate.Value,_EndExportDate.Value);
			bool ret = proc.excute(mode);
			if(proc.isNoFileSelected()) return;
			wchar_t* msg = proc.getLastErrMsg();
			MzMessageBoxEx(m_hWnd,msg,ret ? LOADSTRING(IDS_STR_SUCCEED).C_Str() : LOADSTRING(IDS_STR_FAILED).C_Str());
			break;
		}
        case MZ_IDC_BUTTON_EX_RANGE:
		{
			Ui_ExDateRangeWnd dlg;
			dlg.setSelection(_bExportAll);
			dlg.setFromDate(_StartExportDate);
			dlg.setToDate(_EndExportDate);
			RECT rcWork = MzGetWorkArea();
			dlg.Create(rcWork.left, rcWork.bottom - 250, RECT_WIDTH(rcWork),250,// RECT_HEIGHT(rcWork),
					m_hWnd, 0, WS_POPUP);
			// set the animation of the window
			dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_BOTTOM_TO_TOP_2);
			dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_TOP_TO_BOTTOM_1);
			int ret = dlg.DoModal();
			if(ret == ID_OK){
				_StartExportDate = dlg.getDateFrom();
				_EndExportDate = dlg.getDateTo();
				_bExportAll = dlg.getSelection();
				updateButtonRange();
				//TODO: save configuration
			}
			break;
		}
        case MZ_IDC_TOOLBAR_CONFIG:
        {
            int nIndex = lParam;
            if (nIndex == 0) {
                // exit the modal dialog
                EndModal(ID_CANCEL);
                return;
            }
        }
    }
}