#pragma once
#include <mzfc_inc.h>
#include "ui_calculator.h"
#include "UiEditKeySwitcher.h"

class UiEditMoney :
	public UiSingleLineEdit
{
public:
	UiEditMoney(void);
	~UiEditMoney(void);
	void EnableMinus(bool e);
	void EnableTips(bool e);
	int getInputAmount();	//return ·Ö
	bool isEmpty();
	void SetText(LPCTSTR text);
	void ToggleCalc();
	virtual void SetEnable(bool bEnable=true){
		if(bEnable){
			if(pMaskLabel){
				pMaskLabel->SetVisible(false);
				pMaskLabel->Invalidate();
			}
			SetFocus(true);
		}else{
			if(pMaskLabel){
				pMaskLabel->SetVisible(true);
				pMaskLabel->Invalidate();
			}
			SetFocus(false);
		}
		UiSingleLineEdit::SetEnable(bEnable);
	}
	virtual void SetPos(int x, int y, int w, int h, UINT flags=0){
		UiSingleLineEdit::SetPos(x,y,w,h,flags);
		if(pMaskLabel){
			pMaskLabel->SetPos(x+32,y,w-32,h,flags);
			pMaskLabel->Invalidate();
		}
	}
	void SetMaskWin(UiEditKeySwitcher *pMask){
		pMaskLabel = pMask;
	}
protected:
	virtual int OnChar(TCHAR chCharCode, LPARAM lKeyData);
private:
	bool _enMinus;
	int _dotPos;
	Ui_CalculatorWnd *pcalcdlg;
	UiEditKeySwitcher *pMaskLabel;
};
