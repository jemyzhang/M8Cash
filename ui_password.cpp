#include "ui_password.h"
#include "m8cash.h"
#include <cMzCommon.h>
using namespace cMzCommon;

MZ_IMPLEMENT_DYNAMIC(Ui_PasswordWnd)

#define MZ_IDC_TOOLBAR_CALENDAR 101
#define MZ_IDC_Edit_YEAR 102
#define MZ_IDC_Edit_MONTH 103
#define MZ_IDC_Edit_DAY 104

Ui_PasswordWnd::Ui_PasswordWnd(void)
{
	_uiMode = 0;
	_isModal = false;
}

Ui_PasswordWnd::~Ui_PasswordWnd(void)
{
	g_pwddlgshow = false;
}

void Ui_PasswordWnd::getPassword(wchar_t* p,int* plen){
	if(this->m_EdtPassword.GetText().IsEmpty()){
		*p = '\0';
		plen = 0;
	}
	wsprintf(p,L"%s",this->m_EdtPassword.GetText().C_Str());
	*plen = m_EdtPassword.GetText().Length();
}

void Ui_PasswordWnd::updateUi(){
	if(_uiMode){
		m_CaptionTitle.SetText(LOADSTRING(IDS_STR_SETUP_PWD).C_Str());
	    //m_EdtPassword.SetTip2(LOADSTRING(IDS_STR_NEW_PWD).C_Str());
	    m_EdtPassword.SetTip(LOADSTRING(IDS_STR_PWD_TIP).C_Str());
	}else{
		wchar_t name[128];
		wsprintf(name,L"%s v%s",LOADSTRING(IDS_STR_APPNAME).C_Str(),VER_STRING);
		m_CaptionTitle.SetText(name);
	    //m_EdtPassword.SetTip2(LOADSTRING(IDS_STR_PASSWORD).C_Str());
	}
}

BOOL Ui_PasswordWnd::OnInitDialog() {
	g_pwddlgshow = true;
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 0;
    m_CaptionTitle.SetPos(0, y, GetWidth(), MZM_HEIGHT_CAPTION);
    AddUiWin(&m_CaptionTitle);

	y+=MZM_HEIGHT_CAPTION*2;
	m_BtnKeyBoard.SetPos(5, y + 10, 40, 40);
	m_BtnKeyBoard.SetSlotWin(&m_EdtPassword);
	m_BtnKeyBoard.SetButtonImage(pimg[IDB_PNG_LOGIN - IDB_PNG_BEGIN]);
	
	m_EdtAmountMask.SetSlotWin(&m_EdtPassword);

	m_EdtPassword.SetMaskWin(&m_EdtAmountMask);
	m_EdtPassword.SetPos(5, y, GetWidth() - 10, MZM_HEIGHT_SINGLELINE_EDIT);
    m_EdtPassword.SetMaxChars(32);
    m_EdtPassword.SetSipMode(IM_SIP_MODE_GEL_LETTER, MZM_HEIGHT_TEXT_TOOLBAR);
    m_EdtPassword.SetLeftInvalid(50);
	if(_uiMode){
		//m_EdtPassword.E
	}else{
		m_EdtPassword.EnablePassWord();
	}
    AddUiWin(&m_EdtPassword);
	AddUiWin(&m_EdtAmountMask);
	AddUiWin(&m_BtnKeyBoard);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_CANCEL).C_Str());
    m_Toolbar.SetButton(2, true, true, LOADSTRING(IDS_STR_OK).C_Str());
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_CALENDAR);
    AddUiWin(&m_Toolbar);

	updateUi();
	//m_EdtPassword.SetFocus(true);
    return TRUE;
}

void Ui_PasswordWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
        case MZ_IDC_TOOLBAR_CALENDAR:
        {
            int nIndex = lParam;
			if(nIndex == 0){	//取消
				if(_uiMode){
					if(_isModal){
						EndModal(ID_CANCEL);
					}else{
						m_EdtPassword.SetText(L"\0");
						Show(false);
						g_pwddlgshow = false;
					}
				}else{
					EndModal(ID_CANCEL);
					m_EdtPassword.SetFocus(false);
					PostQuitMessage(0);
				}
				return;
			}
			if(nIndex == 2){	//确定
				CMzStringW s = m_EdtPassword.GetText();
				if(_uiMode){
					if(s.IsEmpty()){
						g_bencypt = false;
						cash_db.encrypt(NULL,0);
						MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_PWD_CLEAR_S).C_Str(),2000);
					}else{
						g_bencypt = true;
						cash_db.encrypt(s.C_Str(),s.Length());
						MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_PWD_SET_S).C_Str(),2000);
					}
				}else{
                    bool ret = false;
					if(s.IsEmpty()){
                        ret = cash_db.checkpwd(NULL,0);
					}else{
						ret = cash_db.checkpwd(s.C_Str(),s.Length());
					}
					if(!ret){
						//popup error message
						//L"密码错误，或者数据库已损坏"
						cash_db.disconnect();
						MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_PWD_INCORRECT).C_Str(),2000);
						m_EdtPassword.SetFocus(true);
						return;
					}
				}
				if(_isModal){
					EndModal(ID_OK);
				}else{
					g_pwddlgshow = false;
					m_EdtPassword.SetText(L"\0");
					Show(false);
				}
				return;
			}
		}
	}
}
