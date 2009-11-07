#pragma once
#include <mzfc_inc.h>

class UiEditKeySwitcher :
	public UiStatic
{
public:

	UiEditKeySwitcher(void)
	{
		pslotwin = 0;
	}

	~UiEditKeySwitcher(void)
	{
	}
	virtual int OnLButtonUp(UINT fwKeys, int xPos, int yPos){
		if(pslotwin){
			pslotwin->SetEnable(true);
		}
		return UiStatic::OnLButtonUp(fwKeys,xPos,yPos);
	}
	void SetSlotWin(UiWin* pWin){
		pslotwin = pWin;
	}
private:
	UiWin *pslotwin;
};

class UiEditKeySwitchButton :
	public UiButton_Image
{
public:

	UiEditKeySwitchButton(void)
	{
		pslotwin = 0;
		SetButtonType(MZC_BUTTON_NONE);
	}

	~UiEditKeySwitchButton(void)
	{
	}
	virtual int OnLButtonUp(UINT fwKeys, int xPos, int yPos){
		if(pslotwin){
			pslotwin->SetEnable(!pslotwin->IsEnable());
		}
		return UiButton_Image::OnLButtonUp(fwKeys,xPos,yPos);
	}
	void SetSlotWin(UiWin* pWin){
		pslotwin = pWin;
	}
	void SetButtonImage(ImagingHelper *img){
		if(img == 0) return;
		SetImage_Normal(img);
		SetImage_Pressed(img);
	}
private:
	UiWin *pslotwin;
};