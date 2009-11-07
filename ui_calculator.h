#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include <list>
#include "calcore.h"

class UiNumericDisplay : public UiWin
{
public:
	UiNumericDisplay(){
		_dotPos = 0;
		_isEmpty = true;
	}
	void setText1(LPCTSTR text) { _l1str = text; }
	void setText2(LPCTSTR text) { _l2str = text; }
	void removeChar1();
	void addChar1(wchar_t ch);
	void addChar2(wchar_t ch){ addChar(ch,_l2str);	}
	void addString1(LPCTSTR str){ addString(str,_l1str);	}
	void addString2(LPCTSTR str){ addString(str,_l2str);	}
	CMzString getText1() { return _l1str; }
	CMzString getText2() { return _l2str; }
	bool isNonInput(){
		return _isEmpty;
	}
	void setNonInput(){
		_isEmpty = true;
		_dotPos = 0;
	}
	void initNum(wchar_t ch){
		_isEmpty = false;
		wchar_t s[2] = {ch,0};
		_l1str = s;
	}
	void initVal(double val){
		wchar_t strval[64];
		wsprintf(strval,L"%.2f",val);
		_isEmpty = false;
		_l1str = strval;
	}
	void clearNum();
	double getInputval();
private:
	void addChar(wchar_t ch,CMzString& s){ 
		wchar_t tch[2];
		tch[0] = ch;
		tch[1] = 0;
		s = s + tch;
	}
	void addString(LPCTSTR str,CMzString& s){
		s = s + str;
	}
protected:
	void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
		//draw background
		MzDrawSelectedBg_NoLine(hdcDst,prcWin);

		COLORREF cr;       
		HFONT hf;
		RECT rect;
		rect.left = prcWin->left + 5;
		rect.right = prcWin->right - 5;
		LONG height = prcWin->bottom - prcWin->top;

		SetBkMode(hdcDst,TRANSPARENT);

		if(!_l1str.IsEmpty()){	//draw text of line 1
			cr = RGB(0,0,0);
			hf = FontHelper::GetFont( 25 );
			SelectObject( hdcDst , hf );
			rect.top = prcWin->top + height / 3;
			rect.bottom = rect.top + height * 2 / 3;
			MzDrawText( hdcDst, _l1str.C_Str(), &rect, DT_RIGHT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );
			DeleteObject( hf );
		}
		if(!_l2str.IsEmpty()){  //draw text of line2
			cr = RGB(128,128,128);
			hf = FontHelper::GetFont( 17 );
			SelectObject( hdcDst , hf );
			rect.top = prcWin->top;
			rect.bottom = rect.top + height / 3;
			MzDrawText( hdcDst, _l2str.C_Str(), &rect, DT_RIGHT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );
			DeleteObject( hf );
		}
	}
private:
	CMzString _l1str;
	CMzString _l2str;
	int _dotPos;
	bool _isEmpty;	//never input
};

typedef enum CALCBTNTYPE{
	CALBTN_NONE,
	CALBTN_NUM,
	CALBTN_OP,
	CALBTN_FUNC,
}CALCBTNTYPE_t;

typedef struct CALCBTNINFO{
	int ID;
	LPCTSTR text;
	CALCBTNTYPE_t atype;	//action type
	int hsize;	//blocks
	int vsize;	//blocks
}CALCBTNINFO_t;

#define MZ_MW_BTN_NUMERIC MZFC_WM_MESSAGE+0x80
#define MZ_MW_BTN_OPERATOR MZFC_WM_MESSAGE+0x81
#define MZ_MW_BTN_FUNCTION MZFC_WM_MESSAGE+0x82

class UiCalButton : public UiButton
{
public:
	UiCalButton(int ID = -1, LPCTSTR text = 0, CALCBTNTYPE_t t = CALBTN_NONE){
		if(ID > 0){
			SetID(ID);
		}
		if(text){
			SetText(text);
		}
		SetTextColor(RGB(0,0,0));
		_t = t;
	}
	void setActType(CALCBTNTYPE_t t){
		_t = t;
	}
	CALCBTNTYPE_t getActType(){
		return _t;
	}

	virtual int OnLButtonUp(UINT fwKeys, int xPos, int yPos){
		switch(_t){
			case CALBTN_NUM:
				PostMessageW(MZ_MW_BTN_NUMERIC,GetID(),0);
				break;
			case CALBTN_OP:
				PostMessageW(MZ_MW_BTN_OPERATOR,GetID(),0);
				break;
			case CALBTN_FUNC:
				PostMessageW(MZ_MW_BTN_FUNCTION,GetID(),0);
				break;
			default:
				break;
		}
		return UiButton::OnLButtonUp(fwKeys,xPos,yPos);
	}
private:
	CALCBTNTYPE_t _t;
};

class UiControlBG : public UiWin
{
public:
	UiControlBG(void){}
	~UiControlBG(void) {}
protected:
	virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
		UiWin::PaintWin(hdcDst, prcWin, prcUpdate);
		MzDrawGridPopupMenuBG_480(hdcDst,prcWin);
	}
};

class Ui_CalculatorWnd : public CMzWndEx 
{
	MZ_DECLARE_DYNAMIC(Ui_CalculatorWnd);
public:
	Ui_CalculatorWnd(void);
	~Ui_CalculatorWnd(void);
public:
	UiControlBG m_bg;
	UiNumericDisplay m_Display;
	UiCalButton *pm_Buttons[20];
public:
	double getResult() { return _result; };
	void setInitOprand(double val){
		_opndInput = false;
		m_Display.initVal(val);
		m_Display.setNonInput();
	}
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
private:
	double _result;
	CalcCore calc;
	bool _opPressed;
	bool _opndInput;
};
