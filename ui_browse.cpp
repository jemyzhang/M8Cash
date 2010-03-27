#include "ui_browse.h"
#include "m8cash.h"
#include "ui_main.h"
#include "ui_dailyreport.h"

#include "ui_reminder.h"
#include "ui_filter.h"

#include <cMzCommon.h>
using namespace cMzCommon;

MZ_IMPLEMENT_DYNAMIC(Ui_BrowseWnd)
#define MZ_IDC_TOOLBAR_BROWSE 101
#define MZ_IDC_LIST 102
#define MZ_IDC_SUMMARYMODE 104
#define MZ_IDC_BUTTON_FILTER 105

#define MZ_IDC_BUTTON_EDIT 106
#define MZ_IDC_BUTTON_DEL 107

#define IDC_PPM_DELETE		101
#define IDC_PPM_EDIT		102
#define IDC_PPM_CANCEL		103

void RecordList::setupList(int* i) { 
	idlist = i;
	int nSize = GetItemCount();
	plist_record = new CASH_TRANSACT_ptr[nSize];
	for(int i = 0; i < nSize; i++){
		CASH_TRANSACT_ptr p = new CASH_TRANSACT_t;
		cash_db.recordById(idlist[i],p);

		plist_record[i] = p;
	}
}
void RecordList::clearList(){
	if(plist_record){
		int nSize = GetItemCount();
		for(int i = 0; i < nSize; i++){
			CASH_TRANSACT_ptr p = plist_record[i];
			delete p;
		}
		plist_record = 0;
	}
	idlist = 0;
}


void RecordList::DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate) {
    // draw the text
	if(idlist == 0) return;

	// draw the high-light background for the selected item
    if (nIndex == GetSelectedIndex()) {
        MzDrawSelectedBg(hdcDst, prcItem);
    }

    wchar_t str[128];
    COLORREF cr = RGB(0,0,0);       
    HFONT hf;
	RECT rcText = *prcItem;
	RECT RectText;
	DWORD _browseMode = appconfig.IniBrowseMode.Get();
	DWORD _orderMode = appconfig.IniBrowseOrderMode.Get();

	int lineHeight = rcText.bottom - rcText.top;
	if(_browseMode == Ui_BrowsecfgWnd::BROWSE_DETAIL){
		lineHeight /= 2;
	}

	CASH_TRANSACT_ptr prec = plist_record[nIndex];
	//if(!cash_db.recordById(idlist[nIndex],&rec)) return;

	CASH_CATEGORY_ptr cat = cash_db.categoryById(prec->categoryid);
	CASH_ACCOUNT_ptr acnt = cash_db.accountById(prec->accountid);
	CASH_ACCOUNT_ptr toacnt = 0;
	if(cat->type == CT_TRANSFER){
		toacnt = cash_db.accountById(prec->toaccountid);
	}
	if(_browseMode == Ui_BrowsecfgWnd::BROWSE_DETAIL){
        ::SetTextColor( hdcDst , cr );
        hf = FontHelper::GetFont( 20 );
        SelectObject( hdcDst , hf );

		if(_orderMode == Ui_BrowsecfgWnd::ORDERBYACCOUNT){
			if(prec->isTransfer){
				wsprintf(str,L" %s→%s",acnt->name,toacnt->name);
			}else{
				wsprintf(str,L" %s",acnt->name);
			}
		}else{
			wsprintf(str,L" %s",cat->name);
		}
		//第一行第一列
		RectText.left = rcText.left;
		RectText.top = rcText.top;
		RectText.right = rcText.right - 300;
		RectText.bottom = rcText.top + lineHeight;
		MzDrawText( hdcDst , str, &RectText , DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );

		wsprintf(str,L"%d.%02d",
				prec->amount/100,prec->amount%100
		);
		if(cat->type == CT_INCOME){
			cr = RGB(64,128,64);
		}else if(cat->type == CT_OUTGOING){
			cr = RGB(128,64,64);
		}else{
			cr = RGB(64,64,128);
		}
        ::SetTextColor( hdcDst , cr );
		//第一行第二列
		RectText.left = rcText.right - 300;
		RectText.right = rcText.right - 180;
		MzDrawText( hdcDst , str, &RectText , DT_RIGHT|DT_SINGLELINE|DT_VCENTER );

		DeleteObject( hf );

		//font size change to 17

		//第二行

        cr = RGB(128,128,128);
        ::SetTextColor( hdcDst , cr );
        hf = FontHelper::GetFont( 17 );
        SelectObject( hdcDst, hf );

		if(_orderMode == Ui_BrowsecfgWnd::ORDERBYACCOUNT){
			wsprintf(str,L" %s<%s>",cat->name,cash_db.getPersonNameById(prec->personid));
		}else{
			if(prec->isTransfer){
				wsprintf(str,L" %s→%s",acnt->name,toacnt->name,cash_db.getPersonNameById(prec->personid));
			}else{
				wsprintf(str,L" %s<%s>",acnt->name,cash_db.getPersonNameById(prec->personid));
			}
		}
		//第二行第一列
		RectText.left = rcText.left;
		RectText.top = rcText.top + lineHeight;
		RectText.right = rcText.right - 300;
		RectText.bottom = rcText.bottom;
		MzDrawText( hdcDst , str, &RectText , DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );

        cr = RGB(128,128,128);
        ::SetTextColor( hdcDst , cr );

		//第二行第二列
		RectText.left = rcText.right - 300;
		RectText.right = rcText.right - 15;
		MzDrawText( hdcDst , prec->note , &RectText , DT_RIGHT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );
        DeleteObject(hf);

	}else if(_browseMode == Ui_BrowsecfgWnd::BROWSE_SIMPLE){
        ::SetTextColor( hdcDst , cr );
        hf = FontHelper::GetFont( 20 );
        SelectObject( hdcDst , hf );

		if(_orderMode == Ui_BrowsecfgWnd::ORDERBYACCOUNT){
			if(prec->isTransfer){
				wsprintf(str,L" %s→%s<%s>",acnt->name,toacnt->name,cash_db.getPersonNameById(prec->personid));
			}else{
				wsprintf(str,L" %s<%s>",acnt->name,cash_db.getPersonNameById(prec->personid));
			}
		}else{
			wsprintf(str,L" %s<%s>",cat->name,cash_db.getPersonNameById(prec->personid));
		}
		//第一行第一列
		RectText.left = rcText.left;
		RectText.top = rcText.top;
		RectText.right = rcText.right - 300;
		RectText.bottom = rcText.top + lineHeight;
		MzDrawText( hdcDst , str, &RectText , DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );

		wsprintf(str,L"%d.%02d",
				prec->amount/100,prec->amount%100
		);
		if(cat->type == CT_INCOME){
			cr = RGB(64,128,64);
		}else if(cat->type == CT_OUTGOING){
			cr = RGB(128,64,64);
		}else{
			cr = RGB(64,64,128);
		}
        ::SetTextColor( hdcDst , cr );
		RectText.left = rcText.right - 300;
		RectText.right = rcText.right - 180;
		MzDrawText( hdcDst , str, &RectText , DT_RIGHT|DT_SINGLELINE|DT_VCENTER );

		DeleteObject( hf );
	}
	//draw date
	//font size change to 17
    cr = RGB(128,128,128);
    ::SetTextColor( hdcDst , cr );
    hf = FontHelper::GetFont( 17 );
    SelectObject( hdcDst, hf );

	//第一行第三列
	RectText.left = rcText.right - 180;
	RectText.top = rcText.top;
	RectText.right = rcText.right - 15;
	RectText.bottom = rcText.top + lineHeight;
	CMzString dateonly = prec->date;
	MzDrawText( hdcDst , dateonly.SubStr(0,10).C_Str(), &RectText , DT_RIGHT|DT_SINGLELINE|DT_VCENTER );
	DeleteObject(hf);
}


Ui_BrowseWnd::Ui_BrowseWnd(){
	idarray = 0;
	sel_recordID = -1;
}

Ui_BrowseWnd::~Ui_BrowseWnd(){
}

BOOL Ui_BrowseWnd::OnInitDialog() {
        // Must all the Init of parent class first!
        if (!CMzWndEx::OnInitDialog()) {
            return FALSE;
        }

        // Then init the controls & other things in the window
		int y = 0;
		m_FilterBar.SetPos(0, y, GetWidth(), USER_DEF_HEIGHT*2);
		AddUiWin(&m_FilterBar);

		int w = 100;
		m_btnFilter.SetPos(0,y,w,USER_DEF_HEIGHT*2);
		m_btnFilter.SetText(LOADSTRING(IDS_STR_FILTER).C_Str());
		m_btnFilter.SetID(MZ_IDC_BUTTON_FILTER);
		m_FilterBar.AddChild(&m_btnFilter);

		m_StaticBar.SetPos(w,y,GetWidth() - w,USER_DEF_HEIGHT*2);
		m_StaticBar.SetEnable(false);
		m_StaticBar.SetTextSize(20);
		m_FilterBar.AddChild(&m_StaticBar);

		y+=USER_DEF_HEIGHT*2;
        m_RecordList.SetPos(0, y, GetWidth(), GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR - MZM_HEIGHT_HEADINGBAR*3);
        m_RecordList.SetID(MZ_IDC_LIST);
		m_RecordList.SetTextSize(m_RecordList.GetTextSize()*2/3);
		m_RecordList.SetItemHeight(40);
        m_RecordList.EnableScrollBarV(true);
        m_RecordList.EnableNotifyMessage(true);
        AddUiWin(&m_RecordList);

		m_summary.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR - MZM_HEIGHT_HEADINGBAR, GetWidth(), MZM_HEIGHT_HEADINGBAR);
        m_summary.SetID(MZ_IDC_SUMMARYMODE);
		m_summary.EnableNotifyMessage(true);
		AddUiWin(&m_summary);

        m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
		m_Toolbar.SetButton(1, true, true, LOADSTRING(IDS_STR_MORE).C_Str());
		m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_RETURN).C_Str());
		m_Toolbar.SetButton(2, true, true, LOADSTRING(IDS_STR_EDIT).C_Str());
        m_Toolbar.EnableLeftArrow(true);
        m_Toolbar.SetID(MZ_IDC_TOOLBAR_BROWSE);
        AddUiWin(&m_Toolbar);
		::SetTimer(m_hWnd,0x8001,100,NULL);	//先显示界面，再显示列表
		
        return TRUE;
}

LRESULT Ui_BrowseWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case MZ_WM_MOUSE_NOTIFY:
        {
            int nID = LOWORD(wParam);
            int nNotify = HIWORD(wParam);
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);

			//列表
			if (nID == MZ_IDC_LIST && nNotify == MZ_MN_LBUTTONUP) {
                if (!m_RecordList.IsMouseDownAtScrolling() && !m_RecordList.IsMouseMoved()) {
                    int nIndex = m_RecordList.CalcIndexOfPos(x, y);
					if(nIndex != -1 && nIndex == sel_recordID){
						Ui_DailyReportWnd dlg;
						RECT rcWork = MzGetWorkArea();
						dlg.setDetailIndex(idarray[m_RecordList.GetSelectedIndex()]);
						dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
								m_hWnd, 0, WS_POPUP);
						// set the animation of the window
						dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
						dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
						int nRet = dlg.DoModal();
					}
					sel_recordID = nIndex;
                    m_RecordList.SetSelectedIndex(nIndex);
					m_RecordList.Invalidate();
					if(nIndex == -1){
						m_Toolbar.SetButton(2,false,false,LOADSTRING(IDS_STR_EDIT).C_Str());
					}else{
						m_Toolbar.SetButton(2, true, true, LOADSTRING(IDS_STR_EDIT).C_Str());
					}
					m_Toolbar.Invalidate();
                }
                return 0;
            }
            if (nID == MZ_IDC_LIST && nNotify == MZ_MN_MOUSEMOVE) {
				sel_recordID = -1;
                m_RecordList.SetSelectedIndex(-1);
				m_RecordList.Invalidate();
				m_Toolbar.SetButton(2, false, false, LOADSTRING(IDS_STR_EDIT).C_Str());
				m_Toolbar.Invalidate();
                return 0;
            }
       }
    }
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

void Ui_BrowseWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
        UINT_PTR id = LOWORD(wParam);
        switch (id) {
			case MZ_IDC_BUTTON_FILTER:
			{
				Ui_FilterWnd dlg;
				RECT rcWork = MzGetWorkArea();
				dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
						m_hWnd, 0, WS_POPUP);
				// set the animation of the window
				dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
				dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
				int nRet = dlg.DoModal();
				if (nRet == ID_OK) {
					::SetTimer(m_hWnd,0x8001,100,NULL);	//延时刷新界面
				}
				break;
			}
            case MZ_IDC_TOOLBAR_BROWSE:
            {
                int nIndex = lParam;
				if (nIndex == 2) {
					// pop out a PopupMenu:
					CPopupMenu ppm;
					struct PopupMenuItemProp pmip;      

					pmip.itemCr = MZC_BUTTON_PELLUCID;
					pmip.itemRetID = IDC_PPM_CANCEL;
					pmip.str = LOADSTRING(IDS_STR_CANCEL).C_Str();
					ppm.AddItem(pmip);  

					pmip.itemCr = MZC_BUTTON_PELLUCID;
					pmip.itemRetID = IDC_PPM_DELETE;
					pmip.str = LOADSTRING(IDS_STR_DELETE).C_Str();
					ppm.AddItem(pmip);  

					pmip.itemCr = MZC_BUTTON_PELLUCID;
					pmip.itemRetID = IDC_PPM_EDIT;
					pmip.str = LOADSTRING(IDS_STR_EDIT).C_Str();
					ppm.AddItem(pmip);

					RECT rc = MzGetWorkArea();      
					rc.top = rc.bottom - ppm.GetHeight();
					ppm.Create(rc.left,rc.top,RECT_WIDTH(rc),RECT_HEIGHT(rc),m_hWnd,0,WS_POPUP);      
					int nID = ppm.DoModal();
					if (nID == IDC_PPM_DELETE){
						//delete record
						if(m_RecordList.GetSelectedIndex() == -1) return;
						//popup confirm
						if(MzMessageBoxEx(m_hWnd,LOADSTRING(IDS_STR_CONFIRM_DELETE).C_Str(),
							LOADSTRING(IDS_STR_OK).C_Str(),MZ_YESNO,false) == 1){
							cash_db.deleteTransactionById(idarray[m_RecordList.GetSelectedIndex()]);
							ReminderInfo_ptr reminder = 
								cash_reminder.getReminderByRecordId(idarray[m_RecordList.GetSelectedIndex()]);
							if(reminder){
								cash_reminder.deleteReminder(reminder);
								cash_reminder.saveReminderList();
							}
							updateUi();
						}
					}else if(nID == IDC_PPM_EDIT){
						Ui_MainWnd* pdlg = (Ui_MainWnd*)uiEditor;
						pdlg->setEditIndex(idarray[m_RecordList.GetSelectedIndex()]);
						EndModal(ID_OK);
						return;
					}
					return;
				}
                if (nIndex == 1) {
					Ui_BrowsecfgWnd dlg;
					RECT rcWork = MzGetWorkArea();
					dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
							m_hWnd, 0, WS_POPUP);
					// set the animation of the window
					dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_BOTTOM_TO_TOP_2);
					dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_TOP_TO_BOTTOM_1);
					int nRet = dlg.DoModal();
					if (nRet == ID_OK) {
						//...
						updateUi();
					}
                }
                if (nIndex == 0) {
					EndModal(ID_CANCEL);
					return;
                }
            }
        }
}

void Ui_BrowseWnd::updateUi(){
	//clear list
	m_RecordList.clearList();
	m_RecordList.RemoveAll();

	CMzString strBar;
	uRecordDate_t date_s;
	uRecordDate_t date_e;
	date_s.Value = appconfig.IniFilterStartDate.Get();
	date_e.Value = appconfig.IniFilterEndDate.Get();
	if(date_s.Value == 0) date_s.Value = cash_db.getMinTransactionDate().Value;
	if(date_e.Value == 0) date_e.Value = cash_db.getMaxTransactionDate().Value;

	if(date_s.Value == 0 && date_e.Value == 0){
		strBar = LOADSTRING(IDS_STR_ALLDATE).C_Str();
	}else{
		if(date_s.Value > date_e.Value) date_e.Value = date_s.Value;	//date_s大于maxRecordDate的情况
		wchar_t sdate[16];
		wsprintf(sdate,LOADSTRING(IDS_STR_YYYYMMDD).C_Str(),
			date_s.Date.Year,date_s.Date.Month,date_s.Date.Day);
		strBar = sdate;
		strBar = strBar + L"~";
		wsprintf(sdate,LOADSTRING(IDS_STR_YYYYMMDD).C_Str(),
			date_e.Date.Year,date_e.Date.Month,date_e.Date.Day);
		strBar = strBar + sdate;
	}
	strBar = strBar + L"\n";

	CASH_ACCOUNT_ptr filter_account = 0;
	CASH_CATEGORY_ptr filter_category = 0;
	CASH_PERSON_ptr filter_person = 0;

	int faid = (int)appconfig.IniFilterAccount.Get();
	int fcid = (int)appconfig.IniFilterCategory.Get();
	int pid = (int)appconfig.IniFilterPerson.Get();

	if(faid != -1){
		filter_account = cash_db.accountById(faid);
		strBar = strBar + filter_account->name;
	}else{
		strBar = strBar + LOADSTRING(IDS_STR_ALL_ACCOUNT);
	}
	strBar = strBar + L"/";
	if(fcid != -1){
		filter_category = cash_db.categoryById(fcid);
		strBar = strBar + filter_category->name;
	}else{
		strBar = strBar + LOADSTRING(IDS_STR_ALL_CATEGORY);
	}
	if(pid != -1){
		filter_person = cash_db.personById(pid);
		strBar = strBar + L"/" + filter_person->name;
	}
	m_StaticBar.SetText(strBar.C_Str());

	m_FilterBar.Invalidate();
	m_FilterBar.Update();
	if(date_s.Value == 0 && date_e.Value == 0){	//无数据
		//由于以上已经计算过数据日期范围，当全部为0时表示无数据
		m_summary.SetVisible(false);
		refreshUi();
		return;
	}
	//以上为界面设置

	DWORD _browseMode = appconfig.IniBrowseMode.Get();
	DWORD _orderMode = appconfig.IniBrowseOrderMode.Get();

	if(_browseMode == Ui_BrowsecfgWnd::BROWSE_DETAIL){	//详情
		m_RecordList.SetItemHeight(60);
	}else{
		m_RecordList.SetItemHeight(40);
	}

	//以上为list设置

	//以下计算数据及内容

    ListItem li;
    wchar_t str[256];

	cash_db.getTransactionsByDate_v2(&date_s.Date,&date_e.Date,
		_orderMode | Ui_BrowsecfgWnd::ORDERBYDATE);

	int tz = cash_db.list_search_record.size();

	if(idarray){
		delete[] idarray;
		idarray = 0;
	}

	if(tz <= 0){	//无数据状态
		m_summary.SetVisible(false);
		refreshUi();
		return;
	}
	MzBeginWaitDlg(m_hWnd,NULL,TRUE);

	idarray = new int[tz];
	int *p = idarray;

	double total_income = 0;
	double total_outgoing = 0;

	list<CASH_TRANSACT_ptr>::iterator i = cash_db.list_search_record.begin();

	for (;i != cash_db.list_search_record.end(); i++) {
		CASH_TRANSACT_ptr rec = *i;
		CASH_CATEGORY_ptr cat = cash_db.categoryById(rec->categoryid);

		if(filter_account && 
			(rec->accountid != filter_account->id) &&
			rec->toaccountid != filter_account->id){
				continue;
		}

		if(filter_category &&
			!(rec->categoryid == filter_category->id ||
			cash_db.isChildCategory(rec->categoryid,filter_category->id))){
				continue;
		}

		if(filter_person &&
			rec->personid != filter_person->id){
				continue;
		}

		if(cat->type == CT_INCOME){
			total_income += rec->amount;
		}else if(cat->type == CT_OUTGOING){
			total_outgoing += rec->amount;
		}else if(cat->type == CT_TRANSFER){
			if(filter_account){
				if(rec->accountid == filter_account->id){	//转出
					total_outgoing += rec->amount;
				}else if(rec->toaccountid == filter_account->id){ //转入
					total_income += rec->amount;
				}
			}
		}

        m_RecordList.AddItem(li);
		*(p++) = rec->transid;
    }
	m_RecordList.setupList(idarray);

	if(faid == -1 && fcid != -1){	//总计
		if(filter_category->type == CT_OUTGOING){
			wsprintf(str,L"%s: %.2f",
				LOADSTRING(IDS_STR_EXPENSE).C_Str(),
				total_outgoing/100);
		}else if(filter_category->type == CT_INCOME){
			wsprintf(str,L"%s: %.2f",
				LOADSTRING(IDS_STR_INCOME).C_Str(),
				total_income/100);
		}else if(filter_category->type == CT_TRANSFER){
			wsprintf(str,L"%s...",
				LOADSTRING(IDS_STR_TRANSFER).C_Str());
		}
	}else if(faid != -1 && fcid == -1){ //流入，流出
		wsprintf(str,LOADSTRING(IDS_STR_SUM_INOUT).C_Str(),
			total_income/100,
			total_outgoing/100,
			(total_income-total_outgoing)/100);
	}else{ //收入，支出
		wsprintf(str,LOADSTRING(IDS_STR_SUM_INEX).C_Str(),
			total_income/100,
			total_outgoing/100,(total_income-total_outgoing)/100);
	}

	m_RecordList.SetSelectedIndex(0);
	m_summary.SetVisible(true);
	m_summary.SetText(str);
	::MzEndWaitDlg();
	m_summary.Invalidate();
	m_RecordList.Invalidate();
}

void Ui_BrowseWnd::refreshUi(){
	//refresh ui
	m_summary.Invalidate();
	m_RecordList.Invalidate();
}
