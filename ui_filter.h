#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include "..\MzCommon\UiSwitchOption.h"

//���������ý���
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
    UiToolbar_Text m_Toolbar;	//ȷ��
	UiCaption  m_Caption;
	UiSwitchOption m_BtnStartDate;	//��ʼ����ʱ�䰴ť ON/OFF
	UiSwitchOption m_BtnEndDate;		//��������ʱ�䰴ť ON/OFF
	UiSwitchOption m_BtnCategory;		//���˷��ఴť ON/OFF
	UiSwitchOption m_BtnAccount;		//�˻����˷��� ON/OFF
	UiSwitchOption m_BtnPerson;		//��Ա���˰�ť ON/OFF
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	void ShowDateDlg(BOOL bstart = true);	//Ĭ����ʼ����ѡ��
	void ShowCategoryDlg();
	void ShowAccountDlg();
	void ShowPersonDlg();
private:
	BOOL optionChanged;	//�Ƿ�ѡ���иı䣬�ı��ID_OK����
};
