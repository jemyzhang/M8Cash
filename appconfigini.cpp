#include "appconfigini.h"


void CashConfig::InitIniKey(){
	IniSelectedAccountIndex.InitKey(L"Config",L"Account",0);
	IniSelectedToAccountIndex.InitKey(L"Config",L"ToAccount",1);
	IniSelectedCategoryIndex.InitKey(L"Config",L"Category",0);
	IniSelectedPersonIndex.InitKey(L"Config",L"Person",0);

	IniBrowseMode.InitKey(L"BrowseConfig",L"Mode",0);
	IniBrowseOrderMode.InitKey(L"BrowseConfig",L"Order",0);

	IniCategoryExpandLevel.InitKey(L"Category",L"InitExpandLevel",0);

	IniExportAll.InitKey(L"Export",L"bExportAll",1);
	IniExportStartDate.InitKey(L"Export",L"StartExportDate",0);
	IniExportEndDate.InitKey(L"Export",L"EndExportDate",0);


	//=============================
	/*
	Autolock
	plan 1:
	//LOWBYTE						//HIGHBYTE
	0: Disable							0
	1: Timeout						seconds after no operation
	2: Screenlock					seconds after screenlock
	3: Timeout & Screenlock			seconds after..
	plan 2:
	0: Disable
	1: as soon as ScreenLocked
	*/
	//=====================================
	IniAutolock.InitKey(L"AutoLock",L"Type",0);

	IniFilterStartDate.InitKey(L"Filter",L"StartDate",0);
	IniFilterEndDate.InitKey(L"Filter",L"EndDate",0);
	IniFilterAccount.InitKey(L"Filter",L"Account",-1);
	IniFilterCategory.InitKey(L"Filter",L"Category",-1);
	IniFilterPerson.InitKey(L"Filter",L"Person",-1);
}

