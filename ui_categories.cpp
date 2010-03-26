#include "ui_categories.h"
#include "ui_edit_category.h"
#include "m8cash.h"
#include <cMzCommon.h>
using namespace cMzCommon;

#define MZ_IDC_TOOLBAR_CATEGORIES 101
#define MZ_IDC_CATEGORY_LIST 102
#define MZ_IDC_BTN_LIST_SELECT 103

#define MZ_IDC_BTN_LIST_COLLEXPN 120

MZ_IMPLEMENT_DYNAMIC(Ui_CategoriesWnd)

void CategoryList::DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate) {
    // draw the high-light background for the selected item
    if (nIndex == GetSelectedIndex()) {
        MzDrawSelectedBg(hdcDst, prcItem);
		if(m_btn){
			m_btn->SetPos(prcItem->right - m_btn->GetWidth() - 10,
				prcItem->top + 5 - prcWin->top,
				0,0,SP_NOSIZE);
			m_btn->SetVisible(true);
			AddChild(m_btn);
		}
    }

    // draw the text
	if(idlist == 0) return;

	CASH_CATEGORY_ptr cat = cash_db.categoryById(idlist[nIndex].id);

	RECT rcText = *prcItem;
	int spacenum = cat->type == CT_TRANSFER ? 0 : cat->level;
	rcText.left = rcText.left + 40 + 30 * spacenum;
	MzDrawText( hdcDst , cat->name , &rcText , DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS );

	// setup button
	if(idlist[nIndex].btnType > 0){
		if(idlist[nIndex].m_btn == 0){
			UiButton_Image* btn = new UiButton_Image();
			int nResId = IDB_PNG_EXPANED + idlist[nIndex].btnType - 1 - IDB_PNG_BEGIN;
			btn->SetImage_Normal(pimg[nResId]);
			btn->SetImage_Pressed(pimg[nResId]);
//			btn->SetID(MZ_IDC_BTN_LIST_COLLEXPN + nIndex);
			AddChild(btn);
			idlist[nIndex].m_btn = btn;
		}
		idlist[nIndex].m_btn->SetPos(rcText.left - 5 - 16,
			prcItem->top + 17 - prcWin->top,
			16, 16);
		idlist[nIndex].m_btn->SetVisible(true);
	}

}

bool Ui_CategoriesWnd::hasChild(int nIndex){
	if(nidsz == 0) return false;

	int nId = nid_disp[nIndex].id;

	CASH_CATEGORY_ptr cat = cash_db.categoryById(nId);
	for(int i = 0; i < nidsz; i++){
		CASH_CATEGORY_ptr c_cat = cash_db.categoryById(idarray[i].id);
		
		if(cash_db.isChildCategory(c_cat->id,cat->id)){
			return true;
		}
	}
	return false;
}

void Ui_CategoriesWnd::hideChild(int nIndex){
	if(nidsz == 0) return;

	int nId = nid_disp[nIndex].id;

	CASH_CATEGORY_ptr cat = cash_db.categoryById(nId);
	for(int i = 0; i < nidsz; i++){
		CASH_CATEGORY_ptr c_cat = cash_db.categoryById(idarray[i].id);
		
		if(idarray[i].id == nId) idarray[i].btnType = 2;
		if(cash_db.isChildCategory(c_cat->id,cat->id)){
			if(idarray[i].btnType == 1){
				idarray[i].btnType = 2;
			}
			idarray[i].isShow = false;
		}
	}
	updateList();
}

void Ui_CategoriesWnd::showChild(int nIndex){
	if(nidsz == 0) return;

	int nId = nid_disp[nIndex].id;

	CASH_CATEGORY_ptr cat = cash_db.categoryById(nId);

	for(int i = 0; i < nidsz; i++){
		CASH_CATEGORY_ptr c_cat = cash_db.categoryById(idarray[i].id);
		if(idarray[i].id == nId) idarray[i].btnType = 1;
		if(c_cat->parentid == cat->id){
//			if(idarray[i].btnType == 2){
//				idarray[i].btnType = 1;
//			}
			idarray[i].isShow = true;
		}
	}
	updateList();
}

void Ui_CategoriesWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
	Ui_EditCateGoryWndEx dlg;
	/*
	if(id >= MZ_IDC_BTN_LIST_COLLEXPN){
		int nIndex = id - MZ_IDC_BTN_LIST_COLLEXPN;
		if(idarray[nIndex].btnType == 1) {//expand status
			hideChild(nIndex);
		}else{
			showChild(nIndex);
		}
		return;
	}
	*/
    switch (id) {
		case MZ_IDC_BTN_LIST_SELECT:
		{
			int nIndex = m_List.GetSelectedIndex();
			if(nIndex == -1) return;
			_selection = nid_disp[m_List.GetSelectedIndex()].id;
			EndModal(ID_OK);
			return;
		}
        case MZ_IDC_TOOLBAR_CATEGORIES:
        {
            int nIndex = lParam;
            if (nIndex == 0) { //返回
                // exit the modal dialog
                EndModal(ID_CANCEL);
                return;
            }
            if (nIndex == 1) { //新建
				// exit the modal dialog
				RECT rcWork = MzGetWorkArea();
				dlg.setMode(0);
				int selidx = nid_disp[m_List.GetSelectedIndex()].id;
				if(selidx == -1) selidx = 0;
				CASH_CATEGORY_ptr selcat = cash_db.categoryById(selidx);
				int selid = selcat->id;
				if(selcat->type == CT_TRANSFER){
					selid = 0;
				}else{
					if(selcat->level > 1){
						selid = selcat->parentid;
					}
				}
				dlg.setEditIndex(selid);
				dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
						m_hWnd, 0, WS_POPUP);
				// set the animation of the window
				dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_BOTTOM_TO_TOP_2);
				dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_TOP_TO_BOTTOM_1);
				int nRet = dlg.DoModal();
				if (nRet == ID_OK) {
					updateList(false);
				}
				//设置选中项目
				for(int i = 0; i < nidsz; i++){
					if(idarray[i].id == selid){
						_sel_idx = i;
						m_BtnSelect.SetVisible(true);
						m_List.SetSelectedIndex(i);
						m_List.Invalidate();
						return;
					}
				}
				_sel_idx = -1;
				m_BtnSelect.SetVisible(false);
				m_List.SetSelectedIndex(-1);
				m_List.Invalidate();
				return;
            }

            if (nIndex == 2) {	//编辑
				// exit the modal dialog
				if(m_List.GetSelectedIndex() < 0) return;

				int selid = nid_disp[m_List.GetSelectedIndex()].id;

				RECT rcWork = MzGetWorkArea();
				dlg.setMode(1);
				dlg.setEditIndex(selid);
				dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
						m_hWnd, 0, WS_POPUP);
				// set the animation of the window
				dlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_BOTTOM_TO_TOP_2);
				dlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_TOP_TO_BOTTOM_1);
				int nRet = dlg.DoModal();
				if (nRet == ID_OK) {
					updateList(false);
				}
				//设置选中项目
				for(int i = 0; i < nidsz; i++){
					if(idarray[i].id == selid){
						_sel_idx = i;
						m_BtnSelect.SetVisible(true);
						m_List.SetSelectedIndex(i);
						m_List.Invalidate();

						CASH_CATEGORY_ptr pcat = cash_db.categoryById(selid);
						if(pcat->level == 0 || (pcat->type == CT_TRANSFER) || _mode){
							m_Toolbar.SetButton(2,false,false,LOADSTRING(IDS_STR_EDIT).C_Str());
						}else{
							m_Toolbar.SetButton(2,true,true,LOADSTRING(IDS_STR_EDIT).C_Str());
						}
						m_Toolbar.Invalidate();
						return;
					}
				}
				_sel_idx = -1;
				m_BtnSelect.SetVisible(false);
				m_List.SetSelectedIndex(-1);
				m_List.Invalidate();

				m_Toolbar.SetButton(2,false,false,LOADSTRING(IDS_STR_EDIT).C_Str());
				m_Toolbar.Invalidate();
                return;
            }
        }
    }
}

BOOL Ui_CategoriesWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
	int y = 0;
	m_Caption1.SetPos(0,y,GetWidth(),MZM_HEIGHT_CAPTION);
	m_Caption1.SetText(LOADSTRING(IDS_STR_CATEGORY_LIST).C_Str());
	AddUiWin(&m_Caption1);

	y+=MZM_HEIGHT_CAPTION;
    m_List.SetPos(0, y, GetWidth(), GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR - MZM_HEIGHT_CAPTION);
	m_List.SetItemHeight(50);
	m_List.SetTextSize(25);
    m_List.SetID(MZ_IDC_CATEGORY_LIST);
    m_List.EnableScrollBarV(true);
    m_List.EnableNotifyMessage(true);
    AddUiWin(&m_List);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_RETURN).C_Str());
    m_Toolbar.EnableLeftArrow(true);
	if(!_mode){
		m_Toolbar.SetButton(1, true, true, LOADSTRING(IDS_STR_NEW).C_Str());
		m_Toolbar.SetButton(2, false, false, LOADSTRING(IDS_STR_EDIT).C_Str());
	}
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_CATEGORIES);
    AddUiWin(&m_Toolbar);

	updateList(false);
	int expendlv = appconfig.IniCategoryExpandLevel.Get();
	for(int i = 0; i < nidsz; i++){
		CASH_CATEGORY_ptr c_cat = cash_db.categoryById(idarray[i].id);
		
		if(c_cat->level == expendlv){
			for(int j = 0; j < nidsz_disp; j++){
				if(nid_disp[j].id == idarray[i].id && hasChild(j)){
					hideChild(j);
				}
			}
		}
	}
	updateList();

	m_BtnSelect.SetPos(0,0,60,45);
	m_BtnSelect.SetButtonType(MZC_BUTTON_DOWNLOAD);
	m_BtnSelect.SetText(LOADSTRING(IDS_STR_SELECT).C_Str());
	m_BtnSelect.SetVisible(false);
	m_BtnSelect.SetTextColor(RGB(64,64,128));
	m_BtnSelect.SetID(MZ_IDC_BTN_LIST_SELECT);
	//AddUiWin(&m_BtnSelect);
	m_List.setupButton(&m_BtnSelect);

    return TRUE;
}

LRESULT Ui_CategoriesWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case MZ_WM_MOUSE_NOTIFY:
        {
            int nID = LOWORD(wParam);
            int nNotify = HIWORD(wParam);
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            if (nID == MZ_IDC_CATEGORY_LIST && nNotify == MZ_MN_LBUTTONUP) {
                if (!m_List.IsMouseDownAtScrolling() && !m_List.IsMouseMoved()) {
                    int nIndex = m_List.CalcIndexOfPos(x, y);

					if(nIndex != -1 && x < GetWidth()/5){
						if(nid_disp[nIndex].btnType == 1) {//expand status
							hideChild(nIndex);
						}else if(nid_disp[nIndex].btnType == 2){
							showChild(nIndex);
						}
					}else{
						if(nIndex != -1 && nIndex == _sel_idx){
							_selection = nid_disp[m_List.GetSelectedIndex()].id;
							EndModal(ID_OK);
							return 0;
						}						
					}

					_sel_idx = nIndex;
					m_List.SetSelectedIndex(nIndex);
                    m_List.Invalidate();

					CASH_CATEGORY_ptr pcat = cash_db.categoryById(nid_disp[nIndex].id);
					if(pcat->level == 0 || (pcat->type == CT_TRANSFER) || _mode){
						m_Toolbar.SetButton(2,false,false,LOADSTRING(IDS_STR_EDIT).C_Str());
					}else{
						m_Toolbar.SetButton(2,true,true,LOADSTRING(IDS_STR_EDIT).C_Str());
					}
					m_Toolbar.Invalidate();
                }
                return 0;
            }
            if (nID == MZ_IDC_CATEGORY_LIST && nNotify == MZ_MN_MOUSEMOVE) {
				_sel_idx = -1;
                m_List.SetSelectedIndex(-1);
                m_List.Invalidate();
				m_BtnSelect.SetVisible(false);
				m_BtnSelect.Invalidate();
				m_Toolbar.SetButton(2,false,false,LOADSTRING(IDS_STR_EDIT).C_Str());
				m_Toolbar.Invalidate();
                return 0;
            }
		}
    }
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

void Ui_CategoriesWnd::sortList(idList *a, int size){
	int level0size = 0;
	int* level0array = new int[size];
	int level1size = 0;
	int* level1array = new int[size];
	int level2size = 0;
	int* level2array = new int[size];
	//先排序level0
	for(int cnt = 0; cnt < size; cnt++){
		CASH_CATEGORY_ptr key = cash_db.categoryById(a[cnt].id);
		if(key->level == 0){
			level0array[level0size++] = key->id;
		}
	}
	//排序level1
	for(int cnt = 0; cnt < size; cnt++){
		CASH_CATEGORY_ptr key = cash_db.categoryById(a[cnt].id);
		if(key->level == 1){
			level1array[level1size++] = key->id;
		}
	}
	//排序level2
	for(int cnt = 0; cnt < size; cnt++){
		CASH_CATEGORY_ptr key = cash_db.categoryById(a[cnt].id);
		if(key->level == 2){
			if(key->type == CT_TRANSFER){
				level0array[level0size++] = key->id;
			}else{
				level2array[level2size++] = key->id;
			}
		}
	}

	for(int i0 = 0; i0 < level0size; i0++){
		(*a++).id = level0array[i0];
		CASH_CATEGORY_ptr l0 = cash_db.categoryById(level0array[i0]);
		for(int i1 = 0; i1 < level1size; i1++){
			CASH_CATEGORY_ptr l1 = cash_db.categoryById(level1array[i1]);
			if(l1->parentid == l0->id){
				(*a++).id = level1array[i1];
				for(int i2 = 0; i2 < level2size; i2++){
					CASH_CATEGORY_ptr l2 = cash_db.categoryById(level2array[i2]);
					if(l2->parentid == l1->id){
						(*a++).id = level2array[i2];
					}
				}
			}
		}
	}
	delete[] level0array;
	delete[] level1array;
	delete[] level2array;
}

void Ui_CategoriesWnd::updateList(bool updateBtn){

	m_List.RemoveAll();

	int cnt = 0;
	
	if(updateBtn){
		if(nidsz == 0) return;

		int ntz = 0;
		for(int i = 0; i < nidsz; i++){
			if(idarray[i].isShow) ntz++;
		}
		if(nid_disp && nid_disp != idarray) delete[] nid_disp;
		nid_disp = new idList[ntz];

		for(int i = 0; i < nidsz; i++){
			if(idarray[i].isShow){
				nid_disp[cnt].id = idarray[i].id;
				nid_disp[cnt].isShow = true;
				nid_disp[cnt].btnType = idarray[i].btnType;
				nid_disp[cnt].m_btn = idarray[i].m_btn;
				cnt++;
			}
		}
//		delete idarray;
//		idarray = nid;
	}else{
		int tz = cash_db.list_category.size();
		if(idarray) delete[] idarray;

		idarray = new idList[tz];

		idList *p = idarray;
		for(int l = 0; l <= _maxdepth; l++){
			if(cash_db.getCategoryByLevel(l)){
				list<CASH_CATEGORY_ptr>::iterator cati = cash_db.list_search_category.begin();
				for(;cati != cash_db.list_search_category.end(); cati++){
					CASH_CATEGORY_ptr cat = *cati;
					if(cat->id != _hideCategoryId){
						p[cnt].id = cat->id;
						p[cnt].m_btn = 0;
						p[cnt].isShow = true;
						cnt++;
					}
				}
			}
		}
		sortList(idarray,cnt);	//按分类顺序整理
		nid_disp = idarray;
		nidsz = cnt;
	}

	nidsz_disp = cnt;
    ListItem li;
	for(int i = 0; i < cnt; i++){
        m_List.AddItem(li);
		if(!updateBtn){
			CASH_CATEGORY_ptr cat = cash_db.categoryById(idarray[i].id);
			if(cat->level < 2 && hasChild(i)){
				nid_disp[i].btnType = 1;
			}else{
				nid_disp[i].btnType = 0;
			}
		}
	}
	m_List.setupList(nid_disp);
}
