#ifndef _UI_CONFIG_H
#define _UI_CONFIG_H

// include the MZFC library header file
#include <mzfc_inc.h>
#include "..\MzCommon\UiSwitchOption.h"

// Popup window derived from CMzWndEx

class Ui_ConfigWnd : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(Ui_ConfigWnd);
public:
protected:
    UiScrollWin m_ScrollWin;
    UiToolbar_Text m_Toolbar;
    UiCaption m_Caption1;
    UiCaption m_Caption2;

	UiButtonEx m_BtnImport;
	UiButtonEx m_BtnExport;
	UiButtonEx m_BtnBackup;
	UiButtonEx m_BtnSetPassword;
	UiButtonEx m_BtnCategoryExpand;
	UiButtonEx m_BtnResetAll;
	UiButtonEx m_BtnOptimize;
	//Autolock
	UiSwitchOption m_BtnAutoLock;

    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
private:
	void updateAutolock();
private:
	DWORD c_expend_level;
};

#endif /*_UI_CONFIG_H*/