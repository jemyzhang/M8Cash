#pragma once

#include <cMzConfig.h>

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
	cMzConfig IniSelectedAccountIndex;
	cMzConfig IniSelectedToAccountIndex;
	cMzConfig IniSelectedCategoryIndex;
	cMzConfig IniSelectedPersonIndex;
	////浏览界面设定
	cMzConfig IniBrowseMode;
	cMzConfig IniBrowseOrderMode;
	//分类设定读写
	cMzConfig IniCategoryExpandLevel;
    //导出日期范围设定
    cMzConfig IniExportAll;
    cMzConfig IniExportStartDate;
    cMzConfig IniExportEndDate;
	//自动锁定设置
	cMzConfig IniAutolock;
	//浏览过滤器设置
	cMzConfig IniFilterStartDate;
	cMzConfig IniFilterEndDate;
	cMzConfig IniFilterAccount;
	cMzConfig IniFilterCategory;
	cMzConfig IniFilterPerson;
private:
	wchar_t ini_path[MAX_PATH];
};
