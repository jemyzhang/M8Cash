#pragma once

#include "../MzCommon/MzConfig.h"

//应用程序配置
class CashConfig : public AppConfigIni
{
public:
	CashConfig()
		: AppConfigIni(L"m8cash.ini"){
		InitIniKey();
	}
protected:
	void InitIniKey();
public:
	//主界面设定读写
	MzConfig IniSelectedAccountIndex;
	MzConfig IniSelectedToAccountIndex;
	MzConfig IniSelectedCategoryIndex;
	MzConfig IniSelectedPersonIndex;
	////浏览界面设定
	MzConfig IniBrowseMode;
	MzConfig IniBrowseOrderMode;
	//分类设定读写
	MzConfig IniCategoryExpandLevel;
    //导出日期范围设定
    MzConfig IniExportAll;
    MzConfig IniExportStartDate;
    MzConfig IniExportEndDate;
	//自动锁定设置
	MzConfig IniAutolock;
	//浏览过滤器设置
	MzConfig IniFilterStartDate;
	MzConfig IniFilterEndDate;
	MzConfig IniFilterAccount;
	MzConfig IniFilterCategory;
	MzConfig IniFilterPerson;
private:
	wchar_t ini_path[MAX_PATH];
};
