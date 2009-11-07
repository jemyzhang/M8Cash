#include "ui_about.h"

#include "ui_config.h"
#include "ui_accounts.h"
#include "ui_categories.h"
#include "m8cash.h"
#include "..\MzCommon\MzCommon.h"
using namespace MzCommon;

MZ_IMPLEMENT_DYNAMIC(UI_AboutWnd)

#define MZ_IDC_TOOLBAR_ABOUT 101

BOOL UI_AboutWnd::OnInitDialog() {
    // Must all the Init of parent class first!
    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    // Then init the controls & other things in the window
    int y = 0;
    m_CaptionTitle.SetPos(0, 0, GetWidth(), MZM_HEIGHT_CAPTION);
	m_CaptionTitle.SetText(LOADSTRING(IDS_STR_ABOUT).C_Str());
    AddUiWin(&m_CaptionTitle);

	y += MZM_HEIGHT_CAPTION*2;
    m_TextName.SetPos(0, y, GetWidth(), MZM_HEIGHT_CAPTION);
	m_TextName.SetTextSize(m_TextName.GetTextSize()*2);
	m_TextName.SetText(LOADSTRING( IDS_STR_APPNAME).C_Str());
    AddUiWin(&m_TextName);

	y += MZM_HEIGHT_CAPTION*2;
	m_TextAuthor.SetPos(0, y, GetWidth(), MZM_HEIGHT_CAPTION);
	wchar_t author[128];
	wsprintf(author,LOADSTRING( IDS_STR_APPAUTHOR).C_Str(),L"JEMYZHANG");
	m_TextAuthor.SetText(author);
    AddUiWin(&m_TextAuthor);

	y += MZM_HEIGHT_CAPTION + 10;
	wchar_t version[128];
	wsprintf(version,LOADSTRING( IDS_STR_APPVERSION).C_Str(),VER_STRING,BUILD_STRING);
	m_TextVersion.SetPos(0, y, GetWidth(), MZM_HEIGHT_CAPTION);
	m_TextVersion.SetText(version);
    AddUiWin(&m_TextVersion);

	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    m_Toolbar.SetButton(0, true, true, LOADSTRING( IDS_STR_RETURN).C_Str());
    m_Toolbar.EnableLeftArrow(true);
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_ABOUT);
    AddUiWin(&m_Toolbar);

    return TRUE;
}

void UI_AboutWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
        case MZ_IDC_TOOLBAR_ABOUT:
        {
            int nIndex = lParam;
            if (nIndex == 0) {
                // exit the modal dialog
                EndModal(ID_OK);
                return;
            }
        }
    }
}