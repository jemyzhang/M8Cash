#include "ui_person_edit.h"
#include "m8cash.h"
#include "..\MzCommon\MzCommon.h"
using namespace MzCommon;

#define MZ_IDC_TOOLBAR_PERSONEDIT 101

#define MZ_IDC_BUTTON_PERSONTYPE 102
#define MZ_IDC_EDIT_PERSONAME 103
#define MZ_IDC_EDIT_PERSONTYPELIST 104

MZ_IMPLEMENT_DYNAMIC(Ui_PersonEditWndEx)

BOOL Ui_PersonEditWndEx::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 0;

	m_CaptionTitle.SetPos(0,y,GetWidth(),MZM_HEIGHT_CAPTION);
	if(_mode == 0){
		m_CaptionTitle.SetText(LOADSTRING(IDS_STR_NEW_PERSON).C_Str());
	}else{
		m_CaptionTitle.SetText(LOADSTRING(IDS_STR_EDIT_PERSON).C_Str());
	}
	AddUiWin(&m_CaptionTitle);

	y += MZM_HEIGHT_CAPTION;
    m_EdtPersonName.SetPos(0, y, GetWidth(), MZM_HEIGHT_SINGLELINE_EDIT);
    m_EdtPersonName.SetMaxChars(10);
    m_EdtPersonName.SetSipMode(IM_SIP_MODE_GEL_PY, MZM_HEIGHT_TEXT_TOOLBAR);
	m_EdtPersonName.SetTip2(LOADSTRING(IDS_STR_NAME).C_Str());
    m_EdtPersonName.SetLeftInvalid(110);
    m_EdtPersonName.SetID(MZ_IDC_EDIT_PERSONAME);
    AddUiWin(&m_EdtPersonName);

    y += MZM_HEIGHT_SINGLELINE_EDIT;
    m_BtnPersonType.SetPos(0, y, GetWidth(), MZM_HEIGHT_BUTTONEX);
    m_BtnPersonType.SetText(LOADSTRING(IDS_STR_PERSON_TYPE).C_Str());
	m_BtnPersonType.SetText2(LOADSTRING(cash_db.getPersonTypeNameStrID((CASH_PERSON_TYPE_t)_selectedTid)).C_Str());
    m_BtnPersonType.SetTextSize2(m_BtnPersonType.GetTextSize2() - 2);
    m_BtnPersonType.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
    AddUiWin(&m_BtnPersonType);

    //set the right image of the extended button m_BtnSetting1
    m_BtnPersonType.SetImage2(imgArrow);
    m_BtnPersonType.SetImageWidth2(imgArrow->GetImageWidth());
    m_BtnPersonType.SetShowImage2(true);
    m_BtnPersonType.SetID(MZ_IDC_BUTTON_PERSONTYPE);

    y += MZM_HEIGHT_BUTTONEX;
    m_TypeList.SetPos(0, y, GetWidth(), GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR - MZM_HEIGHT_TEXT_TOOLBAR);
	m_TypeList.SetID(MZ_IDC_EDIT_PERSONTYPELIST);
	m_TypeList.EnableNotifyMessage(true);
	m_TypeList.SetVisible(false);
	setPersonTypeList();
    AddUiWin(&m_TypeList);

	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    if (_mode == 0) { //new
        m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_CANCEL).C_Str());
        m_Toolbar.SetButton(2, true, true, LOADSTRING(IDS_STR_SAVE).C_Str());
    } else {
        m_Toolbar.SetButton(0, true, true, LOADSTRING(IDS_STR_CANCEL).C_Str());
		m_Toolbar.SetButton(1, true, true, LOADSTRING(IDS_STR_DELETE).C_Str());
        m_Toolbar.SetButton(2, true, true, LOADSTRING(IDS_STR_UPDATE).C_Str());

		CASH_PERSON_ptr p = cash_db.personById(_editIndex);
		if(p){
			m_EdtPersonName.SetText(p->name);
			m_BtnPersonType.SetText2(LOADSTRING(cash_db.getPersonTypeNameStrID(p->type)).C_Str());
		}
    }
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_PERSONEDIT);
    AddUiWin(&m_Toolbar);

    return TRUE;
}

void Ui_PersonEditWndEx::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
        case MZ_IDC_BUTTON_PERSONTYPE:
			//弹出人员类别选择下拉列表
			m_BtnPersonType.SetFocus(true);
			m_BtnPersonType.Invalidate();
			m_BtnPersonType.Update();
			MzCloseSip();
			if(m_TypeList.IsVisible()){
				m_TypeList.SetVisible(false);
			}else{
				m_TypeList.SetVisible(true);
			}
			m_TypeList.Invalidate();
			m_TypeList.Update();
            break;
        case MZ_IDC_TOOLBAR_PERSONEDIT:
        {
            int nIndex = lParam;
            if (_mode == 0) { //新建
                if (nIndex == 0) {	//取消
                    EndModal(ID_CANCEL);
                    return;
                }
                if (nIndex == 2) {	//确定
					//prepare category record
					CASH_PERSON_t newPerson;

					if(m_EdtPersonName.GetText().IsEmpty()){
						MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_WARN_PERSON_SETNAME).C_Str());
						m_EdtPersonName.SetFocus(true);
						return;
					}
					newPerson.name = m_EdtPersonName.GetText().C_Str();
					newPerson.type = (CASH_PERSON_TYPE_t)_selectedTid;

					//insert category record
					bool isconf = true;
					if(cash_db.checkDupPerson(&newPerson) != -1){
						MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_ERR_PERSON_ADD).C_Str());
						m_EdtPersonName.SetText(L"\0");
						m_EdtPersonName.Invalidate();
						m_EdtPersonName.Update();
					}else{
						cash_db.appendPerson(&newPerson);
						MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_INFO_PERSON_ADDS).C_Str());
						EndModal(ID_OK);
					}
                    return;
                }
            } else {	//编辑
                if (nIndex == 2) {	//更新
					//prepare category record
					CASH_PERSON_t	updatedPerson;
					CASH_PERSON_ptr pPerson = cash_db.personById(_editIndex);
					if(pPerson == NULL) return;	//错误发生

					//copy
					updatedPerson.id = pPerson->id;

					if(m_EdtPersonName.GetText().IsEmpty()){
						MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_WARN_PERSON_SETNAME).C_Str());
						m_EdtPersonName.SetFocus(true);
						return;
					}
					C::newstrcpy(&updatedPerson.name,m_EdtPersonName.GetText().C_Str());
					updatedPerson.type = (CASH_PERSON_TYPE_t)_selectedTid;
					//update person record
					if(lstrcmp(updatedPerson.name,pPerson->name) != 0){
						if(cash_db.checkDupPerson(&updatedPerson) != -1){
							MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_ERR_PERSON_UPDATE).C_Str());
							m_EdtPersonName.SetText(L"\0");
							m_EdtPersonName.Invalidate();
							m_EdtPersonName.Update();
							return;
						}
					}
					cash_db.updatePerson(&updatedPerson);
					MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_INFO_PERSON_UPDATES).C_Str());
                    EndModal(ID_OK);
                    return;
                }
                if (nIndex == 1) { //删除
					if(MzMessageBoxEx(m_hWnd,LOADSTRING(IDS_STR_WARN_PERSON_DELETE).C_Str(),LOADSTRING(IDS_STR_OK).C_Str(),MZ_YESNO,false) == 1){
						if(!cash_db.deletePersonById(_editIndex)){
							MzAutoMsgBoxEx(m_hWnd,LOADSTRING(IDS_STR_ERR_PERSON_INUSE).C_Str());
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

void Ui_PersonEditWndEx::setPersonTypeList(){
	int nsize;
	const int* sets = cash_db.getPersonTypeNameStrSet(&nsize);
	for(int i = 0; i < nsize; i++){
		ListItem l;
		l.Text = LOADSTRING(sets[i]);
		m_TypeList.AddItem(l);
	}
}

LRESULT Ui_PersonEditWndEx::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case MZ_WM_MOUSE_NOTIFY:
        {
            int nID = LOWORD(wParam);
            int nNotify = HIWORD(wParam);
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            if (nID == MZ_IDC_EDIT_PERSONTYPELIST && nNotify == MZ_MN_LBUTTONUP) {
                if (!m_TypeList.IsMouseDownAtScrolling() && !m_TypeList.IsMouseMoved()) {
                    int nIndex = m_TypeList.CalcIndexOfPos(x, y);
					if(nIndex != -1){
						_selectedTid = nIndex;
						m_BtnPersonType.SetText2(
							LOADSTRING(
							cash_db.getPersonTypeNameStrID((CASH_PERSON_TYPE_t)_selectedTid)).C_Str());
						m_BtnPersonType.Invalidate();
						m_BtnPersonType.Update();
						m_TypeList.SetVisible(false);
						m_TypeList.Invalidate();
						m_TypeList.Update();
					}
                }
                return 0;
            }
            if (nID == MZ_IDC_EDIT_PERSONTYPELIST && nNotify == MZ_MN_MOUSEMOVE) {
                m_TypeList.SetSelectedIndex(-1);
                m_TypeList.Invalidate();
                m_TypeList.Update();
                return 0;
            }
        }
    }
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}