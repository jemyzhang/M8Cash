#include "ui_dailyreport.h"
#include "m8cash.h"
#include "ui_calendar.h"

#include "..\MzCommon\MzCommon.h"
using namespace MzCommon;

#define MZ_IDC_TOOLBAR_ABOUT 101
#define MZ_IDC_CAPTION_DATE 102
#define MZ_IDC_BUTTON_PRE_DATE 103
#define MZ_IDC_BUTTON_NEXT_DATE 104
#define MZ_IDC_LIST 105

void DailyRecordList::DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate) {
    // draw the text
	if(idlist == 0) return;

	// draw the high-light background for the selected item
    if (nIndex == GetSelectedIndex()) {
        MzDrawSelectedBg(hdcDst, prcItem);
    }

    wchar_t str[128];
    COLORREF cr = RGB(128,128,128);       
    HFONT hf;
	RECT rcText = *prcItem;
	RECT RectText;
	int lineHeight = rcText.bottom - rcText.top;
	lineHeight /= 2;

	CASH_RECORD_t rec;
	if(!cash_db.recordById(idlist[nIndex],&rec)) return;
	CASH_CATEGORY_ptr cat = cash_db.categoryById(rec.categoryid);
	CASH_ACCOUNT_ptr acnt = cash_db.accountById(rec.accountid);
	CASH_ACCOUNT_ptr toacnt = 0;
	if(cat->type == CT_TRANSFER){
		toacnt = cash_db.accountById(rec.toaccountid);
	}


    ::SetTextColor( hdcDst , cr );
    hf = FontHelper::GetFont( 17 );
    SelectObject( hdcDst , hf );

	CMzString cmzs;

	//第一行第一列
	cmzs = L"[";
	cmzs = cmzs + cat->name + L"]";
	if(rec.isTransfer){
		wsprintf(str,L"   [%s]→[%s]",acnt->name,toacnt->name);
	}else{
		wsprintf(str,L"   [%s]",acnt->name);
	}
	cmzs = cmzs + str;
	RectText.left = rcText.left + 10;
	RectText.top = rcText.top;
	RectText.right = rcText.right - 200;
	RectText.bottom = rcText.top + lineHeight;
	MzDrawText( hdcDst , cmzs.C_Str(), &RectText , DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );

	//第二行第一列
	RectText.left = rcText.left + 10;
	RectText.top = rcText.top + lineHeight;
	RectText.right = rcText.right - 200;
	RectText.bottom = rcText.bottom;
	MzDrawText( hdcDst , rec.note, &RectText , DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );

	//金额
	wsprintf(str,L"%.2f",	(double)rec.amount/100);
	if(cat->type == CT_INCOME){
		cr = RGB(64,128,64);
	}else if(cat->type == CT_OUTGOING){
		cr = RGB(128,64,64);
	}else{
		cr = RGB(64,64,128);
	}
    ::SetTextColor( hdcDst , cr );
    hf = FontHelper::GetFont( 25 );
    SelectObject( hdcDst , hf );
	//第一行第二列
	RectText.top = rcText.top;
	RectText.bottom = rcText.bottom;
	RectText.left = rcText.right - 200;
	RectText.right = rcText.right - 20;
	MzDrawText( hdcDst , str, &RectText , DT_RIGHT|DT_SINGLELINE|DT_VCENTER );

	DeleteObject( hf );
}

MZ_IMPLEMENT_DYNAMIC(Ui_DailyReportWnd);

Ui_DailyReportWnd::Ui_DailyReportWnd(void)
{
	_seletedIndex = 0;
	idarray = 0;
}

Ui_DailyReportWnd::~Ui_DailyReportWnd(void)
{
}

void Ui_DailyReportWnd::setDetailIndex(int i) { 
	_seletedIndex = i; 
	CASH_RECORD_t r;
	if(!cash_db.recordById(_seletedIndex,&r)) return;
	t = DateTime::StrToDate(r.date);
}

BOOL Ui_DailyReportWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 0;
	//日期
    m_CaptionDate.SetPos(0, y, GetWidth(), MZM_HEIGHT_CAPTION + 20);
	m_CaptionDate.SetID(MZ_IDC_CAPTION_DATE);
	m_CaptionDate.EnableNotifyMessage(true);
	AddUiWin(&m_CaptionDate);

#ifdef EN_IMAGE_BTN
	int bw = pimg[IDB_PNG_L_RRELEASE - IDB_PNG_CALC]->GetImageWidth();
	int bh = pimg[IDB_PNG_L_RRELEASE - IDB_PNG_CALC]->GetImageHeight();
	m_BtnLeft.SetPos(5, (m_CaptionDate.GetHeight() - bh)/2, bw, bh);
	m_BtnLeft.SetImage_Normal(pimg[IDB_PNG_L_LRELEASE - IDB_PNG_CALC]);
	m_BtnLeft.SetImage_Pressed(pimg[IDB_PNG_L_LPRESS - IDB_PNG_CALC]);
	m_BtnLeft.SetID(MZ_IDC_BUTTON_PRE_DATE);
	m_CaptionDate.AddChild(&m_BtnLeft);

	m_BtnRight.SetPos(m_CaptionDate.GetWidth() - bw, (m_CaptionDate.GetHeight() - bh)/2, bw, bh);
	m_BtnRight.SetImage_Normal(pimg[IDB_PNG_L_RRELEASE - IDB_PNG_CALC]);
	m_BtnRight.SetImage_Pressed(pimg[IDB_PNG_L_RPRESS - IDB_PNG_CALC]);
	m_BtnRight.SetID(MZ_IDC_BUTTON_NEXT_DATE);
	m_CaptionDate.AddChild(&m_BtnRight);
#endif

	y += MZM_HEIGHT_CAPTION + 20;
	//内容
    m_TextStatics.SetPos(0, y, GetWidth(), MZM_HEIGHT_SINGLELINE_EDIT*2/3);
	m_TextStatics.SetTextSize(20);
	m_TextStatics.SetEnable(false);
    AddUiWin(&m_TextStatics);

	y += MZM_HEIGHT_SINGLELINE_EDIT;
    m_CaptionList.SetPos(0, y, GetWidth(), MZM_HEIGHT_CAPTION);
	m_CaptionList.SetText(LOADSTRING(IDS_STR_TRANS_LIST).C_Str());
    AddUiWin(&m_CaptionList);

	y += MZM_HEIGHT_CAPTION;
    m_ListDetail.SetPos(0, y, GetWidth(), GetHeight() - y - MZM_HEIGHT_CAPTION/2 - MZM_HEIGHT_SINGLELINE_EDIT*3 - MZM_HEIGHT_TEXT_TOOLBAR);
	m_ListDetail.SetItemHeight(60);
	m_ListDetail.SetID(MZ_IDC_LIST);
	m_ListDetail.EnableNotifyMessage(true);
    AddUiWin(&m_ListDetail);

	y += m_ListDetail.GetHeight();
    m_CaptionDetail.SetPos(0, y, GetWidth(), MZM_HEIGHT_CAPTION);
	m_CaptionDetail.SetText(LOADSTRING(IDS_STR_TRANS_DETAIL).C_Str());
    AddUiWin(&m_CaptionDetail);

	y += MZM_HEIGHT_CAPTION;
    m_TextDetail.SetPos(0, y, GetWidth(), MZM_HEIGHT_SINGLELINE_EDIT*3);
	m_TextDetail.SetEnable(false);
    AddUiWin(&m_TextDetail);

	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_RETURN).C_Str());
    m_Toolbar.EnableLeftArrow(true);
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_ABOUT);
    AddUiWin(&m_Toolbar);

	updateDetail();
    return TRUE;
}

UINT weekdayID[] = {
	IDS_STR_WEEKDAY_NAME1,IDS_STR_WEEKDAY_NAME2,
	IDS_STR_WEEKDAY_NAME3,IDS_STR_WEEKDAY_NAME4,
	IDS_STR_WEEKDAY_NAME5,IDS_STR_WEEKDAY_NAME6,
	IDS_STR_WEEKDAY_NAME0
};

void Ui_DailyReportWnd::updateDetail(){
	wchar_t s[128];
	CMzString cmzs;
	wsprintf(s,LOADSTRING(IDS_STR_YYYYMMDD).C_Str(),t.wYear,t.wMonth,t.wDay);
	cmzs = s;
	cmzs = cmzs + L"[";
	cmzs = cmzs + LOADSTRING(weekdayID[ DateTime::getWeekDay(t.wYear,t.wMonth,t.wDay)]); 
	cmzs = cmzs + L"]";
	m_CaptionDate.SetText(cmzs.C_Str());

	//无数据的情况
	m_TextStatics.SetText(LOADSTRING(IDS_STR_NO_TRANSACTION).C_Str());
	m_ListDetail.RemoveAll();
	m_ListDetail.SetVisible(false);
	m_CaptionList.SetVisible(false);
	m_CaptionDetail.SetVisible(false);
	m_TextDetail.SetVisible(false);

	RECORDATE_t date;
	date.Year = t.wYear;
	date.Month = t.wMonth;
	date.Day = t.wDay;
	if(cash_db.getRecordsByDate(&date,&date)){
		int sz = cash_db.list_search_record.size();
		if(idarray){
			delete idarray;
			idarray = 0;
		}

		if(sz > 0){
			idarray = new int[sz];
			list<CASH_RECORD_ptr>::iterator i = cash_db.list_search_record.begin();
			int cnt = 0;
			double in = 0;
			double out = 0;
			for(; i != cash_db.list_search_record.end(); i++){
				CASH_RECORD_ptr r = *i;
				idarray[cnt++] = r->transid;
				m_ListDetail.AddItem(ListItem());
				CASH_CATEGORY_ptr c = cash_db.categoryById(r->categoryid);
				if(c->type == CT_INCOME){
					in += r->amount;
				}else if(c->type == CT_OUTGOING){
					out += r->amount;
				}
			}
			m_ListDetail.setupList(idarray);
			//今日收入，支出，盈余
			wsprintf(s,LOADSTRING(IDS_STR_SUM_INEX).C_Str(),in/100,out/100,(in-out)/100);
			cmzs = s;
			m_TextStatics.SetText(cmzs.C_Str());

			m_ListDetail.SetSelectedIndex(0);
			updateDetailText();
			m_ListDetail.SetVisible(true);
			m_CaptionList.SetVisible(true);
			m_CaptionDetail.SetVisible(true);
			m_TextDetail.SetVisible(true);
		}
	}
	m_CaptionList.Invalidate();
	m_CaptionList.Update();
	m_CaptionDetail.Invalidate();
	m_CaptionDetail.Update();
	m_CaptionDate.Invalidate();
	m_CaptionDate.Update();
	m_TextStatics.Invalidate();
	m_TextStatics.Update();
	m_ListDetail.Invalidate();
	m_ListDetail.Update();
}

void Ui_DailyReportWnd::updateDetailText(){
	if(idarray == 0) {
		m_TextDetail.SetText(L"");
		m_TextDetail.Invalidate();
		m_TextDetail.Update();
		return;
	}
	int idx = m_ListDetail.GetSelectedIndex();
	if(idx == -1){
		m_TextDetail.SetTextSize(30);
		m_TextDetail.SetText(LOADSTRING(IDS_STR_DAILY_TIP).C_Str());
		m_TextDetail.Invalidate();
		m_TextDetail.Update();
		return;
	}
	CASH_RECORD_t rec;
	if(!cash_db.recordById(idarray[idx],&rec)){
		m_TextDetail.SetTextSize(30);
		m_TextDetail.SetText(L"错误，不存在此记录");
		m_TextDetail.Invalidate();
		m_TextDetail.Update();
		return;
	}
	CASH_CATEGORY_ptr c = cash_db.categoryById(rec.categoryid);
	if(c == 0){
		m_TextDetail.SetTextSize(30);
		m_TextDetail.SetText(L"错误，不存在此记录的分类");
		m_TextDetail.Invalidate();
		m_TextDetail.Update();
		return;
	}
	wchar_t s[256];
	m_TextDetail.SetTextSize(20);
	//类别
	CMzString cmzs;
	//此类别今日共收入/支出 xx
	cmzs = LOADSTRING(IDS_STR_CATEGORY);
	cmzs = cmzs + L" [" + cash_db.getCategoryFullNameById(rec.categoryid) + L"]";
	if(rec.isTransfer){
		cmzs = cmzs + L"\n";
	}else{
		cmzs = cmzs + (c->type == CT_INCOME ? LOADSTRING(IDS_STR_INCOME) : LOADSTRING(IDS_STR_EXPENSE));
		cmzs = cmzs + L": ";
		double amount = 0;
		RECORDATE_t date;
		date.Year = t.wYear;
		date.Month = t.wMonth;
		date.Day = t.wDay;

		if(cash_db.getRecordsByDate(&date,&date)){
			list<CASH_RECORD_ptr>::iterator i = cash_db.list_search_record.begin();
			for(; i != cash_db.list_search_record.end(); i++){
				CASH_RECORD_ptr rs = *i;
				if(rs->categoryid == rec.categoryid || cash_db.isChildCategory(rs->categoryid,rec.categoryid)){
					amount += rs->amount;
				}
			}
		}
		wsprintf(s,L"%.2f\n",amount/100);
		cmzs = cmzs + s;
	}
	//账户
	cmzs = cmzs + LOADSTRING(IDS_STR_ACCOUNT);
	cmzs = cmzs + L"[" + cash_db.getAccountNameById(rec.accountid) + L"]\n";
	double in = 0;
	double out = 0;
	RECORDATE_t date;
	date.Year = t.wYear;
	date.Month = t.wMonth;
	date.Day = t.wDay;
	if(cash_db.getRecordsByAccount(rec.accountid,&date,&date)){
		list<CASH_RECORD_ptr>::iterator i = cash_db.list_search_record.begin();
		for(; i != cash_db.list_search_record.end(); i++){
			CASH_RECORD_ptr r = *i;
			if(r->isTransfer){	//转出
				out += r->amount;
			}else{
				CASH_CATEGORY_ptr c = cash_db.categoryById(r->categoryid);
				if(c->type == CT_INCOME){
					in += r->amount;
				}else{
					out += r->amount;
				}
			}
		}
	}
	if(cash_db.getRecordsByToAccount(rec.accountid,&date,&date)){
		list<CASH_RECORD_ptr>::iterator i = cash_db.list_search_record.begin();
		for(; i != cash_db.list_search_record.end(); i++){
			CASH_RECORD_ptr r = *i;
			in += r->amount;
		}
	}
	wsprintf(s,LOADSTRING(IDS_STR_SUM_INOUT).C_Str(),in/100,out/100,(in-out)/100);
	cmzs = cmzs + s;
	//此账户今日共流入，流出，差额，余额
	m_TextDetail.SetText(cmzs.C_Str());
	m_TextDetail.Invalidate();
	m_TextDetail.Update();
}

void Ui_DailyReportWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
		case MZ_IDC_BUTTON_PRE_DATE:
		{
			DateTime::OneDayDate(t,true);
			updateDetail();
			updateDetailText();
			break;
		}
		case MZ_IDC_BUTTON_NEXT_DATE:
		{
			DateTime::OneDayDate(t);
			updateDetail();
			updateDetailText();
			break;
		}
        case MZ_IDC_TOOLBAR_ABOUT:
        {
            int nIndex = lParam;
            if (nIndex == 0) {
                // exit the modal dialog
                EndModal(ID_OK);
                return;
            }
        }
    }
}

LRESULT Ui_DailyReportWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case MZ_WM_MOUSE_NOTIFY:
        {
            int nID = LOWORD(wParam);
            int nNotify = HIWORD(wParam);
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);

			//列表
			if (nID == MZ_IDC_LIST && nNotify == MZ_MN_LBUTTONDOWN) {
                if (!m_ListDetail.IsMouseDownAtScrolling() && !m_ListDetail.IsMouseMoved()) {
                    int nIndex = m_ListDetail.CalcIndexOfPos(x, y);
                    m_ListDetail.SetSelectedIndex(nIndex);
                    m_ListDetail.Invalidate();
                    m_ListDetail.Update();
					updateDetailText();
                }
                return 0;
            }
            if (nID == MZ_IDC_LIST && nNotify == MZ_MN_MOUSEMOVE) {
                m_ListDetail.SetSelectedIndex(-1);
                m_ListDetail.Invalidate();
                m_ListDetail.Update();
				updateDetailText();
                return 0;
            }

			if(nID == MZ_IDC_CAPTION_DATE && nNotify == MZ_MN_LBUTTONUP){	//双击 显示全部
//				GetLocalTime(&t);
				Ui_CalendarWnd calendardlg;
				RECT rcWork = MzGetWorkArea();
				calendardlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
						m_hWnd, 0, WS_POPUP);
				// set the animation of the window
				calendardlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
				calendardlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
				int nRet = calendardlg.DoModal();
				if (nRet == ID_OK) {
					t = DateTime::StrToDate(calendardlg.getDate().C_Str());
					updateDetail();
					updateDetailText();
				}
				return 0;
			}

       }
    }
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}
