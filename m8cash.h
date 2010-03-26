#include "database.h"
#include "ui_main.h"
#include "ui_reminder.h"
#include "resource.h"
#include "appconfigini.h"

#define VER_STRING L"2.50"COMPILEL
#ifdef _DEBUG
#define COMPILEM	L"D"
#else
#define COMPILEM	L"R"
#endif

#ifdef MZFC_STATIC
#define COMPILEL	L"s"
#else
#define COMPILEL	L"d"
#endif

#define BUILD_STRING	L"20100325" COMPILEM

// Application class derived from CMzApp
extern CashConfig appconfig;
extern wchar_t db_path[256];
extern clsCASHDB cash_db;
extern wchar_t ini_reminder[256];
extern CashReminder cash_reminder;
extern ImagingHelper *pimg[];
extern ImagingHelper *imgArrow;
extern HINSTANCE LangresHandle;
extern HINSTANCE ImgresHandle;
extern bool g_bencypt;
extern bool g_pwddlgshow;


class M8CashApp : public CMzApp {
public:
    // The main window of the app.
    //Ui_MainWnd m_MainWnd;
    Ui_MainWnd *m_pMainWnd;

    //check pwd
    bool checkpwd();
	void loadImageRes();
    // Initialization of the application
    virtual BOOL Init();
	virtual int Done(){
		FreeMzResModule();
		if(isExternLangres) FreeLibrary(LangresHandle);
		return CMzApp::Done();
		if(m_pMainWnd) delete m_pMainWnd;
	}
private:
	bool isExternLangres;
};

