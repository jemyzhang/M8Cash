#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include <Mzfc/MzProgressDialog.h> 

//#ifdef IM_USETAB
//#pragma message("���뵼��: ʹ��tab�ָ���")

//#define DEF_ACCOUNT_TITLE_FORMAT_S L"�˻�����\t�ڳ����\t���\t��ע\t�Ƿ�̶���"
#define DEF_ACCOUNT_FORMAT_S L"%s\t%.2f\t%.2f\t%s\t%d"

//#define DEF_CATEGORY_TITLE_FORMAT_S L"����1\t����2\t����3"
#define DEF_CATEGORY_FORMAT_S L"%s\t%s\t%s"

//#define DEF_RECORD_TITLE_FORMAT_S L"����\t���\t�˻�\tת���˻�\t����\t��ע"
#define DEF_RECORD_FORMAT_S L"%s\t%.2f\t%s\t%s\t%s\t%s\t%s"
//#else
//#pragma message("���뵼��: ʹ�ö��ŷָ���")

//#define DEF_ACCOUNT_TITLE_FORMAT_C L"\"�˻�����\",\"�ڳ����\",\"���\",\"��ע\",\"�Ƿ�̶���\""
//#define DEF_ACCOUNT_TITLE_FORMAT_EXCEL_C L"�˻�����,�ڳ����,���,��ע,�Ƿ�̶���"
#define DEF_ACCOUNT_FORMAT_C L"\"%s\",\"%.2f\",\"%.2f\",\"%s\",\"%d\""

//#define DEF_CATEGORY_TITLE_FORMAT_C L"\"����1\",\"����2\",\"����3\""
//#define DEF_CATEGORY_TITLE_FORMAT_EXCEL_C L"����1,����2,����3"
#define DEF_CATEGORY_FORMAT_C L"\"%s\",\"%s\",\"%s\""

//#define DEF_RECORD_TITLE_FORMAT_C L"\"����\",\"���\",\"�˻�\",\"ת���˻�\",\"����\",\"��ע\""
//#define DEF_RECORD_TITLE_FORMAT_EXCEL_C L"����,���,�˻�,ת���˻�,����,��ע"
#define DEF_RECORD_FORMAT_C L"\"%s\",\"%.2f\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\""
//#endif

class Ui_ProcessImExport : public CMzWndEx
{
	MZ_DECLARE_DYNAMIC(Ui_ProcessImExport);
public:
	Ui_ProcessImExport(void);
	~Ui_ProcessImExport(void);
public:
	enum PROCESSTYPE{
		PROCESS_NONE		=	0x00,

		PROCESS_EXPORT		=	0x0001,
		PROCESS_IMPORT		=	0x0002,

		PROCESS_FILE_CSV_S	=	0x0010,	//csv with space
		PROCESS_FILE_CSV_C	=	0x0020,	//csv with comma
		PROCESS_FILE_QIF	=	0x0040,

		PROCESS_ACCOUNT		=	0x0100,
		PROCESS_CATEGORY	=	0x0200,
		PROCESS_RECORD		=	0x0400,
	};
	//process type
	bool excute(int t = (PROCESS_EXPORT | PROCESS_FILE_CSV_C | PROCESS_RECORD));	//start process
	wchar_t* getLastErrMsg() { return _lastErrMsg; }
	bool isNoFileSelected() { return _filenotsel; }
	void setExportDateRange(bool isAll = true, DWORD ds = 0, DWORD de = 0){
		_dateAll = isAll;
		_sdate = ds;
		_edate = de;
	}
protected:
	bool process_csv_import(wchar_t* file,int &succed, int &omited, int &failed);
	bool process_csv_export(wchar_t* path,int &n);
	bool process_qif_import(wchar_t* file,int &succed, int &omited, int &failed);
	bool process_qif_export(wchar_t* path,int &n);
private:
	void processOption(int t);
	//������ʱ�ַ����е�˫����
	wchar_t* processToken(wchar_t* &token);
private:
	bool _isImport;
	int _filetype;
	int _process;
	bool _filenotsel;
	//�������ڷ�Χ
	bool _dateAll;
	DWORD _sdate;
	DWORD _edate;
private:
	MzProgressDialog m_Progressdlg;
	wchar_t _lastErrMsg[256];
	wchar_t _exportfilename[128];
};
