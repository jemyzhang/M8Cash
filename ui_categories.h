#ifndef _UI_CATEGORIES_H
#define _UI_CATEGORIES_H

// include the MZFC library header file
#include <mzfc_inc.h>

typedef struct idList {
	int id;
	int btnType;
	bool isShow;
	UiButton_Image *m_btn;
}idList_t;

// Popup window derived from CMzWndEx
class CategoryList : public UiList {
public:
    // override the DrawItem member function to do your own drawing of the list
	CategoryList() { idlist = 0; m_btn = 0; }
    void DrawItem(HDC hdcDst, int nIndex, RECT* prcItem, RECT *prcWin, RECT *prcUpdate);
	void setupList(idList* i) { idlist = i; }
	void setupButton(UiButton *b){ m_btn = b; }
	virtual void OnRemoveItem(int nIndex){	//移除item时，同时移除button
		if(idlist){
			if(idlist[nIndex].m_btn){
				RemoveChild(idlist[nIndex].m_btn);
				delete idlist[nIndex].m_btn;
				idlist[nIndex].m_btn = 0;
			}
		}
	}
	virtual void PaintWin(HDC hdc, RECT* prcWin, RECT* prcUpdate){//移动时隐藏所有按钮
		if(idlist){
			int nCount = GetItemCount();
			for(int i = 0 ; i < nCount; i++){
				if(idlist[i].m_btn){
					idlist[i].m_btn->SetVisible(false);
				}
			}
		}
		return UiList::PaintWin(hdc, prcWin, prcUpdate);
	}
protected:
private:
	UiButton *m_btn;
	idList* idlist;
};

class Ui_CategoriesWnd : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(Ui_CategoriesWnd);
public:
    Ui_CategoriesWnd() {
		_mode = 0;
		_maxdepth = 2;
		_hideCategoryId = -1;
		idarray = 0;
		nid_disp = 0;
		_sel_idx = -1;
    }
    //set display mode
    //0: edit mode, 1: selection mode
    //use before show
	void setMode(int m) { _mode = m; }

    int getSelectionIndex() {
        return _selection;
    }

	void setNodeDepth(int d){
		if(d < 0) d = 0;
		if(d > 2) d = 2;
		_maxdepth = d;
	}

	void setHideCategory(int h){
		_hideCategoryId = h;
	}
protected:
    UiScrollWin m_ScrollWin;
    UiToolbar_Text m_Toolbar;
    UiHeadingBar m_Caption1;
	UiButton m_BtnSelect;


    CategoryList m_List;

    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);

	//update display list
	void updateList(bool updateBtn = true);
	void sortList(idList *p, int size);

	void hideChild(int nIndex);
	void showChild(int nIndex);
	bool hasChild(int nIndex);
private:
	int _mode;
    int _selection;
	int _maxdepth;	//max = 2
	int _hideCategoryId;
	idList *idarray;
	idList *nid_disp;	//id array for display
	int nidsz;	//id array size;
	int nidsz_disp;	//displayed id array size;
	int _sel_idx;
};

#endif /*_UI_CATEGORIES_H*/