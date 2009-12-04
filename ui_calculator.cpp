#include "ui_calculator.h"
#include <MzCommon.h>
using namespace MzCommon;

MZ_IMPLEMENT_DYNAMIC(Ui_CalculatorWnd)

#define MZ_IDC_BTN_NUM_BASE 101	//0-9 101-110
#define MZ_IDC_BTN_DOT		111
#define MZ_IDC_BTN_SIGN		112
#define MZ_IDC_BTN_ADD		113
#define MZ_IDC_BTN_DEC		114
#define MZ_IDC_BTN_PLUS		115
#define MZ_IDC_BTN_DIV		116
#define MZ_IDC_BTN_CLEAR	117
#define MZ_IDC_BTN_BACK		118
#define MZ_IDC_BTN_EQU		119
#define MZ_IDC_BTN_EXIT		120

CALCBTNINFO_t calbtns[][5] = {
	{
		{ MZ_IDC_BTN_NUM_BASE + 7,	L"7",	CALBTN_NUM,	1,	1},
		{ MZ_IDC_BTN_NUM_BASE + 8,	L"8",	CALBTN_NUM,	1,	1},
		{ MZ_IDC_BTN_NUM_BASE + 9,	L"9",	CALBTN_NUM,	1,	1},
		{ MZ_IDC_BTN_ADD,			L"+",	CALBTN_OP,	1,	1},
		{ MZ_IDC_BTN_EXIT,			L"⊙",CALBTN_FUNC,1,	1},
	},
	{
		{ MZ_IDC_BTN_NUM_BASE + 4,	L"4",	CALBTN_NUM,	1,	1},
		{ MZ_IDC_BTN_NUM_BASE + 5,	L"5",	CALBTN_NUM,	1,	1},
		{ MZ_IDC_BTN_NUM_BASE + 6,	L"6",	CALBTN_NUM,	1,	1},
		{ MZ_IDC_BTN_DEC,			L"-",	CALBTN_OP,	1,	1},
		{ MZ_IDC_BTN_CLEAR,			L"C",	CALBTN_FUNC,1,	1},
	},
	{
		{ MZ_IDC_BTN_NUM_BASE + 1,	L"1",	CALBTN_NUM,	1,	1},
		{ MZ_IDC_BTN_NUM_BASE + 2,	L"2",	CALBTN_NUM,	1,	1},
		{ MZ_IDC_BTN_NUM_BASE + 3,	L"3",	CALBTN_NUM,	1,	1},
		{ MZ_IDC_BTN_PLUS,			L"×",	CALBTN_OP,	1,	1},
		{ MZ_IDC_BTN_BACK,			L"←",	CALBTN_FUNC,1,	1},
	},
	{
		{ MZ_IDC_BTN_NUM_BASE + 0,	L"0",	CALBTN_NUM,	1,	1},
		{ MZ_IDC_BTN_DOT,			L".",	CALBTN_NUM,	1,	1},
		{ MZ_IDC_BTN_SIGN,			L"±",	CALBTN_NUM,1,	1},
		{ MZ_IDC_BTN_DIV,			L"÷",	CALBTN_OP,	1,	1},
		{ MZ_IDC_BTN_EQU,			L"=",	CALBTN_FUNC,1,	1},
	},
};

Ui_CalculatorWnd::Ui_CalculatorWnd(void)
{
	_opPressed = false;
	_opndInput = false;
	_result = 0.00;
}

Ui_CalculatorWnd::~Ui_CalculatorWnd(void)
{
	for(int i =0; i < 20; i++){
		delete pm_Buttons[i];
	}
}
#define HSIZE 85
#define VSIZE 50
BOOL Ui_CalculatorWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
	m_bg.SetPos(0,0,GetWidth(),GetHeight());
	AddUiWin(&m_bg);

    int y = 8;
	m_Display.SetPos(10,y,GetWidth() - 20,70);
	m_Display.setText1(L"0");
	m_Display.clearNum();
	m_bg.AddChild(&m_Display);

	y += 80;
	int cnt = 0;
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 5; j++){
			if(calbtns[i][j].ID > 0){
				pm_Buttons[cnt] = new UiCalButton(calbtns[i][j].ID,
					calbtns[i][j].text,calbtns[i][j].atype);
				pm_Buttons[cnt]->SetButtonType(MZC_BUTTON_DOWNLOAD);
				pm_Buttons[cnt]->SetPos(10 + j*(HSIZE + 7), 
					y + i *(VSIZE + 5),
					HSIZE * calbtns[i][j].hsize, 
					VSIZE * calbtns[i][j].vsize );
				m_bg.AddChild(pm_Buttons[cnt]);
				cnt++;
			}
		}
	}

	calc.InitStack();
    return TRUE;
}

void Ui_CalculatorWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
//    switch (id) {

//	}
	return;
}

extern CalcOperator optrs[];

LRESULT Ui_CalculatorWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
		case MZ_MW_BTN_NUMERIC:
		{
			if(_opPressed){
				_opPressed = false;
				_opndInput = true;
			}
			int nID = LOWORD(wParam);
			if(nID <= MZ_IDC_BTN_NUM_BASE + 9){  
				int num = nID - MZ_IDC_BTN_NUM_BASE;
				wchar_t numstr = num + '0';
				if(m_Display.isNonInput()){
					m_Display.initNum(numstr);
				}else{
					m_Display.addChar1(numstr);
				}
			}else if(nID == MZ_IDC_BTN_DOT){
				m_Display.addChar1('.');
			}else if(nID == MZ_IDC_BTN_SIGN){
				m_Display.addChar1('-');
			}
			m_Display.Invalidate();
			break;
		}
		case MZ_MW_BTN_OPERATOR:
		{
			if(_opPressed) break;
			_opPressed = true;
			calc.PushOpnd(m_Display.getInputval());
			int nID = LOWORD(wParam);
			wchar_t* s;
			switch(nID){
				case MZ_IDC_BTN_ADD:
					s = L" + ";
					calc.PushOptr(optrs[0]);
					break;
				case MZ_IDC_BTN_DEC:
					s = L" - ";
					calc.PushOptr(optrs[1]);
					break;
				case MZ_IDC_BTN_PLUS:
					s = L" × ";
					calc.PushOptr(optrs[2]);
					break;
				case MZ_IDC_BTN_DIV:
					s = L" ÷ ";
					calc.PushOptr(optrs[3]);
					break;
			}
			m_Display.addString2(m_Display.getText1().C_Str());
			m_Display.addString2(s);
			if(_opndInput){
				m_Display.setText1(calc.GetResult());
			}
			m_Display.Invalidate();
			m_Display.setNonInput();
			break;
		}
		case MZ_MW_BTN_FUNCTION:
		{
			int nID = LOWORD(wParam);
			switch(nID){
				case MZ_IDC_BTN_EXIT:
					EndModal(ID_CANCEL);
					break;
				case MZ_IDC_BTN_EQU:
					calc.PushOpnd(m_Display.getInputval());
					_result = calc.CalculateAll();
					EndModal(ID_OK);
					break;
				case MZ_IDC_BTN_CLEAR:
					_opPressed = false;
					_opndInput = false;
					m_Display.setText2(L"\0");
					m_Display.clearNum();
					m_Display.Invalidate();
					calc.InitStack();
					break;
				case MZ_IDC_BTN_BACK:
					if(_opPressed && _opndInput){	//结果不可以用退格键删除
						return 0;
					}
					m_Display.addChar1(0x08);
					m_Display.Invalidate();
					break;
			}
		}
	}

    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

void UiNumericDisplay::addChar1(wchar_t chCharCode){
	//only accept 0-9,dot,-
	int inputlen = _l1str.Length();
	//排除连续输入0的问题
	if(inputlen == 1 && _l1str == L"0" && chCharCode == '0') return;
	//排除在输入运算符后输入dot的问题
	if(_isEmpty && chCharCode == '.'){
		_isEmpty = false;
		_l1str = L"0.";
		_dotPos = 2;
		return;
	}
	//only 2 digital after dot
	if(chCharCode != 0x08 && _dotPos != 0 && (inputlen - _dotPos > 1)) return;
	//only 8 digital before dot
	if(chCharCode != 0x08 &&  chCharCode != '.' && _dotPos == 0 && inputlen > 8) return;


	if(chCharCode < '0' || chCharCode > '9'){
		if(chCharCode != '.' && chCharCode != '-' && chCharCode != 0x08){
			return;
		}
		if(chCharCode == '-'){
			if(_isEmpty) return;
			if(_l1str.C_Str()[0] == '-'){
				_l1str = _l1str.SubStr(1,_l1str.Length() - 1);
			}else{
				CMzString Ssign = L"-";
				_l1str = Ssign + _l1str;
			}
			return;
		}
		if(chCharCode == '.'){
			if(_dotPos != 0){
				return;
			}else{
				_dotPos = inputlen + 1;
				chCharCode = '.';
			}
		}
		if(chCharCode == 0x08){ //backspace
			if(inputlen == _dotPos){
				_dotPos = 0;
			}
			removeChar1();
			return;
		}
	}
	addChar(chCharCode,_l1str);
	if(_isEmpty) _isEmpty = false;
}

void UiNumericDisplay::removeChar1(){
	if(_l1str.Length() == 1){
		setText1(L"0");
		_isEmpty = true;
		return;
	}
	_l1str = _l1str.SubStr(0,_l1str.Length() - 1);
}

void UiNumericDisplay::clearNum(){
	setText1(L"0");
	_isEmpty = true;
	_dotPos = 0;
}

double UiNumericDisplay::getInputval(){
	double retval = 0.0;
	if(_l1str.Length()){
		swscanf(_l1str.C_Str(),L"%lf",&retval);
	}
	return retval;
}