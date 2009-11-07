#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include "..\MzCommon\UiSwitchOption.h"

//过滤器设置界面
class Ui_FilterWnd : public CMzWndEx 
{
	MZ_DECLARE_DYNAMIC(Ui_FilterWnd);
public:
	Ui_FilterWnd(void);
	~Ui_FilterWnd(void);
private:
	void updateStartDate();
	void updateEndDate();
	void updateCategory();
	void updateAccount();
	void updatePerson();
	void updateUi(){
		updateStartDate();
		updateEndDate();
		updateCategory();
		updateAccount();
		updatePerson();
	}
public:
    UiToolbar_Text m_Toolbar;	//确定
	UiCaption  m_Caption;
	UiSwitchOption m_BtnStartDate;	//起始过滤时间按钮 ON/OFF
	UiSwitchOption m_BtnEndDate;		//结束过滤时间按钮 ON/OFF
	UiSwitchOption m_BtnCategory;		//过滤分类按钮 ON/OFF
	UiSwitchOption m_BtnAccount;		//账户过滤分类 ON/OFF
	UiSwitchOption m_BtnPerson;		//人员过滤按钮 ON/OFF
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void ShowDateDlg(BOOL bstart = true);	//默认起始日期选择
	void ShowCategoryDlg();
	void ShowAccountDlg();
	void ShowPersonDlg();
private:
	BOOL optionChanged;	//是否选项有改变，改变就ID_OK返回
};
