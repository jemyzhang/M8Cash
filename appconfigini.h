#pragma once

#include "../MzCommon/MzConfig.h"

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
	MzConfig IniSelectedAccountIndex;
	MzConfig IniSelectedToAccountIndex;
	MzConfig IniSelectedCategoryIndex;
	MzConfig IniSelectedPersonIndex;
	////��������趨
	MzConfig IniBrowseMode;
	MzConfig IniBrowseOrderMode;
	//�����趨��д
	MzConfig IniCategoryExpandLevel;
    //�������ڷ�Χ�趨
    MzConfig IniExportAll;
    MzConfig IniExportStartDate;
    MzConfig IniExportEndDate;
	//�Զ���������
	MzConfig IniAutolock;
	//�������������
	MzConfig IniFilterStartDate;
	MzConfig IniFilterEndDate;
	MzConfig IniFilterAccount;
	MzConfig IniFilterCategory;
	MzConfig IniFilterPerson;
private:
	wchar_t ini_path[MAX_PATH];
};
