#include "ui_report.h"
#include "m8cash.h"

MZ_IMPLEMENT_DYNAMIC(Ui_MonthReportWnd)

#define MZ_IDC_TOOLBAR_REPORT 101
#define MZ_IDC_TOOLBAR_MONTH 102

Ui_MonthReportWnd::Ui_MonthReportWnd(void)
{
	_mode = CT_OUTGOING;
	_viewAccount = false;
}

Ui_MonthReportWnd::~Ui_MonthReportWnd(void)
{
}

BOOL Ui_MonthReportWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 0;
	m_CaptionTitle.SetPos(0, y, GetWidth(),MZM_HEIGHT_CAPTION);
	AddUiWin(&m_CaptionTitle);

	y+=MZM_HEIGHT_TEXT_TOOLBAR;
	m_piegraph.SetPos(0,y,GetWidth(),200);
	AddUiWin(&m_piegraph);

	y+= 200;
	m_ScrollWin.SetPos(0, y, GetWidth(), GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR*2 - 200);
    m_ScrollWin.EnableScrollBarV(true);
    AddUiWin(&m_ScrollWin);

	m_table.SetPos(0,0,GetWidth(),25);
	m_table.setColumnTitle1(LOADSTRING(IDS_STR_CATEGORY).C_Str());
	if(_mode == CT_INCOME){
		m_table.setColumnTitle2(LOADSTRING(IDS_STR_INCOME).C_Str());
	}else{
		m_table.setColumnTitle2(LOADSTRING(IDS_STR_EXPENSE).C_Str());
	}
	m_ScrollWin.AddChild(&m_table);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_RETURN).C_Str());
    m_Toolbar.SetButton(2, true, true, LOADSTRING(IDS_STR_SWITCH).C_Str());
	m_Toolbar.EnableLeftArrow(true);
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_REPORT);
    AddUiWin(&m_Toolbar);

    return TRUE;
}

void Ui_MonthReportWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
        case MZ_IDC_TOOLBAR_REPORT:
        {
            int nIndex = lParam;
			if(nIndex == 0){	//返回
				EndModal(ID_OK);
				return;
			}
			if(nIndex == 2){
				setupData(!_viewAccount);
				return;
			}
		}
	}
}

bool Ui_MonthReportWnd::setupData(bool _view){
	_viewAccount = _view;
	MzBeginWaitDlg(m_hWnd);
	if(_view){
		setupAccountData();
	}else{
		setupCategoryData();
	}
	updateUi();
	MzEndWaitDlg();
	return true;
}

void Ui_MonthReportWnd::setupCategoryData(){
	m_piegraph.resetData();
	m_table.resetItem();
	list<CASH_CATEGORY_ptr>::iterator i = cash_db.list_category.begin();

	int ncolor = 0;
	COLORREF* pcolor = m_piegraph.GetColorTable(ncolor);
	int cnt = 0;
	uRecordDate_t date_s;
	uRecordDate_t date_e;
	date_s.Value = appconfig.IniFilterStartDate.Get();
	date_e.Value = appconfig.IniFilterEndDate.Get();
	int f_personId = appconfig.IniFilterPerson.Get();

	for(; i != cash_db.list_category.end(); i++){
		int amount = 0;
		CASH_CATEGORY_ptr c = *i;
		if(c->type != _mode) continue;

		if(cash_db.getRecordsByCategory(c->id,&date_s.Date,&date_e.Date)){
			list<CASH_RECORD_ptr>::iterator s = cash_db.list_search_record.begin();
			for(;s != cash_db.list_search_record.end(); s++){
				CASH_RECORD_ptr r = *s;
				if(f_personId != -1 && r->personid != f_personId) continue;	//过滤人员
				amount += r->amount;
			}
			if(amount > 0){
				m_piegraph.appendItem(c->name,amount);
				m_table.appendItem(c->name,amount,pcolor[cnt%ncolor]);
				cnt ++;
			}
		}
		//Do Events
		MSG msg;  
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )  
		{  
			TranslateMessage(   &msg   );  
			DispatchMessage(   &msg   );  
		}
	}
	m_table.setColumnTitle1(LOADSTRING(IDS_STR_CATEGORY).C_Str());
}

void Ui_MonthReportWnd::setupAccountData(){
	m_table.resetItem();
	m_piegraph.resetData();
	list<CASH_ACCOUNT_ptr>::iterator i = cash_db.list_account.begin();

	int ncolor = 0;
	COLORREF* pcolor = m_piegraph.GetColorTable(ncolor);
	int cnt = 0;
	uRecordDate_t date_s;
	uRecordDate_t date_e;
	date_s.Value = appconfig.IniFilterStartDate.Get();
	date_e.Value = appconfig.IniFilterEndDate.Get();
	int f_personId = appconfig.IniFilterPerson.Get();

	for(; i != cash_db.list_account.end(); i++){
		int amount = 0;
		CASH_ACCOUNT_ptr c = *i;
		if(cash_db.getRecordsByAccount(c->id,&date_s.Date,&date_e.Date)){
			list<CASH_RECORD_ptr>::iterator s = cash_db.list_search_record.begin();
			for(;s != cash_db.list_search_record.end(); s++){
				CASH_RECORD_ptr r = *s;
				CASH_CATEGORY_ptr p = cash_db.categoryById(r->categoryid);
				if(p->type == _mode){	//收入 支出
					if(f_personId != -1 && r->personid != f_personId) continue;	//过滤人员
					amount += r->amount;
				}
			}
			if(amount > 0){
				m_piegraph.appendItem(c->name,amount);
				m_table.appendItem(c->name,amount,pcolor[cnt%ncolor]);
				cnt ++;
			}
		}
		//Do Events
		MSG msg;  
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )  
		{  
			TranslateMessage(   &msg   );  
			DispatchMessage(   &msg   );  
		}
	}
	m_table.setColumnTitle1(LOADSTRING(IDS_STR_ACCOUNT).C_Str());
}

void Ui_MonthReportWnd::updateUi(){
	wchar_t date[64];
	CMzString strBar;
	uRecordDate_t date_s;
	uRecordDate_t date_e;
	date_s.Value = appconfig.IniFilterStartDate.Get();
	date_e.Value = appconfig.IniFilterEndDate.Get();
	if(date_s.Value == 0) date_s.Value = cash_db.getMinRecordDate().Value;
	if(date_e.Value == 0) date_e.Value = cash_db.getMaxRecordDate().Value;

	if(date_s.Value == 0 && date_e.Value == 0){
		strBar = LOADSTRING(IDS_STR_ALLDATE).C_Str();
	}else{
		wchar_t sdate[16];
		wsprintf(sdate,LOADSTRING(IDS_STR_YYYYMMDD).C_Str(),
			date_s.Date.Year,date_s.Date.Month,date_s.Date.Day);
		strBar = sdate;
		strBar = strBar + L"~";
		wsprintf(sdate,LOADSTRING(IDS_STR_YYYYMMDD).C_Str(),
			date_e.Date.Year,date_e.Date.Month,date_e.Date.Day);
		strBar = strBar + sdate;
	}
	if(_mode == CT_INCOME){
		wsprintf(date,LOADSTRING(IDS_STR_MONTHLY_TITLE).C_Str(),
			LOADSTRING(IDS_STR_INCOME).C_Str(),strBar.C_Str());
	}else{
		wsprintf(date,LOADSTRING(IDS_STR_MONTHLY_TITLE).C_Str(),
			LOADSTRING(IDS_STR_EXPENSE).C_Str(),strBar.C_Str());
	}
	m_CaptionTitle.SetText(date);
	m_CaptionTitle.Invalidate();
	m_piegraph.Invalidate();
	m_piegraph.Update();
	CMzString tblCol2Name;	//第二列名称，加入人员名称
	if(_mode == CT_INCOME){
		tblCol2Name = LOADSTRING(IDS_STR_INCOME).C_Str();
	}else{
		tblCol2Name = LOADSTRING(IDS_STR_EXPENSE).C_Str();
	}
	int personid = appconfig.IniFilterPerson.Get();
	if(personid != -1){
		tblCol2Name = tblCol2Name + L"[";
		tblCol2Name = tblCol2Name + cash_db.getPersonNameById(personid);
		tblCol2Name = tblCol2Name + L"]";
	}
	m_table.setColumnTitle2(tblCol2Name.C_Str());
	m_ScrollWin.Invalidate();
}

void UiReportTable::PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
	UiWin::PaintWin(hdcDst,prcWin,prcUpdate);

	/*
	//先擦除
	HBRUSH myBrush = CreateSolidBrush(RGB(255-16,255-16,255-16));
	RECT rect;
	rect.top = 0;
	rect.left = 0;
	rect.bottom = prcWin->bottom - prcWin->top;
	rect.right = prcWin->right - prcWin->left;
	FillRect(hdcDst,&rect,myBrush);//画之前先擦除.
	*/
	if(list_name.size() == 0) return;


	HFONT hf;
	COLORREF cr;
	RECT RectTitle;
	RECT RectContentL;
	RECT RectContentR;
	RECT prc = *prcWin;
	RectTitle.left = prc.left + 20;
	RectTitle.right = prc.right - 180;
	RectTitle.top = prc.top;
	RectTitle.bottom = prc.top + 25;

	SetBkMode(hdcDst,TRANSPARENT);

	cr = RGB(128,128,128);
	::SetTextColor( hdcDst , cr );
	hf = FontHelper::GetFont( 20 );
	SelectObject( hdcDst, hf );
	
	MzDrawText( hdcDst , col1Text , &RectTitle , DT_LEFT|DT_SINGLELINE|DT_VCENTER );
	RectTitle.left = prc.right - 180;
	RectTitle.right = prc.right;
	RectTitle.top = prc.top;
	RectTitle.bottom = prc.top + 25;

	MzDrawText( hdcDst , col2Text , &RectTitle , DT_CENTER|DT_SINGLELINE|DT_VCENTER );
    DeleteObject(hf);

	cr = RGB(0,0,0);
	::SetTextColor( hdcDst , cr );
	hf = FontHelper::GetFont( 18 );
	SelectObject( hdcDst, hf );

	int lineHeight = 20;
	RectContentL.left = prc.left + 40;
	RectContentL.right = prc.right - 180;
	RectContentL.top = prc.top + 25;
	RectContentL.bottom = prc.top + 25 + lineHeight;

	RectContentR.left = prc.right - 300;
	RectContentR.right = prc.right - 40;
	RectContentR.top = prc.top + 25;
	RectContentR.bottom = prc.top + 25 + lineHeight;

	calPercent();//计算百分比

	list<wchar_t*>::iterator n = list_name.begin();
	list<int>::iterator v = list_val.begin();
	list<double>::iterator p = list_percent.begin();
	list<COLORREF>::iterator c = list_color.begin();
	wchar_t str[64];
	int cnt = 0;
	for(; n != list_name.end(); n++, v++, p++, c++){
		//第一列 彩框
		int ch = 16;
		int cw = 16;
		int cx = RectContentL.left - cw;
		int cy = RectContentL.top;
		RECT rect = {cx,cy,cx + cw - 2,cy + ch};
		Rectangle(hdcDst,rect.left,rect.top,rect.right,rect.bottom);
		RECT frect = {cx+1,cy + 1,cx + cw - 3,cy + ch - 1};
		HBRUSH bqbrush = CreateSolidBrush(*c);
		FillRect(hdcDst,&frect,bqbrush);

		//第二列 名称
		MzDrawText( hdcDst , *n , &RectContentL , DT_LEFT|DT_SINGLELINE|DT_VCENTER );
		//第三列 数值
		double amount = *v;
		amount /= 100;
		wsprintf(str,L"%.2f( %.2f%% )",amount,*p);
		MzDrawText( hdcDst , str , &RectContentR , DT_RIGHT|DT_SINGLELINE|DT_VCENTER );
		RectContentL.top += lineHeight;
		RectContentL.bottom += lineHeight;
		RectContentR.top += lineHeight;
		RectContentR.bottom += lineHeight;
	}
    DeleteObject(hf);
}


//年度收支对比图

MZ_IMPLEMENT_DYNAMIC(Ui_YearlyReportWnd)

#define MZ_IDC_TOOLBAR_YEAR 102

Ui_YearlyReportWnd::Ui_YearlyReportWnd(void)
{
	_year = -1;
}

Ui_YearlyReportWnd::~Ui_YearlyReportWnd(void)
{
}

BOOL Ui_YearlyReportWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 0;
	m_CaptionTitle.SetPos(0, y, GetWidth(),MZM_HEIGHT_CAPTION);
	AddUiWin(&m_CaptionTitle);

	y+=MZM_HEIGHT_CAPTION;
    m_DateBar.SetPos(0, y, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    m_DateBar.SetButton(0, true, true, LOADSTRING(IDS_STR_PREV_YEAR).C_Str());
    m_DateBar.SetButton(1, true, true, LOADSTRING(IDS_STR_THIS_YEAR).C_Str());
    m_DateBar.SetButton(2, true, true, LOADSTRING(IDS_STR_NEXT_YEAR).C_Str());
    m_DateBar.SetID(MZ_IDC_TOOLBAR_YEAR);
    AddUiWin(&m_DateBar);

	y+=MZM_HEIGHT_TEXT_TOOLBAR;
	m_bargraph.SetPos(0,y,GetWidth(),GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR - y);
	AddUiWin(&m_bargraph);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_RETURN).C_Str());
	m_Toolbar.EnableLeftArrow(true);
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_REPORT);
    AddUiWin(&m_Toolbar);
	setupDate();

    return TRUE;
}

void Ui_YearlyReportWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
        case MZ_IDC_TOOLBAR_REPORT:
        {
            int nIndex = lParam;
			if(nIndex == 0){	//返回
				EndModal(ID_OK);
				return;
			}
		}
        case MZ_IDC_TOOLBAR_YEAR:
        {
            int nIndex = lParam;
			int y = _year;
			if(nIndex == 0){	//上一年
				y--;
				setupDate(y);
				return;
			}
			if(nIndex == 1){
				setupDate();
				return;
			}
			if(nIndex == 2){	//下一年
				y++;
				setupDate(y);
				return;
			}
		}
	}
}

bool Ui_YearlyReportWnd::setupDate(int y){
	if(y == -1){ //current year
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);
		y = sysTime.wYear;
	}
	if(y == _year){
		return false;
	}
	_year = y;
	setupBarData();
	updateUi();
	return true;
}

void Ui_YearlyReportWnd::setupBarData(){
	m_bargraph.resetData();
	for(int i = 0; i < 12; i++){
		wchar_t mm[8];
		int in = 0;
		int out = 0;
		wsprintf(mm,L"%d",i+1);
		RECORDATE_t date;
		date.Year = _year;
		date.Month = i+1;
		date.Day = 0;
		cash_db.AccountInOutById(in,out,-1,&date,&date);
		m_bargraph.appendItem(mm,in,out);
	}
}

void Ui_YearlyReportWnd::updateUi(){
	MzBeginWaitDlg(m_hWnd);
	wchar_t date[32];
	wsprintf(date,LOADSTRING(IDS_STR_YEARLY_TITLE).C_Str(),_year);
	m_CaptionTitle.SetText(date);
	m_CaptionTitle.Invalidate();
	m_bargraph.Invalidate();
	m_bargraph.Update();
	MzEndWaitDlg();
}


//账户最近30天折线图
MZ_IMPLEMENT_DYNAMIC(Ui_AccountReportWnd)

#define MZ_IDC_TOOLBAR_ACCOUNT_REPORT 101

Ui_AccountReportWnd::~Ui_AccountReportWnd(void)
{
	rotateScreen(1);
}

BOOL Ui_AccountReportWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 0;
	m_CaptionTitle.SetPos(0, y, GetWidth(),MZM_HEIGHT_CAPTION);
	AddUiWin(&m_CaptionTitle);

	y+=MZM_HEIGHT_CAPTION;
	m_linegraph.SetPos(0,y,GetWidth(),GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR_w720 - y);
	AddUiWin(&m_linegraph);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR_w720, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR_w720);
    m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_RETURN).C_Str());
	m_Toolbar.EnableLeftArrow(true);
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_ACCOUNT_REPORT);
	m_Toolbar.SetTextBarType(TEXT_TOOLBAR_TYPE_720);
    AddUiWin(&m_Toolbar);

    return TRUE;
}

void Ui_AccountReportWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
        case MZ_IDC_TOOLBAR_ACCOUNT_REPORT:
        {
            int nIndex = lParam;
			if(nIndex == 0){	//返回
				EndModal(ID_OK);
				//rotateScreen(1);
				return;
			}
		}
	}
}

LRESULT Ui_AccountReportWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
	/*
    switch (message) {
        case WM_DISPLAYCHAGE:
        {
            int nID = LOWORD(wParam);
            int nNotify = HIWORD(wParam);
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
       }
    }*/
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

void Ui_AccountReportWnd::setupAccountID(int id)
{ 
	_id = id;
//	reqUpdate = true;
//	if(reqUpdate){
//		reqUpdate = false;
		updateUi();
//	}
}

void Ui_AccountReportWnd::updateUi(){
	if(_id == -1) return;

	CASH_ACCOUNT_ptr account = cash_db.accountById(_id);
	wchar_t caption[128];
	wsprintf(caption,LOADSTRING(IDS_STR_30DAY).C_Str(),account->name);
	m_CaptionTitle.SetText(caption);
	m_CaptionTitle.Invalidate();

	SYSTEMTIME dt;
	GetLocalTime(&dt);
	LineGraphInfo_t in;
	LineGraphInfo_t out;
	LineGraphInfo_t bal;
	in.color = RGB(64,255,64);
	in.name = LOADSTRING(IDS_STR_IN).C_Str();
	out.color = RGB(255,64,64);
	out.name = LOADSTRING(IDS_STR_OUT).C_Str();
	bal.color = RGB(255,242,0);
	bal.name = LOADSTRING(IDS_STR_BALANCE).C_Str();

	RECORDATE_t date;

	//找到31天前的日期
	for(int i = 0; i < 30 ; i++){
		DateTime::OneDayDate(dt,true);
	}
	date.Year = dt.wYear;
	date.Month = dt.wMonth;
	date.Day = dt.wDay;
	int balance = cash_db.AccountBalanceById(_id,0,true,&date);

	for(int i = 0; i < 30 ; i++){
		int amount_in,amount_out;
		DateTime::OneDayDate(dt);
		date.Year = dt.wYear;
		date.Month = dt.wMonth;
		date.Day = dt.wDay;
		cash_db.AccountInOutById(amount_in,amount_out,_id,&date,&date);
		in.data.push_back(amount_in);
		out.data.push_back(amount_out);
		balance = balance + amount_in - amount_out;
		bal.data.push_back(balance);
		wchar_t dateTime[16];
		wsprintf(dateTime,L"%d.%d",dt.wMonth,dt.wDay);
		m_linegraph.m_xnames.push_back(dateTime);
		//Do Events
		MSG msg;  
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )  
		{  
			TranslateMessage(   &msg   );  
			DispatchMessage(   &msg   );  
		}
	}
	m_linegraph.appendItem(&in);
	m_linegraph.appendItem(&out);
	m_linegraph.appendItem(&bal);

	m_linegraph.Invalidate();
	m_linegraph.Update();
}

bool Ui_AccountReportWnd::rotateScreen(DWORD dwRotaion)
{
        DEVMODE settings;
        memset(&settings, 0, sizeof(DEVMODE));
        settings.dmSize = sizeof(DEVMODE);

        settings.dmFields = DM_DISPLAYORIENTATION;
        ChangeDisplaySettingsEx(NULL, &settings, NULL, CDS_TEST, NULL);
        DWORD rotation = settings.dmDisplayOrientation;
        if (dwRotaion == rotation)
                return false;

        settings.dmDisplayOrientation = dwRotaion;
        return (DISP_CHANGE_SUCCESSFUL == ChangeDisplaySettingsEx(NULL, &settings, NULL, 0, NULL));
}