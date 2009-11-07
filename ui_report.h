#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include "ui_piegraph.h"
#include "ui_bargraph.h"
#include "cashdb.h"
#include "..\MzCommon\MzCommon.h"
using namespace MzCommon;

class UiReportTable : public UiWin {
public:
    // override the DrawItem member function to do your own drawing of the list
	UiReportTable():UiWin() { col1Text = 0; col2Text = 0;}
	~UiReportTable() { resetItem(); }
	void setColumnTitle1(wchar_t* title) { 
		if(col1Text) 
			delete[] col1Text;
		C::newstrcpy(&col1Text,title); 
	}
	void setColumnTitle2(wchar_t* title) { 
		if(col2Text)
			delete[] col2Text;
		C::newstrcpy(&col2Text,title); 
	}
	void appendItem(wchar_t* name, int val, COLORREF c) {
		//resize windows size
		int fz = GetTextSize();
		SetPos(0,0,GetWidth(),GetHeight() + 20,SP_NOMOVE);

		list_color.push_back(c);	//颜色按照顺序

		wchar_t* pname = 0;
		C::newstrcpy(&pname,name);
		list<int>::iterator ival = list_val.begin();
		list<wchar_t*>::iterator iname = list_name.begin();
		for(; ival != list_val.end(); ival++, iname++){
			if(val > *ival){
				list_val.insert(ival,val);
				list_name.insert(iname,pname);
				return;
			}
		}
		list_name.push_back(pname);  
		list_val.push_back(val);
	}
	void calPercent(){
		list<int>::iterator i = list_val.begin();
		double total = 0;
		for(;i != list_val.end(); i++){
			total += *i;
		}
		i = list_val.begin();
		for(;i != list_val.end(); i++){
			list_percent.push_back(((double)*i)*100/total);
		}
	}
	void resetItem() { 
		list<wchar_t*>::iterator i = list_name.begin();
		for(;i != list_name.end(); i++){
			delete [] *i;
		}
		list_name.clear(); 
		list_val.clear(); 
		list_percent.clear();
		list_color.clear();
		//resize windows size
		SetPos(0,0,GetWidth(),25,SP_NOMOVE);
	}
protected:
    void PaintWin(HDC hdc, RECT* prcWin, RECT* prcUpdate);
private:
	wchar_t* col1Text;
	wchar_t* col2Text;
	list<wchar_t*> list_name;
	list<int> list_val;
	list<double> list_percent;
	list<COLORREF> list_color;
};

class Ui_MonthReportWnd : public CMzWndEx 
{
	MZ_DECLARE_DYNAMIC(Ui_MonthReportWnd);
public:
	Ui_MonthReportWnd(void);
	~Ui_MonthReportWnd(void);
public:
	//-1 means current month
	//return false if there is no record
	bool setupData(bool);
	void setMode(CASH_TRANSACT_TYPE_t m) { _mode = m; setupData(_viewAccount);}
public:
	UiCaption m_CaptionTitle;
    UiScrollWin m_ScrollWin;
	UiToolbar_Text m_DateBar;
    UiToolbar_Text m_Toolbar;
	UiPieGraph m_piegraph;
	UiReportTable m_table;  
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
private:
	void setupCategoryData();
	void setupAccountData();

	void updateUi();
private:
	CASH_TRANSACT_TYPE_t _mode; //1: 收入报表, 0: 支出报表
	bool _viewAccount;
};

class Ui_YearlyReportWnd : public CMzWndEx 
{
	MZ_DECLARE_DYNAMIC(Ui_YearlyReportWnd);
public:
	Ui_YearlyReportWnd(void);
	~Ui_YearlyReportWnd(void);
public:
	//-1 means current month
	//return false if there is no record
	bool setupDate(int year = -1);
public:
	UiCaption m_CaptionTitle;
    UiScrollWin m_ScrollWin;
	UiToolbar_Text m_DateBar;
    UiToolbar_Text m_Toolbar;
	UiBarGraph m_bargraph;
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
private:
	void setupBarData();
	void updateUi();
private:
	int _year;
};

#include "ui_linegraph.h"
class Ui_AccountReportWnd : public CMzWndEx 
{
	MZ_DECLARE_DYNAMIC(Ui_AccountReportWnd);
public:
	Ui_AccountReportWnd(void){
		_id = -1;
		reqUpdate = false;
		//rotateScreen(0);

	}
	~Ui_AccountReportWnd(void);
public:
	void setupAccountID(int id);
	bool rotateScreen(DWORD dwRotaion);
public:
	UiCaption m_CaptionTitle;
    UiToolbar_Text m_Toolbar;
	UiLineGraph m_linegraph;
	/// called when received WM_PAINT
	virtual void OnPaint(HDC hdc, LPPAINTSTRUCT ps){
		rotateScreen(0);

		CMzWndEx::OnPaint(hdc,ps);
	}

	/// called when the window needs paint or update.
	virtual void PaintWin(HDC hdc, RECT* prcUpdate = NULL){
		CMzWndEx::PaintWin(hdc,prcUpdate);
	}

protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void updateUi();
private:
	int _id;
	bool reqUpdate;
};
