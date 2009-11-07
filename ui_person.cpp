#include "ui_person.h"
#include "ui_person_edit.h"
#include "resource.h"
#include "..\MzCommon\MzCommon.h"
using namespace MzCommon;

#define MZ_IDC_TOOLBAR_ACCOUNTS 101
#define MZ_IDC_PERSON_LIST 102
#define MZ_IDC_BTN_ALL	103
#define MZ_IDC_BTN_FAM	104
#define MZ_IDC_BTN_CON	105
#define MZ_IDC_BTN_INS	106

MZ_IMPLEMENT_DYNAMIC(Ui_PersonsWnd)

extern clsCASHDB cash_db;
extern ImagingHelper *imgArrow;
extern HINSTANCE LangresHandle;

void PersonList::DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate) {
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

	//第一块区域 显示图标 暂时不使用
	Rect01.left = rcText.left;
	Rect01.top = rcText.top;
	Rect01.right = rcText.left + 32;
	Rect01.bottom = rcText.bottom;
	//第二块区域 显示名称
	Rect02.left = Rect01.right + 10;
	Rect02.top = Rect01.top;
	Rect02.right = rcText.right - 20;
	Rect02.bottom = rcText.bottom;

	//图标
	//RECT rectImage = {prcItem->right - (imgArrow->GetImageWidth()<<1),prcItem->top,prcItem->right,prcItem->bottom};
	//imgArrow->Draw(hdcDst,&rectImage);
	//名称
	ListItem *pl = GetItem(nIndex);
	cr = RGB(0,0,0);
	::SetTextColor( hdcDst , cr );
	hf = FontHelper::GetFont( 30 );
	SelectObject( hdcDst , hf );
	MzDrawText( hdcDst , pl->Text , &Rect02 , DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );
	DeleteObject( hf );

}

BOOL Ui_PersonsWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
	int y = 0;
	m_Caption1.SetPos(0,y,GetWidth(),MZM_HEIGHT_CAPTION*2);
	AddUiWin(&m_Caption1);
	//m_Caption上的子按钮
	int x = 0;
	m_BtnAll.SetPos(0,0,GetWidth()/4,MZM_HEIGHT_CAPTION*2);
	m_BtnAll.SetText(LOADSTRING(IDS_STR_PERSON_ALL).C_Str());
	m_BtnAll.SetID(MZ_IDC_BTN_ALL);
	m_Caption1.AddChild(&m_BtnAll);

	x += GetWidth()/4;
	m_BtnFamily.SetPos(x,0,GetWidth()/4,MZM_HEIGHT_CAPTION*2);
	m_BtnFamily.SetText(LOADSTRING(IDS_STR_PERSON_FAMILY).C_Str());
	m_BtnFamily.SetID(MZ_IDC_BTN_FAM);
	m_Caption1.AddChild(&m_BtnFamily);

	x += GetWidth()/4;
	m_BtnContact.SetPos(x,0,GetWidth()/4,MZM_HEIGHT_CAPTION*2);
	m_BtnContact.SetText(LOADSTRING(IDS_STR_PERSON_CONTACT).C_Str());
	m_BtnContact.SetID(MZ_IDC_BTN_CON);
	m_Caption1.AddChild(&m_BtnContact);

	x += GetWidth()/4;
	m_BtnInstitution.SetPos(x,0,GetWidth()/4,MZM_HEIGHT_CAPTION*2);
	m_BtnInstitution.SetText(LOADSTRING(IDS_STR_PERSON_INSTITUTION).C_Str());
	m_BtnInstitution.SetID(MZ_IDC_BTN_INS);
	m_Caption1.AddChild(&m_BtnInstitution);

	y+=MZM_HEIGHT_CAPTION*2;
    m_List.SetPos(0, y, GetWidth(), GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR - MZM_HEIGHT_CAPTION);
    m_List.SetID(MZ_IDC_PERSON_LIST);
    m_List.EnableScrollBarV(true);
    m_List.EnableNotifyMessage(true);
    AddUiWin(&m_List);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_RETURN).C_Str());
    m_Toolbar.EnableLeftArrow(true);
	if(!_mode){
		m_Toolbar.SetButton(1, true, true, LOADSTRING(IDS_STR_NEW).C_Str());
		m_Toolbar.SetButton(2, false, true, LOADSTRING(IDS_STR_EDIT).C_Str());
	}
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_ACCOUNTS);
    AddUiWin(&m_Toolbar);

	updateList();
    return TRUE;
}

void Ui_PersonsWnd::setButtonStatus(CASH_PERSON_TYPE_t t){
	m_BtnAll.SetState(MZCS_BUTTON_NORMAL);//MZCS_BUTTON_PRESSED);
	m_BtnContact.SetState(MZCS_BUTTON_NORMAL);
	m_BtnFamily.SetState(MZCS_BUTTON_NORMAL);
	m_BtnInstitution.SetState(MZCS_BUTTON_NORMAL);
	switch(t){
		case PRSN_ALL:
			m_BtnAll.SetState(MZCS_BUTTON_PRESSED);
			break;
		case PRSN_FAMILY:
			m_BtnFamily.SetState(MZCS_BUTTON_PRESSED);
			break;
		case PRSN_CONTACT:
			m_BtnContact.SetState(MZCS_BUTTON_PRESSED);
			break;
		case PRSN_INSTITUTION:
			m_BtnInstitution.SetState(MZCS_BUTTON_PRESSED);
			break;
	}
	m_BtnAll.Invalidate();
	m_BtnFamily.Invalidate();
	m_BtnContact.Invalidate();
	m_BtnInstitution.Invalidate();
}

void Ui_PersonsWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
		case MZ_IDC_BTN_ALL:
			{
				if(_listpersonType != PRSN_ALL){
					_listpersonType = PRSN_ALL;
					updateList();
				}
				break;
			}
		case MZ_IDC_BTN_FAM:
			{
				if(_listpersonType != PRSN_FAMILY){
					_listpersonType = PRSN_FAMILY;
					updateList();
				}
				break;
			}
		case MZ_IDC_BTN_CON:
			{
				if(_listpersonType != PRSN_CONTACT){
					_listpersonType = PRSN_CONTACT;
					updateList();
				}
				break;
			}
		case MZ_IDC_BTN_INS:
			{
				if(_listpersonType != PRSN_INSTITUTION){
					_listpersonType = PRSN_INSTITUTION;
					updateList();
				}
				break;
			}
        case MZ_IDC_TOOLBAR_ACCOUNTS:
        {
            int nIndex = lParam;
            if (nIndex == 0) { //返回
                // exit the modal dialog
                EndModal(ID_CANCEL);
                return;
            }
            if (nIndex == 1) { //新建
				Ui_PersonEditWndEx dlg;
				RECT rcWork = MzGetWorkArea();
				dlg.setMode(0);
				if(_listpersonType == PRSN_ALL){
					dlg.setEditIndex(PRSN_FAMILY);
				}else{
					dlg.setEditIndex(_listpersonType);
				}
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
				Ui_PersonEditWndEx dlg;
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

LRESULT Ui_PersonsWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case MZ_WM_MOUSE_NOTIFY:
        {
            int nID = LOWORD(wParam);
            int nNotify = HIWORD(wParam);
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
			if (nID == MZ_IDC_PERSON_LIST && nNotify == MZ_MN_LBUTTONUP) {
				if (!m_List.IsMouseDownAtScrolling() && !m_List.IsMouseMoved()) {
					int nIndex = m_List.CalcIndexOfPos(x, y);
					if(nIndex != -1 && nIndex == _sel_idx){
						_selection = idarray[m_List.GetSelectedIndex()];
						EndModal(ID_OK);
						return 0;
					}
					_sel_idx = nIndex;
					m_List.SetSelectedIndex(nIndex);
					m_List.Invalidate();
					if(!_mode){
						if(nIndex != -1){
							m_Toolbar.SetButton(2, true, true, LOADSTRING(IDS_STR_EDIT).C_Str());
						}else{
							m_Toolbar.SetButton(2, false, true, LOADSTRING(IDS_STR_EDIT).C_Str());
						}
						m_Toolbar.Invalidate();
					}
				}
				return 0;
			}
            if (nID == MZ_IDC_PERSON_LIST && nNotify == MZ_MN_MOUSEMOVE) {
				_sel_idx = -1;
                m_List.SetSelectedIndex(-1);
                m_List.Invalidate();
				if(!_mode){
					m_Toolbar.SetButton(2, false, true, LOADSTRING(IDS_STR_EDIT).C_Str());
					m_Toolbar.Invalidate();
				}
               return 0;
            }
       }
    }
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

void Ui_PersonsWnd::updateList(){

	m_List.RemoveAll();

    ListItem li;
    CMzString str;

	//TODO: 获取人员列表
	cash_db.getPersonByType(_listpersonType);

	int tz = cash_db.list_search_person.size();
	if(idarray) delete[] idarray;

	idarray = new int[tz];

	int *p = idarray;

	list<CASH_PERSON_ptr>::iterator i = cash_db.list_search_person.begin();
	for (;i != cash_db.list_search_person.end(); i++) {
		CASH_PERSON_ptr psn = *i;
		li.Text = psn->name;
		li.Data = (void *)psn->type;
		m_List.AddItem(li);
		*(p++) = psn->id;
    }
	m_List.setupList(idarray);
	m_List.Invalidate();
	setButtonStatus(_listpersonType);
}
