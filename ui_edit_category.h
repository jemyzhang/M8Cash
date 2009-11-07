#ifndef _UI_EDIT_CATEGORY_H
#define _UI_EDIT_CATEGORY_H

// include the MZFC library header file
#include <mzfc_inc.h>

// Main window derived from CMzWndEx

class Ui_EditCateGoryWndEx : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(Ui_EditCateGoryWndEx);
public:

    Ui_EditCateGoryWndEx() {
        _mode = 0;
        _editIndex = 0;
		_selectedPidx = 0;
    }

    void setMode(int m) {
        _mode = m;
    }

    void setEditIndex(int i) {
		if(_mode == 0){	//ÐÂ½¨
			_selectedPidx = i;
		}
        _editIndex = i;
    }
public:
    UiToolbar_Text m_Toolbar;
    UiButtonEx m_BtnParentCategory;
    UiSingleLineEdit m_EdtCategoryName;
	UiCaption m_CaptionTitle;
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
private:
    int _mode; //0: new, 1: edit
    int _editIndex;
	int _selectedPidx;	//selected parent id
};


#endif /*_UI_EDIT_CATEGORY_H*/