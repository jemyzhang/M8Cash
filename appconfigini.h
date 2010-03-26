#pragma once

#include <cMzConfig.h>

//Ӧ�ó�������
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
	//�������趨��д
	cMzConfig IniSelectedAccountIndex;
	cMzConfig IniSelectedToAccountIndex;
	cMzConfig IniSelectedCategoryIndex;
	cMzConfig IniSelectedPersonIndex;
	////��������趨
	cMzConfig IniBrowseMode;
	cMzConfig IniBrowseOrderMode;
	//�����趨��д
	cMzConfig IniCategoryExpandLevel;
    //�������ڷ�Χ�趨
    cMzConfig IniExportAll;
    cMzConfig IniExportStartDate;
    cMzConfig IniExportEndDate;
	//�Զ���������
	cMzConfig IniAutolock;
	//�������������
	cMzConfig IniFilterStartDate;
	cMzConfig IniFilterEndDate;
	cMzConfig IniFilterAccount;
	cMzConfig IniFilterCategory;
	cMzConfig IniFilterPerson;
private:
	wchar_t ini_path[MAX_PATH];
};
