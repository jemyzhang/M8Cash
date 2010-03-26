#include "UiSwitchOption.h"
#include <cMzCommon.h>

using namespace cMzCommon;

MZ_IMPLEMENT_DYNAMIC(UiSwitchOption)

#define MZ_IDC_OPTION_LIST 101
#define MZ_IDC_BTN_OK   102
#define MZ_IDC_BTN_CANCEL   103
UiSwitchOption::UiSwitchOption(void)
{
	SetTextOffset2(120);
	SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	SetTextMaxLen(0);

    m_Switch.SetButtonType(MZC_BUTTON_SWITCH);
	m_Switch.SetButtonMode(MZC_BUTTON_MODE_HOLD);
    AddChild(&m_Switch);
}

UiSwitchOption::~UiSwitchOption(void)
{
}
