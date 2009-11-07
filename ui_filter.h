#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>

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
	UiButtonEx m_BtnStartDate;		//起始过滤时间
	UiButtonEx m_BtnStartDateSW;	//起始过滤时间按钮 ON/OFF
	UiButtonEx m_BtnEndDate;		//结束过滤时间
	UiButtonEx m_BtnEndDateSW;		//结束过滤时间按钮 ON/OFF
	UiButtonEx m_BtnCategory;		//过滤分类
	UiButtonEx m_BtnCategorySW;		//过滤分类按钮 ON/OFF
	UiButtonEx m_BtnAccount;		//账户过滤
	UiButtonEx m_BtnAccountSW;		//账户过滤分类 ON/OFF
	UiButtonEx m_BtnPerson;			//人员过滤
	UiButtonEx m_BtnPersonSW;		//人员过滤按钮 ON/OFF
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
};
