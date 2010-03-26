#ifndef _UI_PERSON_EDIT_H
#define _UI_PERSON_EDIT_H

// include the MZFC library header file
#include <mzfc_inc.h>

// Main window derived from CMzWndEx

class Ui_PersonEditWndEx : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(Ui_PersonEditWndEx);
public:

    Ui_PersonEditWndEx() {
        _mode = 0;
        _editIndex = 0;
		_selectedTid = 0;
    }

    void setMode(int m) {
        _mode = m;
    }

    void setEditIndex(int i) {
		if(_mode == 0){	//新建
			_selectedTid = i;
		}
        _editIndex = i;
    }
public:
    UiToolbar_Text m_Toolbar;
    UiButtonEx m_BtnPersonType;
    UiSingleLineEdit m_EdtPersonName;
	UiHeadingBar m_CaptionTitle;
	UiList m_TypeList;
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
	LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	//设置类别内容
	void setPersonTypeList();
private:
    int _mode; //0: 新建, 1: 删除
    int _editIndex;
	int _selectedTid;	//selected person id
};


#endif /*_UI_PERSON_EDIT_H*/