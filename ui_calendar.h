#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include <list>

#define GRID_USE_UILIST 0
#if GRID_USE_UILIST
#pragma message("Grid:使用UiList")
#else
#pragma message("Grid:直接绘图")
#endif
class UiGrid : public UiWin
{
public:
	UiGrid(void);
	~UiGrid(void);
	virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate);
public:
	//if ret = false, no selection
	bool calcSelectedIndex(int x, int y,int &row, int &col);
	void setSelectedIndex(int row,int col);
	//if ret = false, no selection
	bool getSelectedIndex(int &row, int &col);
public:
	//if idx == -1, set all
	void setTextSize(int row,int col, int nSize);
	//if idx == -1 set all
	void setTextColor(int row,int col, COLORREF c);
	void setText(int row,int col, LPCTSTR text);
	int getTextSize(int row,int col);
	COLORREF getTextColor(int row,int col);
	LPCTSTR getText(int row,int col);
	void setSelectedBgColor(COLORREF c);
	void setSelectedTextColor(COLORREF c);
	void setGridAutoSize(bool);
	void setGridSize(int width, int height);
public:
	void setGrids(int nRow,int nCol);
	int getRowCount(void);
	int getColCount(void);
public:
	virtual void Update(){
		_reqUpdate = true;
	}
	virtual void SetPos(int x, int y, int w, int h, UINT flags=0);
private:
	typedef struct GridAttr{
		CMzString text;
		int textSize;
		COLORREF textColor;
		bool isSelected;
	}GridAttr_t,*GridAttr_ptr;
	GridAttr_ptr *_grids;
#if GRID_USE_UILIST
	UiButton *_colList;
#endif
private:
	COLORREF _selbg, _seltxt;
	int _rows,_cols;
	bool _isAutosize;
	int _gwidth, _gheight;
	bool _reqUpdate;
	int m_nMaxX;
	int m_nMaxY;
private:
	HDC pMemDC;             //定义内存DC指针
	HBITMAP pBitmap;        //定义内存位图指针
private:
	bool checkRange(int row, int col);
};

#define CALENDAR_USE_GRID 1
#if CALENDAR_USE_GRID
#pragma message("日历:使用日历方格")
#else
#pragma message("日历:直接输入")
#endif

class Ui_WeekBar : public UiStatic{
public:
	virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate);
};

class Ui_CalendarWnd : public CMzWndEx 
{
	MZ_DECLARE_DYNAMIC(Ui_CalendarWnd);
public:
	Ui_CalendarWnd(void);
	~Ui_CalendarWnd(void);
public:
    UiToolbar_Text m_Toolbar;
#if !CALENDAR_USE_GRID
    UiSingleLineEdit m_EdtCalendarYear;
    UiSingleLineEdit m_EdtCalendarMonth;
    UiSingleLineEdit m_EdtCalendarDay;
	UiCaption m_CaptionTitle;
#else
	UiCaption m_CaptionHeader;
	UiButton_Image m_BtnNext;
	UiButton_Image m_BtnPre;
	UiStatic m_YearMonth;
	Ui_WeekBar m_WeekBar;
	UiGrid m_Calendar;
#endif
public:
	CMzString getDate();
	void initDate(int y, int m, int d){
		_year = y; _month = m; _day = d;
	}
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
#if !CALENDAR_USE_GRID
	void initDateText();
	bool checkDateText()
#else
	void updateGrid();
private:
	int _year;
	int _month;
	int _day;
#endif
};
