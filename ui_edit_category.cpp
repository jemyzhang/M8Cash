#include "ui_edit_category.h"
#include "ui_categories.h"
#include "m8cash.h"
#include <cMzCommon.h>
using namespace cMzCommon;

#define MZ_IDC_TOOLBAR_CATEDIT 101

#define MZ_IDC_BUTTON_CATPARENT 102
#define MZ_IDC_EDIT_CATNAME 103

MZ_IMPLEMENT_DYNAMIC(Ui_EditCateGoryWndEx)

BOOL Ui_EditCateGoryWndEx::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 0;

	m_CaptionTitle.SetPos(0,y,GetWidth(),MZM_HEIGHT_HEADINGBAR);
	if(_mode == 0){
		m_CaptionTitle.SetText(LOADSTRING(IDS_STR_NEW_CATEGORY).C_Str());
	}else{
		m_CaptionTitle.SetText(LOADSTRING(IDS_STR_EDIT_CATEGORY).C_Str());
	}
	AddUiWin(&m_CaptionTitle);

	y += MZM_HEIGHT_HEADINGBAR;
    m_EdtCategoryName.SetPos(0, y, GetWidth(), MZM_HEIGHT_SINGLELINE_EDIT);
    m_EdtCategoryName.SetMaxChars(10);
    m_EdtCategoryName.SetSipMode(IM_SIP_MODE_GEL_PY, MZM_HEIGHT_TEXT_TOOLBAR);
	m_EdtCategoryName.SetTip2(LOADSTRING(IDS_STR_NAME).C_Str());
    m_EdtCategoryName.SetLeftInvalid(110);
    m_EdtCategoryName.SetID(MZ_IDC_EDIT_CATNAME);
    AddUiWin(&m_EdtCategoryName);

    y += MZM_HEIGHT_SINGLELINE_EDIT;
    m_BtnParentCategory.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnParentCategory.SetText(LOADSTRING(IDS_STR_PARENT_CATEGORY).C_Str());
	m_BtnParentCategory.SetText2(cash_db.getCategoryFullNameById(_editIndex));
    m_BtnParentCategory.SetTextSize2(m_BtnParentCategory.GetTextSize2() - 2);
    m_BtnParentCategory.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    AddUiWin(&m_BtnParentCategory);

    //set the right image of the extended button m_BtnSetting1
    m_BtnParentCategory.SetImage2(imgArrow);
    m_BtnParentCategory.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnParentCategory.SetShowImage2(true);
    m_BtnParentCategory.SetID(MZ_IDC_BUTTON_CATPARENT);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    if (_mode == 0) { //new
        m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_CANCEL).C_Str());
        m_Toolbar.SetButton(2, true, true, LOADSTRING(IDS_STR_SAVE).C_Str());
    } else {
		CASH_CATEGORY_ptr c = cash_db.categoryById(_editIndex);
        m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_CANCEL).C_Str());
		if(c->level > 0 && c->type != CT_TRANSFER){
			m_Toolbar.SetButton(1, true, true, LOADSTRING(IDS_STR_DELETE).C_Str());
		}
        m_Toolbar.SetButton(2, true, true, LOADSTRING(IDS_STR_UPDATE).C_Str());
		_selectedPidx = c->parentid;
		m_EdtCategoryName.SetText(cash_db.getCategoryNameById(_editIndex));
		if(c->level > 0 && c->type != CT_TRANSFER){
			m_BtnParentCategory.SetText2(cash_db.getCategoryFullNameById(c->parentid));
		}else{
			m_BtnParentCategory.SetVisible(false);
		}
    }
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_CATEDIT);
    AddUiWin(&m_Toolbar);


    return TRUE;
}

void Ui_EditCateGoryWndEx::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    int nRet;
    UINT_PTR id = LOWORD(wParam);
    Ui_CategoriesWnd catdlg;
    RECT rcWork;
    switch (id) {
        case MZ_IDC_BUTTON_CATPARENT:
            rcWork = MzGetWorkArea();
            catdlg.setMode(1);
			if(_mode == 0){	//new
				catdlg.setHideCategory(-1);
				catdlg.setNodeDepth(1);
			}else{
				catdlg.setHideCategory(cash_db.categoryById(_editIndex)->id);
				catdlg.setNodeDepth(1);//cash_db.categoryById(_editIndex)->level-1);
			}
            catdlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
                    m_hWnd, 0, WS_POPUP);
            // set the animation of the window
            catdlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_BOTTOM_TO_TOP_2);
            catdlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_TOP_TO_BOTTOM_1);
            nRet = catdlg.DoModal();
            if (nRet == ID_OK) {
                _selectedPidx = catdlg.getSelectionIndex();
                m_BtnParentCategory.SetText2(cash_db.getCategoryFullNameById(_selectedPidx));
            }
            break;
        case MZ_IDC_TOOLBAR_CATEDIT:
        {
            int nIndex = lParam;
            if (_mode == 0) { //new
                if (nIndex == 0) {	//取消
                    EndModal(ID_CANCEL);
                    return;
                }
                if (nIndex == 2) {	//确定
					//prepare category record
					CASH_CATEGORY_t newCat;
					CASH_CATEGORY_ptr parentCat;

					if(m_EdtCategoryName.GetText().IsEmpty()){
						MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_WARN_CATEGORY_SETNAME).C_Str());
						m_EdtCategoryName.SetFocus(true);
						return;
					}
					newCat.name = m_EdtCategoryName.GetText().C_Str();
					newCat.parentid = _selectedPidx;

					parentCat = cash_db.categoryById(_selectedPidx);
					newCat.level = parentCat->level + 1;
					newCat.type = parentCat->type;
					//insert category record
					bool isconf = true;
					if(cash_db.checkDupCategory(&newCat,&isconf) != -1 && (isconf == false)){
						MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_ERR_CATEGORY_ADD).C_Str());
						m_EdtCategoryName.SetText(L"\0");
						m_EdtCategoryName.Invalidate();
					}else{
						cash_db.appendCategory(&newCat);
						MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_INFO_CATEGORY_ADDS).C_Str());
						EndModal(ID_OK);
					}
                    return;
                }
            } else {
                if (nIndex == 2) {	//更新
					//prepare category record
					CASH_CATEGORY_t	updatedCat;
					CASH_CATEGORY_ptr pcat = cash_db.categoryById(_editIndex);
					CASH_CATEGORY_ptr parentCat;

					//copy
					updatedCat.id = pcat->id;
					updatedCat.parentid = pcat->parentid;

					if(m_EdtCategoryName.GetText().IsEmpty()){
						MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_WARN_CATEGORY_SETNAME).C_Str());
						m_EdtCategoryName.SetFocus(true);
						return;
					}
					C::newstrcpy(&updatedCat.name,m_EdtCategoryName.GetText().C_Str());
					if(updatedCat.parentid != -1){
						updatedCat.parentid = _selectedPidx;
						parentCat = cash_db.categoryById(_selectedPidx);
						updatedCat.level = parentCat->level + 1;
						updatedCat.type = parentCat->type;
					}
					//update category record
					//if name / level was changed, check the duplication
					if(lstrcmp(updatedCat.name,pcat->name) != 0 || updatedCat.level != pcat->level){
						if(cash_db.checkDupCategory(&updatedCat) != -1){
							MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_ERR_CATEGORY_UPDATE).C_Str());
							m_EdtCategoryName.SetText(L"\0");
							m_EdtCategoryName.Invalidate();
							return;
						}
					}
					cash_db.updateCategory(&updatedCat);
					MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_INFO_CATEGORY_UPDATES).C_Str());
                    EndModal(ID_OK);
                    return;
                }
                if (nIndex == 1) { //删除
					if(MzMessageBoxEx(m_hWnd,LOADSTRING(IDS_STR_WARN_CATEGORY_DELETE).C_Str(),LOADSTRING(IDS_STR_OK).C_Str(),MZ_YESNO,false) == 1){
						if(cash_db.deleteCategoryById(_editIndex)){
							MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_ERR_CATEGORY_INUSE).C_Str());
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
