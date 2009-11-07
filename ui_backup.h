#ifndef _UI_BACKUP_H
#define _UI_BACKUP_H

// include the MZFC library header file
#include <mzfc_inc.h>

class Ui_BackupWnd : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(Ui_BackupWnd);
public:
    Ui_BackupWnd() {   }
protected:
    UiToolbar_Text m_Toolbar;
    UiCaption m_Caption1;
    UiList m_List;

    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
	//update display list
	void updateList();
public:
	static BOOL bbackup();
	static BOOL brecover(CMzString &itemname);
	static BOOL bdelete(CMzString &itemname);
};

#endif /*_UI_BACKUP_H*/