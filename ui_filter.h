#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>

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
	UiButtonEx m_BtnStartDate;		//��ʼ����ʱ��
	UiButtonEx m_BtnStartDateSW;	//��ʼ����ʱ�䰴ť ON/OFF
	UiButtonEx m_BtnEndDate;		//��������ʱ��
	UiButtonEx m_BtnEndDateSW;		//��������ʱ�䰴ť ON/OFF
	UiButtonEx m_BtnCategory;		//���˷���
	UiButtonEx m_BtnCategorySW;		//���˷��ఴť ON/OFF
	UiButtonEx m_BtnAccount;		//�˻�����
	UiButtonEx m_BtnAccountSW;		//�˻����˷��� ON/OFF
	UiButtonEx m_BtnPerson;			//��Ա����
	UiButtonEx m_BtnPersonSW;		//��Ա���˰�ť ON/OFF
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // override the MZFC window messages handler
    virtual LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
};
