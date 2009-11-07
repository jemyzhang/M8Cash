#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include <Mzfc/MzProgressDialog.h> 

//#ifdef IM_USETAB
//#pragma message("导入导出: 使用tab分隔符")

//#define DEF_ACCOUNT_TITLE_FORMAT_S L"账户名称\t期初余额\t余额\t备注\t是否固定项"
#define DEF_ACCOUNT_FORMAT_S L"%s\t%.2f\t%.2f\t%s\t%d"

//#define DEF_CATEGORY_TITLE_FORMAT_S L"级别1\t级别2\t级别3"
#define DEF_CATEGORY_FORMAT_S L"%s\t%s\t%s"

//#define DEF_RECORD_TITLE_FORMAT_S L"日期\t金额\t账户\t转至账户\t分类\t备注"
#define DEF_RECORD_FORMAT_S L"%s\t%.2f\t%s\t%s\t%s\t%s\t%s"
//#else
//#pragma message("导入导出: 使用逗号分隔符")

//#define DEF_ACCOUNT_TITLE_FORMAT_C L"\"账户名称\",\"期初余额\",\"余额\",\"备注\",\"是否固定项\""
//#define DEF_ACCOUNT_TITLE_FORMAT_EXCEL_C L"账户名称,期初余额,余额,备注,是否固定项"
#define DEF_ACCOUNT_FORMAT_C L"\"%s\",\"%.2f\",\"%.2f\",\"%s\",\"%d\""

//#define DEF_CATEGORY_TITLE_FORMAT_C L"\"级别1\",\"级别2\",\"级别3\""
//#define DEF_CATEGORY_TITLE_FORMAT_EXCEL_C L"级别1,级别2,级别3"
#define DEF_CATEGORY_FORMAT_C L"\"%s\",\"%s\",\"%s\""

//#define DEF_RECORD_TITLE_FORMAT_C L"\"日期\",\"金额\",\"账户\",\"转至账户\",\"分类\",\"备注\""
//#define DEF_RECORD_TITLE_FORMAT_EXCEL_C L"日期,金额,账户,转至账户,分类,备注"
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
	//处理导入时字符串中的双引号
	wchar_t* processToken(wchar_t* &token);
private:
	bool _isImport;
	int _filetype;
	int _process;
	bool _filenotsel;
	//导出日期范围
	bool _dateAll;
	DWORD _sdate;
	DWORD _edate;
private:
	MzProgressDialog m_Progressdlg;
	wchar_t _lastErrMsg[256];
	wchar_t _exportfilename[128];
};
