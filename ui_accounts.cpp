#include "ui_accounts.h"
#include "ui_edit_account.h"
#include "m8cash.h"
#include "ui_report.h"

#define MZ_IDC_TOOLBAR_ACCOUNTS 101
#define MZ_IDC_ACCOUNT_LIST 102

MZ_IMPLEMENT_DYNAMIC(Ui_AccountsWnd)
void AccountList::DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate) {
    // draw the high-light background for the selected item
    if (nIndex == GetSelectedIndex()) {
        MzDrawSelectedBg(hdcDst, prcItem);
    }

    // draw the text
	if(idlist == 0) return;

	CASH_ACCOUNT_ptr a = cash_db.accountById(idlist[nIndex]);

    COLORREF cr = RGB(0,0,0);       
    HFONT hf;
	RECT rcText = *prcItem;
	RECT Rect01;
	RECT Rect02;
	RECT Rect03;

	int lineHeight = rcText.bottom - rcText.top;
	lineHeight /= 2;
	//第一块区域 第一行 显示账户名称
	Rect01.left = rcText.left + 10;
	Rect01.top = rcText.top;
	Rect01.right = rcText.right - 200;
	Rect01.bottom = rcText.top + lineHeight;
	//第二块区域 第一行 显示盈余
	Rect02.left = rcText.right - 200 - (imgArrow->GetImageWidth()<<2);
	Rect02.top = rcText.top;
	Rect02.right = rcText.right - 20;
	Rect02.bottom = rcText.top + lineHeight;
	//第三块区域 第二行 显示备注
	Rect03.left = rcText.left + 10;
	Rect03.top = rcText.top + lineHeight;
	Rect03.right = rcText.right - (imgArrow->GetImageWidth()<<1);
	Rect03.bottom = rcText.bottom;

	//账户名称
	cr = RGB(0,0,0);
	::SetTextColor( hdcDst , cr );
	hf = FontHelper::GetFont( 25 );
	SelectObject( hdcDst , hf );
	MzDrawText( hdcDst , a->name , &Rect01 , DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );
	DeleteObject( hf );
	//盈余
	hf = FontHelper::GetFont( 20 );

	SelectObject( hdcDst , hf );
	ListItem *item = GetItem(nIndex);
	if(item->Data != 0){
		cr = RGB(128,64,64);
	}else{
		cr = RGB(64,128,64);
	}
	::SetTextColor( hdcDst , cr );
	MzDrawText( hdcDst , item->Text.C_Str() , &Rect02 , DT_RIGHT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );

	//备注
	cr = RGB(128,128,128);
	::SetTextColor( hdcDst , cr );
	SelectObject( hdcDst , hf );
	MzDrawText( hdcDst , a->note , &Rect03 , DT_RIGHT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );

	DeleteObject( hf );

	RECT rectImage = {prcItem->right - (imgArrow->GetImageWidth()<<1),prcItem->top,prcItem->right,prcItem->bottom};
	imgArrow->Draw(hdcDst,&rectImage);
}

BOOL Ui_AccountsWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
	int y = 0;
	m_Caption1.SetPos(0,y,GetWidth(),MZM_HEIGHT_CAPTION);
	m_Caption1.SetText(LOADSTRING(IDS_STR_ACCOUNT_LIST).C_Str());
	AddUiWin(&m_Caption1);

	y+=MZM_HEIGHT_CAPTION;
    m_List.SetPos(0, y, GetWidth(), GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR - MZM_HEIGHT_CAPTION);
    m_List.SetID(MZ_IDC_ACCOUNT_LIST);
    m_List.EnableScrollBarV(true);
    m_List.EnableNotifyMessage(true);
    AddUiWin(&m_List);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_RETURN).C_Str());
    m_Toolbar.EnableLeftArrow(true);
	if(!_mode){
		m_Toolbar.SetButton(1, true, true, LOADSTRING(IDS_STR_NEW).C_Str());
		m_Toolbar.SetButton(2, true, true, LOADSTRING(IDS_STR_EDIT).C_Str());
	}
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_ACCOUNTS);
    AddUiWin(&m_Toolbar);

	updateList();
    return TRUE;
}

void Ui_AccountsWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
	Ui_EditAccountWndEx dlg;
    switch (id) {
        case MZ_IDC_TOOLBAR_ACCOUNTS:
        {
            int nIndex = lParam;
            if (nIndex == 0) { //返回
                // exit the modal dialog
                EndModal(ID_CANCEL);
                return;
            }
            if (nIndex == 1) { //新建
				RECT rcWork = MzGetWorkArea();
				dlg.setMode(0);
				dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
						m_hWnd, 0, WS_POPUP);
				// set the animation of the window
				dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_BOTTOM_TO_TOP_2);
				dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_TOP_TO_BOTTOM_1);
				int nRet = dlg.DoModal();
				if (nRet == ID_OK) {
					updateList();
				}
                return;
            }

            if (nIndex == 2) { //编辑
				RECT rcWork = MzGetWorkArea();
				dlg.setMode(1);
				dlg.setEditIndex(idarray[m_List.GetSelectedIndex()]);
				dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
						m_hWnd, 0, WS_POPUP);
				// set the animation of the window
				dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_BOTTOM_TO_TOP_2);
				dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_TOP_TO_BOTTOM_1);
				int nRet = dlg.DoModal();
				if (nRet == ID_OK) {
					updateList();
				}
                return;
            }
        }
    }
}

LRESULT Ui_AccountsWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case MZ_WM_MOUSE_NOTIFY:
        {
            int nID = LOWORD(wParam);
            int nNotify = HIWORD(wParam);
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            if (nID == MZ_IDC_ACCOUNT_LIST && nNotify == MZ_MN_LBUTTONUP) {
                if (!m_List.IsMouseDownAtScrolling() && !m_List.IsMouseMoved()) {
                    int nIndex = m_List.CalcIndexOfPos(x, y);
					if(nIndex != -1 && x > GetWidth() - (imgArrow->GetImageWidth()<<1)){
						_sel_idx = nIndex;
						m_List.SetSelectedIndex(nIndex);
						m_List.Invalidate();
						//显示折线图
						Ui_AccountReportWnd dlg;
						MzBeginWaitDlg(m_hWnd);
						UpdateWindow();
						dlg.setupAccountID(idarray[nIndex]);
						MzEndWaitDlg();
						UpdateWindow();
						dlg.rotateScreen(0);
						RECT rcWork = MzGetWorkArea();
						if(rcWork.top == 0) rcWork.top = 40;
						dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
								m_hWnd, 0, WS_POPUP);
						// set the animation of the window
//						dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_BOTTOM_TO_TOP_2);
//						dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_TOP_TO_BOTTOM_1);
						dlg.DoModal();
						dlg.rotateScreen(1);
					}else{
						if(nIndex != -1 && nIndex == _sel_idx){
							_selection = idarray[m_List.GetSelectedIndex()];
							EndModal(ID_OK);
							return 0;
						}
						_sel_idx = nIndex;
						m_List.SetSelectedIndex(nIndex);
						m_List.Invalidate();
					}
                }
                return 0;
            }
            if (nID == MZ_IDC_ACCOUNT_LIST && nNotify == MZ_MN_MOUSEMOVE) {
				_sel_idx = -1;
                m_List.SetSelectedIndex(-1);
                m_List.Invalidate();
                return 0;
            }
       }
    }
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

void Ui_AccountsWnd::updateList(){

	m_List.RemoveAll();

    ListItem li;
    CMzString str;

	int tz = cash_db.list_account.size();
	if(idarray) delete[] idarray;

	idarray = new int[tz];

	int *p = idarray;

	list<CASH_ACCOUNT_ptr>::iterator i = cash_db.list_account.begin();
	for (;i != cash_db.list_account.end(); i++) {
		CASH_ACCOUNT_ptr acc = *i;
		if(_hideAccountId == acc->id) continue;

		wchar_t str[128];
		double balance = cash_db.AccountBalanceById(acc->id);
		if(balance < 0){
			li.Data = (void*)1;
		}else{
			li.Data = 0;
		}
		wsprintf(str,LOADSTRING(IDS_STR_BALANCE1).C_Str(), balance/100);
		li.Text = str;
		
		m_List.AddItem(li);
		*(p++) = acc->id;
    }
	m_List.setupList(idarray);
}
