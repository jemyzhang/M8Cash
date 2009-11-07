#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include "UiEditKeySwitcher.h"

class UiEditPassword :
	public UiSingleLineEdit
{
public:
	UiEditPassword(void) { pMaskLabel = 0; }
	~UiEditPassword(void) {}
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
private:
	UiEditKeySwitcher *pMaskLabel;
};

class Ui_PasswordWnd : public CMzWndEx 
{
	MZ_DECLARE_DYNAMIC(Ui_PasswordWnd);
public:
	Ui_PasswordWnd(void);
	~Ui_PasswordWnd(void);
public:
    UiToolbar_Text m_Toolbar;
    UiEditPassword m_EdtPassword;
	UiCaption m_CaptionTitle;
	UiEditKeySwitcher m_EdtAmountMask;
	UiEditKeySwitchButton m_BtnKeyBoard;
public:
	void getPassword(wchar_t* p,int* plen);
	void setMode(int m) { _uiMode = m; }
	virtual int DoModal(){
		_isModal = true;
		this->m_EdtPassword.SetFocus(true);
		return CMzWndEx::DoModal();
	}
	void Show(bool bShow=true, bool bBringTop=true){
		_isModal = false;
		this->m_EdtPassword.SetFocus(false);
		CMzWndEx::Show(bShow,bBringTop);
		return;
	}
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
//    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	int _uiMode;	//0: enter password, 1: setup password
	void updateUi();
	bool _isModal;
};
