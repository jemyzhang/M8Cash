#include "ui_calendar.h"
#include <MzCommon.h>
using namespace MzCommon;

#include "m8cash.h"

MZ_IMPLEMENT_DYNAMIC(Ui_CalendarWnd)

#define MZ_IDC_TOOLBAR_CALENDAR 101
#define MZ_IDC_Edit_YEAR 102
#define MZ_IDC_Edit_MONTH 103
#define MZ_IDC_Edit_DAY 104
#define MZ_IDC_CALENDAR_GRID 105
#define MZ_IDC_CALENDAR_NEXT 106
#define MZ_IDC_CALENDAR_PRE 107

UiGrid::UiGrid()
	:UiWin()
{
	_grids = 0;
	_selbg = RGB(64,192,192);
	_seltxt = RGB(255,255,255);
	_rows = 1;
	_cols = 1;
	setGridSize(_rows,_cols);
	_gwidth = 68;
	_gheight = 68;
	_reqUpdate = false;
	_isAutosize = false;
#if GRID_USE_UILIST
	_colList = 0;
#endif
}

UiGrid::~UiGrid(){
	setGridSize(0,0);	//release 
	if(pMemDC) ReleaseDC(GetParentWnd(),pMemDC);
	if(pBitmap) DeleteObject(pBitmap);
}

void UiGrid::setGridAutoSize(bool a){
	_isAutosize = a;
}

void UiGrid::setSelectedIndex(int row,int col){
	if(checkRange(row,col)){
		int oldr, oldc;
		if(getSelectedIndex(oldr,oldc)){
			_grids[oldr][oldc].isSelected = false;
		}
		_grids[row][col].isSelected = true;
	}
}

bool UiGrid::calcSelectedIndex(int x, int y, int &row, int &col){
	bool ret = true;
	int c = x / _gwidth;
	int r = y / _gheight;
	if(c > _cols || r > _rows){
		row = -1;
		col = -1;
		ret = false;
	}else{
		row = r;
		col = c;
	}
	return ret;
}

bool UiGrid::getSelectedIndex(int &row, int &col){
	bool ret = true;
		for(int i = 0; i < _rows; i++){
			for(int j = 0; j < _cols; j++){
				if(_grids[i][j].isSelected){
					row = i;
					col = j;
					return ret;
				}
			}
		}
	row = -1;
	col = -1;
	return false;
}

void UiGrid::setGridSize(int width, int height){
	_gwidth = width;
	_gheight = height;
}

void UiGrid::setGrids(int nRow, int nCol){
	if(_grids){
		for(int i = 0; i < _rows; i++){
			for(int j = 0; j < _cols; j++){
				_grids[i][j].text.SetBufferSize(0);
			}
			delete _grids[i];
		}
		delete _grids;
		_grids = 0;
	}
#if GRID_USE_UILIST
	if(_colList){
		delete[] _colList;
		_colList = 0;
	}
#endif
	_rows = nRow;
	_cols = nCol;
	if(_rows >= 0 && _cols >= 0){
		_grids = new GridAttr_ptr[_rows];
		for(int i = 0; i < _rows; i++){
			_grids[i] = new GridAttr_t[_cols];
			for(int j = 0; j < _cols; j++){
				_grids[i][j].isSelected = false;
				_grids[i][j].text = 0;
				_grids[i][j].textColor = RGB(0,0,0);
				_grids[i][j].textSize = 30;
			}
		}
	#if GRID_USE_UILIST
		_colList = new UiButton[_cols*_rows];
	#endif
	}


}

int UiGrid::getColCount(){
	return _cols;
}

int UiGrid::getRowCount(){
	return _rows;
}

void UiGrid::setText(int row,int col, LPCTSTR text){
	if(checkRange(row,col)){
		_grids[row][col].text = text;
	}
}

void UiGrid::setTextColor(int row,int col, COLORREF c){
	if(checkRange(row,col)){
		_grids[row][col].textColor = c;
	}
}

void UiGrid::setTextSize(int row,int col, int nSize){
	if(checkRange(row,col)){
		_grids[row][col].textSize = nSize;
	}
}

LPCTSTR UiGrid::getText(int row,int col){
	if(checkRange(row,col)){
		return _grids[row][col].text;
	}
	return 0;
}

COLORREF UiGrid::getTextColor(int row,int col){
	if(checkRange(row,col)){
		return _grids[row][col].textColor;
	}
	return RGB(0,0,0);
}

int UiGrid::getTextSize(int row,int col){
	if(checkRange(row,col)){
		return _grids[row][col].textSize;
	}
	return 0;
}

bool UiGrid::checkRange(int row, int col){
	if(row >= _rows || col >= _cols ||
		row < 0 || col < 0){
			return false;
	}
	return true;
}

void UiGrid::SetPos(int x, int y, int w, int h, UINT flags){
	UiWin::SetPos(x,y,w,h,flags);
	m_nMaxX = w;
	m_nMaxY = h;
#if GRID_USE_UILIST
#else
	pMemDC = CreateCompatibleDC(GetDC(GetParentWnd()));
	pBitmap = CreateCompatibleBitmap(GetDC(GetParentWnd()),m_nMaxX,m_nMaxY);
#endif
}

void UiGrid::PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
	UiWin::PaintWin(hdcDst,prcWin,prcUpdate);
#if GRID_USE_UILIST
	if(_reqUpdate){
		_reqUpdate = false;
		if(_rows <= 0 || _cols <= 0){
			return;
		}
		int _gridw = _gwidth;
		int _gridh = _gheight;
		if(_isAutosize){
			int _width = prcWin->right - prcWin->left;
			int _height = prcWin->bottom - prcWin->top;
			_gridw = _width/_cols;
			_gridh = _height/_rows;
		}
		for(int r = 0; r < _rows; r++){
			for(int c = 0; c < _cols; c++){
				_colList[c + r*_cols].SetPos((_gridw-1) * c,(_gridh-1)*r,_gridw,_gridh);
				_colList[c + r*_cols].SetText(_grids[r][c].text);
				_colList[c + r*_cols].SetTextColor(_grids[r][c].textColor);
				_colList[c + r*_cols].SetTextSize(_grids[r][c].textSize);
				_colList[c + r*_cols].SetButtonType(MZC_BUTTON_WHITE_RECT);
				_colList[c + r*_cols].SetButtonMode(MZC_BUTTON_MODE_HOLD);
				_colList[c + r*_cols].EnableClipExtend(true);
				_colList[c + r*_cols].SetClipExtend(2);
				if(_grids[r][c].isSelected){
					_colList[c + r*_cols].SetState(MZCS_BUTTON_PRESSED);
				}else{
					_colList[c + r*_cols].SetState(MZCS_BUTTON_NORMAL);
				}
				AddChild(_colList+c + r*_cols);
			}
		}
	}
#else
	if(_reqUpdate){
		_reqUpdate = false;
		SelectObject(pMemDC, pBitmap);
		HBRUSH myBrush = CreateSolidBrush(RGB(255-16,255-16,255-16));
		RECT rect;
		rect.top = 0;
		rect.bottom = m_nMaxY;
		rect.left = 0;
		rect.right = m_nMaxX;

		FillRect(pMemDC,&rect,myBrush);//画之前先擦除.
		//SetTextColor(RGB(0,0,0));

		//无数据
		if(_rows <= 0 || _cols <= 0){
			BitBlt(hdcDst,0,0,m_nMaxX,m_nMaxY,pMemDC,0,0,SRCCOPY);
			return;
		}
		int _gridw = _gwidth;
		int _gridh = _gheight;
		if(_isAutosize){
			int _width = prcWin->right - prcWin->left;
			int _height = prcWin->bottom - prcWin->top;
			_gridw = _width/_cols;
			_gridh = _height/_rows;
		}
		int _x = 0;//prcWin->left;
		int _y = 0;//prcWin->top;

		HFONT font = FontHelper::GetFont(_grids[0][0].textSize);
		SelectObject(pMemDC,font);
		HPEN pen = CreatePen(PS_SOLID, 0,RGB(128,128,128));
		HPEN poldpen = (HPEN)SelectObject(pMemDC,pen);
		SetBkMode(pMemDC,TRANSPARENT);
		for(int i = 0; i < _rows; i++){
			int cx = _x;
			int cy = _y;
			for(int j = 0; j < _cols; j++){
				//格子
				RECT rect = {cx + (_gridw-1)*j,cy + (_gridh-1)*i,cx + _gridw*j + _gridw,cy + _gridh*i + _gridh};
				Rectangle(pMemDC,rect.left,rect.top,rect.right,rect.bottom);
				//格子背景
				RECT frect = {rect.left+1,rect.top + 1,rect.right - 2,rect.bottom - 2};
				RECT textrect = {rect.left+1,rect.top + 1,rect.right - 2,rect.bottom - 2};
				HBRUSH bqbrush;
				if(_grids[i][j].isSelected){	//selected
					//bqbrush = CreateSolidBrush(_selbg);	//选中格子背景颜色
					//pen = CreatePen(PS_SOLID, 1,_seltxt);
					::SetTextColor(pMemDC,_seltxt);
					MzDrawSelectedBg_NoLine(pMemDC,&frect);
				}else{
					bqbrush = CreateSolidBrush(RGB(255-16,255-16,255-16));
					//pen = CreatePen(PS_SOLID, 1,_grids[i][j].textColor);
					::SetTextColor(pMemDC,_grids[i][j].textColor);
					FillRect(pMemDC,&frect,bqbrush);
				}
				//选择字体颜色
				//HPEN oldpen = (HPEN)SelectObject(pMemDC,pen);

				MzDrawText( pMemDC,_grids[i][j].text.C_Str(), &textrect, DT_CENTER|DT_VCENTER );
				//DrawText(pMemDC,_grids[i][j].text.C_Str(),_grids[i][j].text.Length(),&textrect,DT_CENTER|DT_VCENTER);
				//SelectObject(pMemDC,oldpen);//恢复笔
				//DeleteObject(pen);
			}
		}
		SelectObject(pMemDC,poldpen);
	}
	BitBlt(hdcDst,prcWin->left,prcWin->top,m_nMaxX,m_nMaxY,pMemDC,0,0,SRCCOPY);
#endif
}
//////

void Ui_WeekBar::PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
	int width = RECT_WIDTH(*prcWin)/7;
	RECT rect;
	rect.top = prcWin->top;
	rect.bottom = prcWin->bottom;

	HFONT font = FontHelper::GetFont(GetTextSize());
	SelectObject(hdcDst,font);	
	SetBkMode(hdcDst,TRANSPARENT);
	
	::SetTextColor(hdcDst,GetTextColor());
	for(int i = 0; i < 7; i++){
		rect.left = width * i;
		rect.right = width * (i + 1);
		MzDrawText( hdcDst,LOADSTRING(IDS_STR_WEEKDAY_NAME0 + i).C_Str(), &rect, DT_CENTER|DT_VCENTER );
	}
	DeleteObject(font);
	UiStatic::PaintWin(hdcDst,prcWin,prcUpdate);
}


Ui_CalendarWnd::Ui_CalendarWnd(void)
{
	_year = 0; _month = 0; _day = 0;
}

Ui_CalendarWnd::~Ui_CalendarWnd(void)
{
}
#if !CALENDAR_USE_GRID
void Ui_CalendarWnd::initDateText(){
    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);
    wchar_t s[5];
    wsprintf(s, L"%4d", sysTime.wYear);
	m_EdtCalendarYear.SetText(s);
	wsprintf(s, L"%2d", sysTime.wMonth);
	m_EdtCalendarMonth.SetText(s);
	wsprintf(s, L"%2d", sysTime.wDay);
	m_EdtCalendarDay.SetText(s);
}

bool Ui_CalendarWnd::checkDateText(){
	int year,month,day;
	year = _wtoi(m_EdtCalendarYear.GetText().C_Str());
	month = _wtoi(m_EdtCalendarMonth.GetText().C_Str());
	day = _wtoi(m_EdtCalendarDay.GetText().C_Str());
	int rc = MZ_CommonFunc::checkDate(year,month,day);
	if(rc&1) {
		m_EdtCalendarYear.SetTextColor(RGB(255,0,0));
	}else{
		m_EdtCalendarYear.SetTextColor(RGB(0,0,0));
	}
	if(rc&2) {
		m_EdtCalendarMonth.SetTextColor(RGB(255,0,0));
	}else{
		m_EdtCalendarMonth.SetTextColor(RGB(0,0,0));
	}
	if(rc&4) {
		m_EdtCalendarDay.SetTextColor(RGB(255,0,0));
	}else{
		m_EdtCalendarDay.SetTextColor(RGB(0,0,0));
	}

	m_EdtCalendarYear.Invalidate();
	m_EdtCalendarYear.Update();
	m_EdtCalendarMonth.Invalidate();
	m_EdtCalendarMonth.Update();
	m_EdtCalendarDay.Invalidate();
	m_EdtCalendarDay.Update();
	return (rc != 0);
}
#endif
CMzString Ui_CalendarWnd::getDate(){
	CMzString sDate;
#if !CALENDAR_USE_GRID
	int year,month,day;
	year = _wtoi(m_EdtCalendarYear.GetText().C_Str());
	month = _wtoi(m_EdtCalendarMonth.GetText().C_Str());
	day = _wtoi(m_EdtCalendarDay.GetText().C_Str());
	wsprintf(sDate.C_Str(), L"%4d-%02d-%02d", year, month, day);
#else
	wsprintf(sDate.C_Str(), L"%4d-%02d-%02d", _year, _month, _day);
#endif
	return sDate;
}

BOOL Ui_CalendarWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 0;
#if !CALENDAR_USE_GRID
    m_CaptionTitle.SetPos(0, y, GetWidth(), MZM_HEIGHT_CAPTION);
    m_CaptionTitle.SetText(L"选择日期");
    AddUiWin(&m_CaptionTitle);

	y+=MZM_HEIGHT_CAPTION;
    m_EdtCalendarYear.SetPos(0, y, GetWidth()/3, MZM_HEIGHT_SINGLELINE_EDIT);
    m_EdtCalendarYear.SetMaxChars(4);
    m_EdtCalendarYear.SetSipMode(IM_SIP_MODE_DIGIT, MZM_HEIGHT_TEXT_TOOLBAR);
    m_EdtCalendarYear.SetTip2(L"年",true);
    m_EdtCalendarYear.SetRightInvalid(40);
	m_EdtCalendarYear.SetID(MZ_IDC_Edit_YEAR);
	m_EdtCalendarYear.EnableNotifyMessage(true);
    AddUiWin(&m_EdtCalendarYear);

    m_EdtCalendarMonth.SetPos(GetWidth()/3, y, GetWidth()/3, MZM_HEIGHT_SINGLELINE_EDIT);
    m_EdtCalendarMonth.SetMaxChars(2);
    m_EdtCalendarMonth.SetSipMode(IM_SIP_MODE_DIGIT, MZM_HEIGHT_TEXT_TOOLBAR);
    m_EdtCalendarMonth.SetTip2(L"月",true);
    m_EdtCalendarMonth.SetRightInvalid(40);
	m_EdtCalendarMonth.SetID(MZ_IDC_Edit_MONTH);
	m_EdtCalendarMonth.EnableNotifyMessage(true);
    AddUiWin(&m_EdtCalendarMonth);

    m_EdtCalendarDay.SetPos(GetWidth()*2/3, y, GetWidth()/3, MZM_HEIGHT_SINGLELINE_EDIT);
    m_EdtCalendarDay.SetMaxChars(2);
    m_EdtCalendarDay.SetSipMode(IM_SIP_MODE_DIGIT, MZM_HEIGHT_TEXT_TOOLBAR);
    m_EdtCalendarDay.SetTip2(L"日",true);
    m_EdtCalendarDay.SetRightInvalid(40);
	m_EdtCalendarDay.SetID(MZ_IDC_Edit_DAY);
	m_EdtCalendarDay.EnableNotifyMessage(true);
    AddUiWin(&m_EdtCalendarDay);
#else
	m_CaptionHeader.SetPos(0,y,GetWidth(),MZM_HEIGHT_CAPTION * 2);
	AddUiWin(&m_CaptionHeader);

	m_YearMonth.SetPos(60,10,GetWidth()*3/4,MZM_HEIGHT_CAPTION);
	m_YearMonth.SetTextSize(30);
	m_CaptionHeader.AddChild(&m_YearMonth);


	int bw = pimg[IDB_PNG_L_RRELEASE - IDB_PNG_CALC]->GetImageWidth();
	int bh = pimg[IDB_PNG_L_RRELEASE - IDB_PNG_CALC]->GetImageHeight();

	m_BtnNext.SetPos(m_CaptionHeader.GetWidth() - bw, (m_CaptionHeader.GetHeight() - bh)/2 - 5, bw, bh);
	m_BtnNext.SetButtonType(MZC_BUTTON_NONE);
	m_BtnNext.SetImage_Normal(pimg[IDB_PNG_L_RRELEASE - IDB_PNG_CALC]);
	m_BtnNext.SetImage_Pressed(pimg[IDB_PNG_L_RPRESS - IDB_PNG_CALC]);
	m_BtnNext.SetID(MZ_IDC_CALENDAR_NEXT);
	m_CaptionHeader.AddChild(&m_BtnNext);

	m_BtnPre.SetPos(5, (m_CaptionHeader.GetHeight() - bh)/2 - 5, bw, bh);
	m_BtnPre.SetButtonType(MZC_BUTTON_NONE);
	m_BtnPre.SetImage_Normal(pimg[IDB_PNG_L_LRELEASE - IDB_PNG_CALC]);
	m_BtnPre.SetImage_Pressed(pimg[IDB_PNG_L_LPRESS - IDB_PNG_CALC]);
	m_BtnPre.SetID(MZ_IDC_CALENDAR_PRE);
	m_CaptionHeader.AddChild(&m_BtnPre);

	m_WeekBar.SetPos(0,MZM_HEIGHT_CAPTION*3/2,GetWidth(),MZM_HEIGHT_CAPTION/2);
	m_WeekBar.SetTextSize(17);
	m_WeekBar.SetTextColor(RGB(128,128,128));
	m_CaptionHeader.AddChild(&m_WeekBar);

	y+=MZM_HEIGHT_CAPTION * 2;
    m_Calendar.SetPos(2, y, GetWidth()-2, 69*6);
	m_Calendar.SetID(MZ_IDC_CALENDAR_GRID);
	m_Calendar.EnableNotifyMessage(true);
    AddUiWin(&m_Calendar);
#endif

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_CANCEL).C_Str());
    m_Toolbar.SetButton(2, true, true, LOADSTRING(IDS_STR_OK).C_Str());
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_CALENDAR);
    AddUiWin(&m_Toolbar);
#if !CALENDAR_USE_GRID
	initDateText();
#else
	updateGrid();
#endif
    return TRUE;
}

void Ui_CalendarWnd::updateGrid(){
	if(_year == 0 || _month == 0 || _day == 0){
		DateTime::getDate(&_year,&_month,&_day);
	}
	wchar_t datestr[16];
	wsprintf(datestr,LOADSTRING(IDS_STR_YYYYMM).C_Str(),_year,_month);
	m_YearMonth.SetText(datestr);
	m_YearMonth.Invalidate();
	int week = (DateTime::getWeekDay(_year,_month,1)+1)%7;	//获取1号的星期
	int days = DateTime::getDays(_year,_month);
	int rows = (week+days)/7 + ((week+days)%7 ? 1 : 0);
	m_Calendar.setGrids(rows,7);
	int t_year,t_month,t_day;
	DateTime::getDate(&t_year,&t_month,&t_day);
	for(int i = 0; i < days; i++){
		int r = (week + i) / 7;
		int c = (week + i) % 7;
		wsprintf(datestr,L"%d",i+1);
		m_Calendar.setText(r,c,datestr);
		if(t_year == _year && t_month == _month && t_day == i+1){
			m_Calendar.setTextColor(r,c,RGB(192,64,64));
		}
		if(_day == i+1){
			m_Calendar.setSelectedIndex(r,c);
		}
	}
	m_Calendar.Invalidate();
	m_Calendar.Update();
}

void Ui_CalendarWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
		case MZ_IDC_CALENDAR_NEXT:
			DateTime::getNextDate(_year,_month);
			_day = 1;
			updateGrid();
			break;
		case MZ_IDC_CALENDAR_PRE:
			DateTime::getPreDate(_year,_month);
			_day = 1;
			updateGrid();
			break;
        case MZ_IDC_TOOLBAR_CALENDAR:
        {
            int nIndex = lParam;
			if(nIndex == 0){	//取消
				EndModal(ID_CANCEL);
				return;
			}
			if(nIndex == 2){	//确定
#if !CALENDAR_USE_GRID
				if(checkDateText() == 0){
					EndModal(ID_OK);
					return;
				}
#else
				int r = 0;
				int c = 0;
				if( m_Calendar.getSelectedIndex(r,c)){
					//check if is invalid selection
					CMzString s = m_Calendar.getText(r,c);
					if(s.Length() == 0){
						return;
					}
					_day = _wtoi(s.C_Str());
					EndModal(ID_OK);
				}
				return;
#endif
			}
		}
	}
}

LRESULT Ui_CalendarWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
		case MZ_WM_MOUSE_NOTIFY:
		{
			int nID = LOWORD(wParam);
			int nNotify = HIWORD(wParam);
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            if (nID == MZ_IDC_CALENDAR_GRID && nNotify == MZ_MN_LBUTTONDOWN) {
                if (!m_Calendar.IsMouseDownAtScrolling() && !m_Calendar.IsMouseMoved()) {
					int r = 0;
					int c = 0;
					if( m_Calendar.calcSelectedIndex(x,y,r,c)){
						//check if is invalid selection
						CMzString s = m_Calendar.getText(r,c);
						if(s.Length() == 0){
							return 0;
						}
						m_Calendar.setSelectedIndex(r,c);
						m_Calendar.Invalidate();
						m_Calendar.Update();
					}
                }
                return 0;
            }
            if (nID == MZ_IDC_CALENDAR_GRID && nNotify == MZ_MN_LBUTTONDBLCLK) {
                if (!m_Calendar.IsMouseDownAtScrolling() && !m_Calendar.IsMouseMoved()) {
					int r = 0;
					int c = 0;
					if( m_Calendar.calcSelectedIndex(x,y,r,c)){
						//check if is invalid selection
						CMzString s = m_Calendar.getText(r,c);
						if(s.Length() == 0){
							return 0;
						}
						_day = _wtoi(s.C_Str());
						EndModal(ID_OK);
					}
                }
                return 0;
            }
			break;
		}
#if !CALENDAR_USE_GRID
		case MZ_WM_UI_FOCUS:
		{
            int nID = LOWORD(wParam);
            if (nID == MZ_IDC_Edit_YEAR || nID == MZ_IDC_Edit_MONTH || nID == MZ_IDC_Edit_DAY) {
				checkDateText();
                return 0;
            }
		}
#endif
    }
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}
