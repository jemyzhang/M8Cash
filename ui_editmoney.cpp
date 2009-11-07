#include "ui_editmoney.h"
#include "m8cash.h"
#include "..\MzCommon\MzCommon.h"
using namespace MzCommon;

UiEditMoney::UiEditMoney(void)
 :UiSingleLineEdit()
{
	_enMinus = true;
	_dotPos = 0;
	SetSipMode(IM_SIP_MODE_DIGITEX, MZM_HEIGHT_TEXT_TOOLBAR);
	//SetSipMode(IM_SIP_MODE_NONE);
	pcalcdlg = 0;
	pMaskLabel = 0;
}

UiEditMoney::~UiEditMoney(void)
{
}

int UiEditMoney::OnChar(TCHAR chCharCode, LPARAM lKeyData){

	//only accept 0-9,dot,-
	int inputlen = GetText().Length();
	if(inputlen == 0){
		SetTextColor(RGB(0,0,0));
	}
	//only 2 digital after dot
	int r = GetCursePos();
	if(r == -1){	//末尾
		if(chCharCode != 0x08 && _dotPos != 0 && (inputlen - _dotPos > 1)) return 0;
		//only 8 digital before dot
		if(chCharCode != 0x08 &&  chCharCode != '#' && _dotPos == 0 && inputlen > 8) return 0;
	}else{	//插入情况
		if(_dotPos == 0){ //无小数点
			if(chCharCode != 0x08 && chCharCode != '#' && inputlen > 8) return 0;
			if(chCharCode == '#'){ //输入小数点
				if(inputlen - r > 2) return 0;
				_dotPos = r+1;
				return UiSingleLineEdit::OnChar('.',lKeyData);
			}
		}else if(r >= _dotPos){ //小数点后
			if(chCharCode == 0x08){
				if(r == _dotPos){
					_dotPos = 0;
				}
			}else if(inputlen - _dotPos > 1){
				return 0;
			}
		}else{ //小数点前
			if(chCharCode == 0x08){
				if(r > 0) _dotPos--;
			}else{
				if(_dotPos > 8){
					return 0;
				}else{
					_dotPos++;
				}
			}
		}
	}

	if(chCharCode < '0' || chCharCode > '9'){
		if(chCharCode != '#' && chCharCode != '*' && chCharCode != 0x08){
			return 0;
		}
		if(chCharCode == '*'){
			if(!_enMinus || inputlen > 0){
				return 0;
			}else{
				chCharCode = '-';
			}
		}
		if(chCharCode == '#'){
			if(_dotPos != 0){
				return 0;
			}else{
				_dotPos = inputlen + 1;
				chCharCode = '.';
			}
		}
		if(chCharCode == 0x08){ //backspace
			if(inputlen == _dotPos){
				_dotPos = 0;
			}
		}
	}
	return UiSingleLineEdit::OnChar(chCharCode,lKeyData);
}

int UiEditMoney::getInputAmount(){
	CMzStringW s = GetText();
	int Yuan = 0;
	int Fen = 0;
	if(s.Length() == 0){
		return 0;
	}
	bool minus = false;
	bool dot = false;
	wchar_t strYuan[10];
	wchar_t strFen[3];
	wchar_t *pstr = s.C_Str();
	wchar_t *py = strYuan;
	wchar_t *pf = strFen;
	for(int i = 0; i < 2; i++){
		strFen[i] = '0';
	}
	for(int i = 0; i < s.Length(); i++){
		if(pstr[i] == '-'){
			minus = true;
			continue;
		}
		if(pstr[i] == '.'){
			dot = true;
			continue;
		}
		if(dot){
			*pf++ = pstr[i];
		}else{
			*py++ = pstr[i];
		}
	}
	*py = 0;
	strFen[2] = 0;
	Yuan = _wtoi(strYuan);
	Fen = _wtoi(strFen);
	int sign = minus ? -1 : 1;
	return (sign * (Yuan*100+Fen));

}

bool UiEditMoney::isEmpty(){
	return (GetText().IsEmpty());
}

void UiEditMoney::SetText(LPCTSTR text){
	UiSingleLineEdit::SetText(text);
	CMzStringW s = GetText();
	wchar_t *pstr = s.C_Str();
	_dotPos = 0;
	for(int i = 0; i < s.Length(); i++){
		if(pstr[i] == '.'){
			_dotPos = i+1;
			break;
		}
	}
}

void UiEditMoney::EnableTips(bool e){
	if(!e) return;
	if(_enMinus){
		/*
		if(pMaskLabel){
			pMaskLabel->SetText(LOADSTRING(IDS_STR_INPUT_TIP_MINUS).C_Str());
			pMaskLabel->Invalidate();
			pMaskLabel->Update();
		}
		*/
		this->SetTip(LOADSTRING(IDS_STR_INPUT_TIP_MINUS).C_Str());
	}else{
		/*
		if(pMaskLabel){
			pMaskLabel->SetText(LOADSTRING(IDS_STR_INPUT_TIP).C_Str());
			pMaskLabel->Invalidate();
			pMaskLabel->Update();
		}
		*/
		this->SetTip(LOADSTRING(IDS_STR_INPUT_TIP).C_Str());
		//m_DisabledLabel.SetText(LOADSTRING(IDS_STR_INPUT_TIP).C_Str());
	}
}

void UiEditMoney::EnableMinus(bool e){
	_enMinus = e;
}


void UiEditMoney::ToggleCalc(){
	bool _isCalcShown = false;
	if(pcalcdlg != 0){
		if(pcalcdlg->IsVisible()){
			_isCalcShown = true;
		}
	}

	if(_isCalcShown){
		_isCalcShown = false;
		pcalcdlg->EndModal(ID_CANCEL);
		return;
	}

	Ui_CalculatorWnd calcdlg;
	pcalcdlg = &calcdlg;
	calcdlg.Create(0,GetRect().bottom + 40,GetParent()->GetWidth(),320,
		GetParentWnd(), 0, WS_POPUP);
    // set the animation of the window
    calcdlg.SetAnimateType_Show(MZ_ANIMTYPE_SCROLL_RIGHT_TO_LEFT_2);
    calcdlg.SetAnimateType_Hide(MZ_ANIMTYPE_SCROLL_LEFT_TO_RIGHT_1);
	if(!GetText().IsEmpty()){
		calcdlg.setInitOprand((double)getInputAmount()/100);
	}
	//calcdlg.Show();
    int nRet = calcdlg.DoModal();
	if(nRet == ID_OK){
		wchar_t res[32];
		double retval = calcdlg.getResult();
		if(!_enMinus && retval < 0){
			SetTextColor(RGB(255,0,0));
		}else{
			SetTextColor(RGB(0,0,0));
		}
		wsprintf(res,L"%.2f",retval);
		SetText(res);
		Invalidate();
	}
	pcalcdlg = 0;
}