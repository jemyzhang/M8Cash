#include "ui_edit_account.h"
#include "ui_accounts.h"
#include "m8cash.h"
#include "..\MzCommon\MzCommon.h"
using namespace MzCommon;

#define MZ_IDC_TOOLBAR_ACOUNTEDIT 101

#define MZ_IDC_EDIT_ACCNAME 102
#define MZ_IDC_EDIT_ACCNOTE 103
#define MZ_IDC_EDIT_ACCINIT_YUAN 104
#define MZ_IDC_EDIT_ACCINIT_JIAO 105
#define MZ_IDC_BUTTON_CALCULATOR 112

MZ_IMPLEMENT_DYNAMIC(Ui_EditAccountWndEx)

BOOL Ui_EditAccountWndEx::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 0;
    m_ScrollWin.SetPos(0, 0, GetWidth(), GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR);
    m_ScrollWin.EnableScrollBarV(true);
    AddUiWin(&m_ScrollWin);

	m_CaptionAccTitle.SetPos(0,y,GetWidth(),MZM_HEIGHT_CAPTION);
	if(_mode == 0){
		m_CaptionAccTitle.SetText(LOADSTRING(IDS_STR_NEW_ACCOUNT).C_Str());
	}else{
		m_CaptionAccTitle.SetText(LOADSTRING(IDS_STR_EDIT_ACCOUNT).C_Str());
	}
	m_ScrollWin.AddChild(&m_CaptionAccTitle);

	y += MZM_HEIGHT_CAPTION;
    m_EdtAccName.SetPos(0, y, GetWidth(), MZM_HEIGHT_SINGLELINE_EDIT);
    m_EdtAccName.SetMaxChars(10);
    m_EdtAccName.SetSipMode(IM_SIP_MODE_GEL_PY, MZM_HEIGHT_TEXT_TOOLBAR);
    m_EdtAccName.SetTip2(LOADSTRING(IDS_STR_NAME).C_Str());
    m_EdtAccName.SetLeftInvalid(100);
    m_EdtAccName.SetID(MZ_IDC_EDIT_ACCNAME);
    m_ScrollWin.AddChild(&m_EdtAccName);

    y += MZM_HEIGHT_SINGLELINE_EDIT;

	y+=MZM_HEIGHT_CAPTION/2;
	m_CaptionAccInit.SetPos(0,y,GetWidth(),MZM_HEIGHT_CAPTION);
	m_CaptionAccInit.SetText(LOADSTRING(IDS_STR_INIT_AMOUNT).C_Str());
	m_ScrollWin.AddChild(&m_CaptionAccInit);

	y += MZM_HEIGHT_CAPTION;
	m_BtnKeyBoard.SetPos(5, y + 10, 40, 40);
	m_BtnKeyBoard.SetSlotWin(&m_EdtAccInitAmount);
	m_BtnKeyBoard.SetButtonImage(pimg[IDB_PNG_COINS - IDB_PNG_BEGIN]);

#ifdef EN_IMAGE_BTN
	m_BtnCalc.SetPos(GetWidth() - 70, y, 60, MZM_HEIGHT_SINGLELINE_EDIT);
	m_BtnCalc.SetButtonType(MZC_BUTTON_DOWNLOAD);
	m_BtnCalc.SetImage_Normal(pimg[IDB_PNG_CALC - IDB_PNG_CALC]);
	m_BtnCalc.SetImage_Pressed(pimg[IDB_PNG_CALC - IDB_PNG_CALC]);
	m_BtnCalc.SetID(MZ_IDC_BUTTON_CALCULATOR);
	m_ScrollWin.AddChild(&m_BtnCalc);
#endif

	m_EdtAmountMask.SetSlotWin(&m_EdtAccInitAmount);
	AddUiWin(&m_EdtAmountMask);

	m_EdtAccInitAmount.SetMaskWin(&m_EdtAmountMask);
	m_EdtAccInitAmount.SetPos(0, y, GetWidth() - 80, MZM_HEIGHT_SINGLELINE_EDIT);
    m_EdtAccInitAmount.SetTip2(LOADSTRING(IDS_STR_AMOUNT_UNIT).C_Str(),true);
    m_EdtAccInitAmount.SetRightInvalid(40);
    m_EdtAccInitAmount.SetLeftInvalid(50);
	m_EdtAccInitAmount.EnableMinus(true);
	m_EdtAccInitAmount.EnableTips(true);
    m_ScrollWin.AddChild(&m_EdtAccInitAmount);

	m_ScrollWin.AddChild(&m_BtnKeyBoard);

    y += MZM_HEIGHT_SINGLELINE_EDIT;
	m_CaptionAccNote.SetPos(0,y,GetWidth(),MZM_HEIGHT_CAPTION);
	m_CaptionAccNote.SetText(LOADSTRING(IDS_STR_NOTE).C_Str());
	m_ScrollWin.AddChild(&m_CaptionAccNote);

	y += MZM_HEIGHT_CAPTION;
    m_EdtAccNote.SetPos(0, y, GetWidth(), GetHeight() - y / 2);
    m_EdtAccNote.EnableScrollBarV(true);
    m_EdtAccNote.SetSipMode(IM_SIP_MODE_GEL_PY);
    m_EdtAccNote.SetID(MZ_IDC_EDIT_ACCNOTE);
    m_ScrollWin.AddChild(&m_EdtAccNote);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    if (_mode == 0) { //new
        m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_CANCEL).C_Str());
        m_Toolbar.SetButton(2, true, true, LOADSTRING(IDS_STR_SAVE).C_Str());
    } else {
		CASH_ACCOUNT_ptr acc = cash_db.accountById(_editIndex);
        m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_CANCEL).C_Str());
		if(!acc->isfix){
			m_Toolbar.SetButton(1, true, true, LOADSTRING(IDS_STR_DELETE).C_Str());
		}
        m_Toolbar.SetButton(2, true, true, LOADSTRING(IDS_STR_UPDATE).C_Str());
		m_EdtAccName.SetText(acc->name);
		m_EdtAccNote.SetText(acc->note);
		wchar_t temp[32];
		double amount = acc->initval;
		wsprintf(temp,L"%.2f",amount/100);
		m_EdtAccInitAmount.SetText(temp);
    }
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_ACOUNTEDIT);
    AddUiWin(&m_Toolbar);

    return TRUE;
}

void Ui_EditAccountWndEx::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
		case MZ_IDC_BUTTON_CALCULATOR:
			m_EdtAccInitAmount.ToggleCalc();
			break;
        case MZ_IDC_TOOLBAR_ACOUNTEDIT:
        {
            int nIndex = lParam;
            if (_mode == 0) { //new
                if (nIndex == 0) {	//取消
                    EndModal(ID_CANCEL);
                    return;
                }
                if (nIndex == 2) {	//确定
					//prepare account record
					CASH_ACCOUNT_t newAcc;

					if(m_EdtAccName.GetText().IsEmpty()){
						MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_WARN_ACCOUNT_NAME).C_Str());
						m_EdtAccName.SetFocus(true);
						return;
					}
					newAcc.name = m_EdtAccName.GetText().C_Str();
					if(!m_EdtAccNote.GetText().IsEmpty()){
						newAcc.note = m_EdtAccNote.GetText().C_Str();
					}else{
						newAcc.note = L"\0";
					}
					newAcc.initval = m_EdtAccInitAmount.getInputAmount();
					newAcc.isfix = 0;

					if(cash_db.checkDupAccount(&newAcc) != -1){
						MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_ERR_ACCOUNT_ADDDUP).C_Str());
						m_EdtAccName.SetText(L"\0");
						m_EdtAccName.Invalidate();
						m_EdtAccName.Update();
					}else{
						//insert account record
						cash_db.appendAccount(&newAcc);
						MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_INFO_ACCOUNT_ADDS).C_Str());
						EndModal(ID_OK);
					}
                    return;
                }
            } else {
                if (nIndex == 2) {	//确定
 					//prepare account record
					CASH_ACCOUNT_t updatedAcc;
					CASH_ACCOUNT_ptr pacc = cash_db.accountById(_editIndex);

					if(m_EdtAccName.GetText().IsEmpty()){
						MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_WARN_ACCOUNT_NAME).C_Str());
						m_EdtAccName.SetFocus(true);
						return;
					}
					//copy
					updatedAcc.id = pacc->id;
					updatedAcc.isfix = pacc->isfix;

					C::newstrcpy(&updatedAcc.name,m_EdtAccName.GetText().C_Str());
					if(!m_EdtAccNote.GetText().IsEmpty()){
						C::newstrcpy(&updatedAcc.note,m_EdtAccNote.GetText().C_Str());
					}else{
						updatedAcc.note = L"\0";
					}
					updatedAcc.initval = m_EdtAccInitAmount.getInputAmount();

					//insert account record
					//if name is changed, check duplication
					if(lstrcmp(updatedAcc.name,pacc->name) != 0){
						if(cash_db.checkDupAccount(&updatedAcc) != -1){
							MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_ERR_ACCOUNT_UPDATEDUP).C_Str());
							m_EdtAccName.SetText(L"\0");
							m_EdtAccName.Invalidate();
							m_EdtAccName.Update();
							return;
						}
					}

					cash_db.updateAccount(&updatedAcc);
					MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_INFO_ACCOUNT_UPDATES).C_Str());
                    EndModal(ID_OK);
                    return;
                }
                if (nIndex == 1) { //删除
					if(MzMessageBoxEx(m_hWnd,
						LOADSTRING(IDS_STR_WARN_ACCOUNT_DELETE).C_Str(),
						LOADSTRING(IDS_STR_OK).C_Str(),MZ_YESNO,false) == 1){
						if(cash_db.deleteAccountById(_editIndex)){
							MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_ERR_ACCOUNT_INUSE).C_Str());
						}
						EndModal(ID_OK);
					}
                    return;
                }
                if (nIndex == 0) { //取消
					EndModal(ID_CANCEL);
                    return;
                }
            }
        }
            break;
    }
}

LRESULT Ui_EditAccountWndEx::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case MZ_WM_MOUSE_NOTIFY:
        {
            int nID = LOWORD(wParam);
            int nNotify = HIWORD(wParam);
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            // process the mouse left button down notification
            if (nID == MZ_IDC_EDIT_ACCNOTE && nNotify == MZ_MN_LBUTTONDOWN) {
                if (!m_ScrollWin.IsMouseDownAtScrolling() && !m_ScrollWin.IsMouseMoved()) {
                    m_ScrollWin.ScrollTo(UI_SCROLLTO_BOTTOM);
                }
                return 0;
            }
        }
            return 0;
    }
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}