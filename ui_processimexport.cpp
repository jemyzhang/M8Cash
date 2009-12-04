#include "ui_processimexport.h"

#include "m8cash.h"
#include "ui_backup.h"

#include "ui_imexport.h"
#include "UiFileDialogWnd.h"
#include <InitGuid.h>
#include <IMzUnknown.h>
#include <IMzUnknown_IID.h>
#include <IFileBrowser.h>
#include <IFileBrowser_GUID.h>

#include <MzCommon.h>
using namespace MzCommon;

MZ_IMPLEMENT_DYNAMIC(Ui_ProcessImExport)

Ui_ProcessImExport::Ui_ProcessImExport(void)
{
	wsprintf(_lastErrMsg,L"Not Started Yet~");
	m_Progressdlg.SetRoundRect(true);
//	m_Progressdlg.SetShowInfo(true);
	m_Progressdlg.SetShowTitle(true);
	_filenotsel = false;
}

Ui_ProcessImExport::~Ui_ProcessImExport(void)
{
}

bool Ui_ProcessImExport::excute(int t){
	bool ret;

	processOption(t);

	IFileBrowser *pFile = NULL;                      
	CoInitializeEx(NULL, COINIT_MULTITHREADED );
	if(_isImport){
		IMzSelect *pSelect = NULL; 
		if ( SUCCEEDED( CoCreateInstance( CLSID_FileBrowser, NULL,CLSCTX_INPROC_SERVER ,IID_MZ_FileBrowser,(void **)&pFile ) ) )
		{     
			if( SUCCEEDED( pFile->QueryInterface( IID_MZ_Select, (void**)&pSelect ) ) )
			{
				TCHAR file[ MAX_PATH ] = { 0 };
				pFile->SetParentWnd( m_hWnd );
				pFile->SetOpenDirectoryPath( L"\\Disk" );
				pFile->SetExtFilter( (_filetype == PROCESS_FILE_CSV_S || (_filetype == PROCESS_FILE_CSV_C)) ? L"*.csv" : L"*.qif" );                                      
				pFile->SetOpenDocumentType(DOCUMENT_SELECT_SINGLE_FILE);
				std::wstring fileDlgTitle = L"请选择需要导入的";
				fileDlgTitle += ((_filetype == PROCESS_FILE_CSV_S || (_filetype == PROCESS_FILE_CSV_C)) ? L"CSV文件" : L"QIF文件");
				pFile->SetTitle((TCHAR*)fileDlgTitle.c_str());
				fileDlgTitle.clear();

				if( pSelect->Invoke() ) 
				{						
					_tcscpy( file, pFile->GetSelectedFileName() );
					//process import action
					int n,o,f;
					if(_filetype == PROCESS_FILE_CSV_S || (_filetype == PROCESS_FILE_CSV_C)){
						ret = process_csv_import(file,n,o,f);
					}else{
						ret = process_qif_import(file,n,o,f);
					}
				}else{
					_filenotsel = true;
					return false;
				}
				pSelect->Release();
			}     
			pFile->Release();
		}	
		CoUninitialize();
		
	}else{
		UiFileDialogWnd dlg;
		dlg.SetTitle(L"请设置导出文件名");
		dlg.SetInitFileName(_exportfilename);
		if(_filetype == PROCESS_FILE_CSV_S || (_filetype == PROCESS_FILE_CSV_C)){
			dlg.SetInitFileSuffix(L".csv");
		}else{
			dlg.SetInitFileSuffix(L".qif");
		}
		dlg.SetInitFolder(L"\\Disk");
        RECT rcWork = MzGetWorkArea();
        dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork),
                m_hWnd, 0, WS_POPUP);
		if( dlg.DoModal() == ID_OK ) 
		{
			DateTime::waitms(500);
			//process file export
			int n;
			if(_filetype == PROCESS_FILE_CSV_S || (_filetype == PROCESS_FILE_CSV_C)){
				ret = process_csv_export(dlg.GetFullFileName(),n);
			}else{
				ret = process_qif_export(dlg.GetFullFileName(),n);
			}
		}else{
			_filenotsel = true;
			return false;
		}
	}
	return ret;
}

void Ui_ProcessImExport::processOption(int t){
	_isImport = ((t&0x000f) == PROCESS_IMPORT);
	_filetype = (t&0x00f0);
	_process = (t&0x0f00);
	if(!_isImport){
		switch(_process){
			case PROCESS_ACCOUNT:
				wsprintf(_exportfilename,LOADSTRING(IDS_STR_EX_ACCOUNT_FILENAME).C_Str(),L"导出",DateTime::NowtoStr());
				break;
			case PROCESS_CATEGORY:
				wsprintf(_exportfilename,LOADSTRING(IDS_STR_EX_CATEGORY_FILENAME).C_Str(),L"导出",DateTime::NowtoStr());
				break;
			case PROCESS_RECORD:
				if(_dateAll){
					wsprintf(_exportfilename,LOADSTRING(IDS_STR_EX_TRANS_ALL_FILENAME).C_Str(),L"导出",DateTime::NowtoStr());
				}else{
					ExportDate_t d1, d2;
					d1.Value = _sdate;
					d2.Value = _edate;
					if(_sdate == _edate){
						wsprintf(_exportfilename,LOADSTRING(IDS_STR_EX_TRANS_D1_FILENAME).C_Str(),
							d1.Date.Year,d1.Date.Month,d1.Date.Day,L"导出");
					}else{
						wsprintf(_exportfilename,LOADSTRING(IDS_STR_EX_TRANS_D2_FILENAME).C_Str(),
							d1.Date.Year,d1.Date.Month,d1.Date.Day,
							d2.Date.Year,d2.Date.Month,d2.Date.Day,L"导出");
					}
				}
				break;
			default:
				wsprintf(_exportfilename,LOADSTRING(IDS_STR_EX_UNK_FILENAME).C_Str(),DateTime::NowtoStr(),L"导出");
				break;
		}
	}
}

wchar_t* Ui_ProcessImExport::processToken(wchar_t* &token){
	if(token[0] == '\"'){
		token++;
	}
	int endpos = lstrlen(token);
	if(endpos > 0){
		endpos -= 1;
		if(token[endpos] == '\"'){
			token[endpos] = '\0';
		}
	}
	return token;
}

bool Ui_ProcessImExport::process_csv_import(wchar_t* file,int &succed, int &omited, int &failed){
	if(_isImport){
		if(MzMessageBoxEx(m_hWnd,
			LOADSTRING(IDS_STR_Q_BACKUP_IMPORT).C_Str(),
			LOADSTRING(IDS_STR_OK).C_Str(),MZ_YESNO,false) == 1){
			Ui_BackupWnd::bbackup();
		}
	}

	TEXTENCODE_t enc = File::getTextCode(file);
	if(enc != ttcAnsi && 
		enc != ttcUnicode &&
		enc != ttcUnicodeBigEndian){
			wsprintf(_lastErrMsg,LOADSTRING(IDS_STR_ERR_IMPORT_UNK_FORMAT).C_Str());
			return false;
	}
	m_Progressdlg.SetShowTitle(true);
	m_Progressdlg.SetShowInfo(true);
	m_Progressdlg.SetRange(0,100);
	m_Progressdlg.SetTitle(LOADSTRING(IDS_STR_IMPORT_WAIT).C_Str());
	m_Progressdlg.SetInfo(LOADSTRING(IDS_STR_IMPORT_ANALYSIS).C_Str());
	m_Progressdlg.BeginProgress(m_hWnd);
	list<CMzString> lines = File::loadText(file,enc);
	wchar_t* seps;
	if(_filetype == PROCESS_FILE_CSV_S){
		seps = L"\t";
	}else{
		seps = L",";
	}
	wchar_t infotxt[64];
	int cnt = 1;
	//错误结果归纳
	int nOmit = 0;
	int nConflict = 0;
	int nSkip = 0;
	int nError = 0;
	int nSuccess = 0;
	switch(_process){
		case PROCESS_ACCOUNT:
		{
			list<CMzString>::iterator i = lines.begin();
			CMzString linestr = *i++;
			UINT firstlineID;
			if(_filetype == PROCESS_FILE_CSV_S){
				firstlineID = IDS_STR_ACCOUNT_TITLE_FORMAT_S;
			}else{
				firstlineID = IDS_STR_ACCOUNT_TITLE_FORMAT_C;
			}
			if(!(linestr == LOADSTRING(firstlineID))){
				if(!(linestr == LOADSTRING(IDS_STR_ACCOUNT_TITLE_FORMAT_EXCEL_C))){
					wsprintf(_lastErrMsg,LOADSTRING(IDS_STR_ERR_IMPORT_FORMAT).C_Str(),
						LOADSTRING(IDS_STR_ACCOUNT).C_Str());
					m_Progressdlg.EndProgress();
					return false;
				}
			}
			int size = lines.size() - 1;
			for(; i != lines.end(); i++){
				wsprintf(infotxt,LOADSTRING(IDS_STR_IMPORT_RECORD).C_Str(),cnt,size);
				cnt++;
				m_Progressdlg.SetInfo(infotxt);
				m_Progressdlg.SetCurValue(cnt*100/size);
				m_Progressdlg.UpdateProgress();
				linestr = *i;
				CASH_ACCOUNT_t account;
				wchar_t* token;
				token = C::_wcstok(linestr.C_Str(),seps);
				if(token == NULL){	//name string
					nSkip++;
					continue;
				}
				processToken(token);	//omit \"\"
				C::newstrcpy(&account.name,token);

				token = C::_wcstok(NULL,seps);
				if(token == NULL){	//initval double
					nSkip++;
					continue;
				}
				processToken(token);	//omit \"\"
				double d_initval = 0.0;
				swscanf(token,L"%lf",&d_initval);
				account.initval = (int)(d_initval * 100);

				token = C::_wcstok(NULL,seps);
				if(token == NULL){	//leftval double
					nSkip++;
					continue;
				}

				token = C::_wcstok(NULL,seps);
				if(token == NULL){	//note string
					nSkip++;
					continue;
				}
				processToken(token);	//omit \"\"
				wchar_t *snote = new wchar_t[1024];
				C::newstrcpy(&account.note,token);
				C::restoreWrap(snote,account.note);
				delete[] account.note;
				account.note = snote;

				token = C::_wcstok(NULL,seps);
				if(token == NULL){	//isfix bool
					nSkip++;
					continue;
				}
				processToken(token);	//omit \"\"
				swscanf(token,L"%d",&account.isfix);
				if(account.isfix > 1){
					nError++;
					account.isfix = 0;
				}
				bool bconf;
				if(cash_db.checkDupAccount(&account,&bconf) == -1){
					cash_db.appendAccount(&account);
					nSuccess++;
				}else{
					if(bconf){
						nConflict++;
					}else{
						nOmit++;
					}
				}
			}
			wsprintf(_lastErrMsg,
				LOADSTRING(IDS_STR_IMPORT_RESULT).C_Str(),
				nSuccess,
				LOADSTRING(IDS_STR_ACCOUNT).C_Str());
			m_Progressdlg.EndProgress();
			return true;
			break;
		}
		case PROCESS_CATEGORY:
		{
			list<CMzString>::iterator i = lines.begin();
			CMzString linestr = *i++;
			UINT firstlineID;
			if(_filetype == PROCESS_FILE_CSV_S){
				firstlineID = IDS_STR_CATEGORY_TITLE_FORMAT_S;
			}else{
				firstlineID = IDS_STR_CATEGORY_TITLE_FORMAT_C;
			}
			if(!(linestr == LOADSTRING(firstlineID))){
				if(!(linestr == LOADSTRING(IDS_STR_CATEGORY_TITLE_FORMAT_EXCEL_C))){
					wsprintf(_lastErrMsg,LOADSTRING(IDS_STR_ERR_IMPORT_FORMAT).C_Str(),
						LOADSTRING(IDS_STR_CATEGORY).C_Str());
					m_Progressdlg.EndProgress();
					return false;
				}
			}
			int size = lines.size() - 1;
			for(; i != lines.end(); i++){
				wsprintf(infotxt,LOADSTRING(IDS_STR_IMPORT_RECORD).C_Str(),cnt,size);
				cnt++;
				m_Progressdlg.SetInfo(infotxt);
				m_Progressdlg.SetCurValue(cnt*100/size);
				m_Progressdlg.UpdateProgress();
				linestr = *i;
				CASH_CATEGORY_t category0;
				CASH_CATEGORY_t category1;
				CASH_CATEGORY_t category2;
				wchar_t* token;
				//级别1
				token = C::_wcstok(linestr.C_Str(),seps);
				if(token == NULL){	//category id
					nSkip++;
					continue;
				}
				processToken(token);	//omit \"\"
				if(lstrlen(token) == 0){
					nSkip++;
					continue;
				}
				C::newstrcpy(&category0.name,token);	//omit \"\"
				category0.parentid = -1;
				if(lstrcmp(category0.name,LOADSTRING(IDS_STR_TRANSFER).C_Str()) == 0){
					category0.level = 2;
				}else{
					category0.level = 0;
				}
				bool bconf;
				int retid = cash_db.checkDupCategory(&category0,&bconf);
				if( retid == -1){
					cash_db.appendCategory(&category0);
					nSuccess++;
				}else{
					category0.id = retid;
					category0.type = cash_db.categoryById(retid)->type;
				}

				//级别2
				token = C::_wcstok(NULL,seps);
				if(token == NULL){	//name string
					nSkip++;
					continue;
				}
				processToken(token);	//omit \"\"
				if(lstrlen(token) == 0){
					continue;
				}
				C::newstrcpy(&category1.name,token);	//omit \"\"
				category1.parentid = category0.id;
				category1.type = category0.type;
				category1.level = 1;
				retid = cash_db.checkDupCategory(&category1,&bconf);
				if(retid == -1){
					cash_db.appendCategory(&category1);
					nSuccess++;
				}else{
					category1.id = retid;
				}

				//级别3
				token = C::_wcstok(NULL,seps);
				if(token == NULL){	//category parent id
					nSkip++;
					continue;
				}
				processToken(token);	//omit \"\"
				if(lstrlen(token) == 0){
					continue;
				}
				C::newstrcpy(&category2.name,token);	//omit \"\"
				category2.parentid = category1.id;
				category2.type = category1.type;
				category2.level = 2;
				if(cash_db.checkDupCategory(&category2,&bconf) == -1){
					cash_db.appendCategory(&category2);
					nSuccess++;
				}
			}
			wsprintf(_lastErrMsg,
				LOADSTRING(IDS_STR_IMPORT_RESULT).C_Str(),
				nSuccess,
				LOADSTRING(IDS_STR_CATEGORY).C_Str());
			m_Progressdlg.EndProgress();
			return true;
			break;
		}
		case PROCESS_RECORD:
		{
			list<CMzString>::iterator i = lines.begin();
			CMzString linestr = *i++;
			bool bexcel = false;
			UINT firstlineID;
			if(_filetype == PROCESS_FILE_CSV_S){
				firstlineID = IDS_STR_RECORD_TITLE_FORMAT_S;
			}else{
				firstlineID = IDS_STR_RECORD_TITLE_FORMAT_C;
			}
			if(!(linestr == LOADSTRING(firstlineID))){
				if(!(linestr == LOADSTRING(IDS_STR_RECORD_TITLE_FORMAT_EXCEL_C))){
					wsprintf(_lastErrMsg,LOADSTRING(IDS_STR_ERR_IMPORT_FORMAT).C_Str(),
						LOADSTRING(IDS_STR_RECORDS).C_Str());
					m_Progressdlg.EndProgress();
					return false;
				}else{
					bexcel = true;
				}
			}
			int size = lines.size() - 1;
			for(; i != lines.end(); i++){
				wsprintf(infotxt,LOADSTRING(IDS_STR_IMPORT_RECORD).C_Str(),cnt,size);
				cnt++;
				m_Progressdlg.SetInfo(infotxt);
				m_Progressdlg.SetCurValue(cnt*100/size);
				m_Progressdlg.UpdateProgress();
				linestr = *i;
				CASH_RECORD_t record;
				wchar_t* token;
				token = C::_wcstok(linestr.C_Str(),seps);
				if(token == NULL){	//日期
					nSkip++;
					continue;
				}
				processToken(token);	//omit \"\"
				int y, d, m;
				if(bexcel){
					//兼容excel日期格式
					swscanf(token,L"%d/%d/%d",&y,&m,&d);
				}else{
					swscanf(token,L"%04d-%02d-%02d",&y,&m,&d);
				}
				//检测时间正确性
				if(y <= 1904 || m <= 0 || m > 12 ||
					d <= 0 || d > 31){
					nSkip++;
					continue;
				}
				wchar_t *datestr = new wchar_t[64];
				wsprintf(datestr,L"%04d-%02d-%02d 00:00:00",y,m,d);
				record.date = datestr;

				token = C::_wcstok(NULL,seps);
				if(token == NULL){	//金额
					nSkip++;
					continue;
				}
				processToken(token);	//omit \"\"
				double d_amount = 0.0;
				swscanf(token,L"%lf",&d_amount);
				record.amount = (int)(d_amount * 100);

				token = C::_wcstok(NULL,seps);
				if(token == NULL){	//账户名称
					nSkip++;
					continue;
				}
				processToken(token);	//omit \"\"
				CASH_ACCOUNT_t a;
				C::newstrcpy(&a.name, token);
				if(lstrlen(a.name) == 0){
					nSkip++;	//账户名称错误
					continue;
				}
				int retid = cash_db.checkDupAccount(&a);
				if(retid != -1){	//账户存在
					record.accountid = retid;
				}else{	//账户不存在，添加
					a.initval = 0;
					a.isfix = 0;
					a.note = L"\0";
					cash_db.appendAccount(&a);
					record.accountid = a.id;
				}

				token = C::_wcstok(NULL,seps);
				if(token == NULL){	//转至账户
					nSkip++;
					continue;
				}
				processToken(token);	//omit \"\"
				if(lstrlen(token) == 0){
					record.toaccountid = -1;
				}else{
					C::newstrcpy(&a.name, token);
					int retid = cash_db.checkDupAccount(&a);
					if(retid != -1){	//账户存在
						record.toaccountid = retid;
					}else{	//账户不存在，添加
						a.initval = 0;
						a.isfix = 0;
						a.note = L"\0";
						cash_db.appendAccount(&a);
						record.toaccountid = a.id;
					}
					record.isTransfer = 1;
				}

				token = C::_wcstok(NULL,seps);
				if(token == NULL){	//分类
					nSkip++;
					continue;
				}
				processToken(token);	//omit \"\"
				wchar_t* categoryStr = token;

				token = C::_wcstok(NULL,seps);
				if(token == NULL){	//人员
					nSkip++;
					continue;
				}
				processToken(token);
				//分析人员信息
				CASH_PERSON_t person;
				C::newstrcpy(&person.name,token);
				person.id = 0;
				person.type = PRSN_FAMILY;
				retid = cash_db.checkDupPerson(&person);
				if(retid != -1){ //人员存在
					record.personid = retid;
				}else{
					cash_db.appendPerson(&person);
					record.personid = person.id;
				}

				token = C::_wcstok(NULL,seps);
				if(token == NULL){	//备注
					nSkip++;
					continue;
				}
				processToken(token);	//omit \"\"
				if(lstrlen(token)){
					wchar_t *snote = new wchar_t[1024];
					C::newstrcpy(&record.note,token);
					C::restoreWrap(snote,record.note);
					delete[] record.note;
					record.note = snote;
				}else{
					record.note = L"\0";
				}

				//分析分类信息
				int level = 0;
				CASH_CATEGORY_t cat0;
				token = C::_wcstok(categoryStr,L" - ");
				if(token == NULL){
					nSkip++;
					continue;
				}
				C::newstrcpy(&cat0.name,token);
				cat0.parentid = -1;
				if(record.isTransfer){
					cat0.level = 2;
				}else{
					cat0.level = 0;
				}
				retid = cash_db.checkDupCategory(&cat0);
				if(retid == -1){
					cash_db.appendCategory(&cat0);
				}else{
					cat0.id = retid;
					cat0.type = cash_db.categoryById(retid)->type;
				}

				token = C::_wcstok(NULL,L" - ");
				if(token == NULL){
					record.categoryid = cat0.id;
				}else{
					CASH_CATEGORY_t cat1;
					C::newstrcpy(&cat1.name,token);
					cat1.parentid = cat0.id;
					cat1.level = cat0.level + 1;
					cat1.type = cat0.type;
					retid = cash_db.checkDupCategory(&cat1);
					if(retid == -1){
						cash_db.appendCategory(&cat1);
					}else{
						cat1.id = retid;
					}
					token = C::_wcstok(NULL,L" - ");
					if(token == NULL){
						record.categoryid = cat1.id;
					}else{
						CASH_CATEGORY_t cat2;
						C::newstrcpy(&cat2.name,token);
						cat2.parentid = cat1.id;
						cat2.level = cat1.level + 1;
						cat2.type = cat1.type;
						retid = cash_db.checkDupCategory(&cat2);
						if(retid == -1){
							cash_db.appendCategory(&cat2);
						}else{
							cat2.id = retid;
						}
						record.categoryid = cat2.id;
					}
				}

				if(cash_db.checkDupRecord(&record) == -1){
					cash_db.appendTransaction(&record);
					nSuccess++;
				}else{
					nOmit++;
				}
			}
			wsprintf(_lastErrMsg,
				LOADSTRING(IDS_STR_IMPORT_RESULT).C_Str(),
				nSuccess,
				LOADSTRING(IDS_STR_RECORDS).C_Str());
			m_Progressdlg.EndProgress();
			return true;
			break;
		}
		default:
		{
			wsprintf(_lastErrMsg,LOADSTRING(IDS_STR_ERR_IMPORT_UNK_FORMAT).C_Str());
			return false;
		}
	}
	return true;
}

bool Ui_ProcessImExport::process_csv_export(wchar_t* file,int &n){
	FILE* fp;
	fp = _wfopen(file,L"wt");
	if(fp == NULL){
		wsprintf(_lastErrMsg,L"文件打开失败。");
		return false;
	}
	m_Progressdlg.SetRange(0,100);
	
	switch(_process){
		case PROCESS_ACCOUNT:
		{
			UINT firstlineID;
			if(_filetype == PROCESS_FILE_CSV_S){
				firstlineID = IDS_STR_ACCOUNT_TITLE_FORMAT_S;
			}else{
				firstlineID = IDS_STR_ACCOUNT_TITLE_FORMAT_C;
			}
			fwprintf(fp,LOADSTRING(firstlineID).C_Str());	//write title
			fwprintf(fp,L"\n");
			int cnt = 0;
			wchar_t note[512];
			m_Progressdlg.SetTitle(LOADSTRING(IDS_STR_EXPORT_WAIT).C_Str());
			m_Progressdlg.BeginProgress(m_hWnd);
			list<CASH_ACCOUNT_ptr>::iterator i = cash_db.list_account.begin();
			int size = cash_db.list_account.size();
			wchar_t *exformat;
			if(_filetype == PROCESS_FILE_CSV_S){
				exformat = DEF_ACCOUNT_FORMAT_S;
			}else{
				exformat = DEF_ACCOUNT_FORMAT_C;
			}
			for(; i != cash_db.list_account.end();i++){
				CASH_ACCOUNT_ptr c = *i;
				int balance = cash_db.AccountBalanceById(c->id);
				fwprintf(fp,exformat,
					c->name,
					(double)c->initval/100,
					(double)balance/100,
					C::removeWrap(note,c->note),
					c->isfix);
				fwprintf(fp,L"\n");
				m_Progressdlg.SetCurValue((cnt++)*100/size);
				m_Progressdlg.UpdateProgress();
			}
			fclose(fp);
			wsprintf(_lastErrMsg,
				LOADSTRING(IDS_STR_EXPORT_RESULT).C_Str(),
				size,
				LOADSTRING(IDS_STR_ACCOUNT).C_Str());
			m_Progressdlg.EndProgress();
		}
			break;
		case PROCESS_CATEGORY:
		{
			UINT firstlineID;
			if(_filetype == PROCESS_FILE_CSV_S){
				firstlineID = IDS_STR_CATEGORY_TITLE_FORMAT_S;
			}else{
				firstlineID = IDS_STR_CATEGORY_TITLE_FORMAT_C;
			}
			fwprintf(fp,LOADSTRING(firstlineID).C_Str());	//write title
			fwprintf(fp,L"\n");
			int cnt = 0;
			m_Progressdlg.SetTitle(LOADSTRING(IDS_STR_EXPORT_WAIT).C_Str());
			m_Progressdlg.BeginProgress(m_hWnd);
			list<CASH_CATEGORY_ptr>::iterator i = cash_db.list_category.begin();
			int size = cash_db.list_category.size();
			wchar_t *exformat;
			if(_filetype == PROCESS_FILE_CSV_S){
				exformat = DEF_CATEGORY_FORMAT_S;
			}else{
				exformat = DEF_CATEGORY_FORMAT_C;
			}
			for(; i != cash_db.list_category.end();i++){
				CASH_CATEGORY_ptr c = *i;
				int level = c->level;
				if(lstrcmp(c->name,LOADSTRING(IDS_STR_TRANSFER).C_Str()) == 0){
					level = 0;
				}
				switch(level){
				case 0:
					fwprintf(fp,exformat,c->name,L"\0",L"\0");
					break;
				case 1:
					fwprintf(fp,exformat,cash_db.categoryById(c->parentid)->name,c->name,L"\0");
					break;
				case 2:
					fwprintf(fp,exformat,
						cash_db.categoryById(cash_db.categoryById(c->parentid)->parentid)->name,
						cash_db.categoryById(c->parentid)->name,
						c->name);
					break;
				default:
					break;
				}
				fwprintf(fp,L"\n");
				m_Progressdlg.SetCurValue((cnt++)*100/size);
				m_Progressdlg.UpdateProgress();
			}
			fclose(fp);
			wsprintf(_lastErrMsg,
				LOADSTRING(IDS_STR_EXPORT_RESULT).C_Str(),
				size,
				LOADSTRING(IDS_STR_CATEGORY).C_Str());
			m_Progressdlg.EndProgress();
		}
			break;
		case PROCESS_RECORD:
		{
			UINT firstlineID;
			if(_filetype == PROCESS_FILE_CSV_S){
				firstlineID = IDS_STR_RECORD_TITLE_FORMAT_S;
			}else{
				firstlineID = IDS_STR_RECORD_TITLE_FORMAT_C;
			}
			fwprintf(fp,LOADSTRING(firstlineID).C_Str());
			fwprintf(fp,L"\n");
			int cnt = 0;
			wchar_t note[512];
			m_Progressdlg.SetTitle(LOADSTRING(IDS_STR_EXPORT_WAIT).C_Str());
			m_Progressdlg.BeginProgress(m_hWnd);
			if(_dateAll){
				cash_db.loadRecords();
			}else{
				ExportDate_t d1, d2;
				d1.Value = _sdate;
				d2.Value = _edate;
				RECORDATE_t rd1,rd2;
				rd1.Year = d1.Date.Year; rd1.Month = d1.Date.Month; rd1.Day = d1.Date.Day;
				rd2.Year = d2.Date.Year; rd2.Month = d2.Date.Month; rd2.Day = d2.Date.Day;
				cash_db.getRecordsByDate(&rd1,&rd2);
			}
			list<CASH_RECORD_ptr>::iterator i = cash_db.list_search_record.begin();
			int size = cash_db.list_search_record.size();
			wchar_t *exformat;
			if(_filetype == PROCESS_FILE_CSV_S){
				exformat = DEF_RECORD_FORMAT_S;
			}else{
				exformat = DEF_RECORD_FORMAT_C;
			}
			for(; i != cash_db.list_search_record.end();i++){
				CASH_RECORD_ptr c = *i;
				CMzString dateonly = c->date;
				fwprintf(fp,exformat,
					dateonly.SubStr(0,10).C_Str(),
					(double)c->amount/100,
					cash_db.getAccountNameById(c->accountid),
					c->isTransfer ? cash_db.getAccountNameById(c->toaccountid) : L"\0",
					cash_db.getCategoryFullNameById(c->categoryid),cash_db.getPersonNameById(c->personid),
					C::removeWrap(note,c->note));
				fwprintf(fp,L"\n");
				m_Progressdlg.SetCurValue((cnt++)*100/size);
				m_Progressdlg.UpdateProgress();
			}
			fclose(fp);
			wsprintf(_lastErrMsg,
				LOADSTRING(IDS_STR_EXPORT_RESULT).C_Str(),
				size,
				LOADSTRING(IDS_STR_RECORDS).C_Str());
			m_Progressdlg.EndProgress();
		}
			break;
		default:
			wsprintf(_lastErrMsg,L"未知导出格式，中止。");
			return false;
	}
	return true;
}
bool Ui_ProcessImExport::process_qif_import(wchar_t* file,int &succed, int &omited, int &failed){
	wsprintf(_lastErrMsg,L"功能尚未完成。");
	return false;
}
bool Ui_ProcessImExport::process_qif_export(wchar_t* file,int &n){
	FILE* fp;
	fp = _wfopen(file,L"wt");
	if(fp == NULL){
		wsprintf(_lastErrMsg,L"文件打开失败。");
		return false;
	}
	m_Progressdlg.SetRange(0,100);
	
	switch(_process){
		case PROCESS_ACCOUNT:
		{
			int cnt = 0;
			m_Progressdlg.SetTitle(LOADSTRING(IDS_STR_EXPORT_WAIT).C_Str());
			m_Progressdlg.BeginProgress(m_hWnd);
			//写入头部
			fwprintf(fp,L"!Account\n");
			list<CASH_ACCOUNT_ptr>::iterator i = cash_db.list_account.begin();
			int size = cash_db.list_account.size();
			for(; i != cash_db.list_account.end();i++){
				CASH_ACCOUNT_ptr c = *i;
				fwprintf(fp,L"N%s\n",c->name);	//名称
				if(lstrlen(c->note)){
					wchar_t snote[1024];
					fwprintf(fp,L"D%s\n",C::removeWrap(snote,c->note));		//描述
				}
				//类型
				if(lstrcmp(c->name,LOADSTRING(IDS_STR_CASH).C_Str()) == 0){
					fwprintf(fp,L"TCash\n");
				}else{
					fwprintf(fp,L"TBank\n");
				}
				//写入结束符
				fwprintf(fp,L"^\n");
				m_Progressdlg.SetCurValue((cnt++)*100/size);
				m_Progressdlg.UpdateProgress();
			}
			fclose(fp);
			wsprintf(_lastErrMsg,
				LOADSTRING(IDS_STR_EXPORT_RESULT).C_Str(),
				size,
				LOADSTRING(IDS_STR_ACCOUNT).C_Str());
			m_Progressdlg.EndProgress();
		}
			break;
		case PROCESS_CATEGORY:
		{
			int cnt = 0;
			m_Progressdlg.SetTitle(LOADSTRING(IDS_STR_EXPORT_WAIT).C_Str());
			m_Progressdlg.BeginProgress(m_hWnd);
			//写入头部
			fwprintf(fp,L"!Type:Cat\n");
			list<CASH_CATEGORY_ptr>::iterator i = cash_db.list_category.begin();
			int size = cash_db.list_category.size();
			for(; i != cash_db.list_category.end();i++){
				CASH_CATEGORY_ptr c = *i;
				if(lstrcmp(c->name,LOADSTRING(IDS_STR_TRANSFER).C_Str()) == 0){
					continue;	//忽略转帐
				}
				int level = c->level;
				switch(level){
				case 0:
					fwprintf(fp,L"N%s\n",c->name);
					break;
				case 1:
					fwprintf(fp,L"N%s:%s\n",cash_db.categoryById(c->parentid)->name,c->name);
					break;
				case 2:
					fwprintf(fp,L"N%s:%s:%s\n",
						cash_db.categoryById(cash_db.categoryById(c->parentid)->parentid)->name,
						cash_db.categoryById(c->parentid)->name,
						c->name);
					break;
				default:
					break;
				}
				//写入类型
				wchar_t* stype;
				if(c->type == CT_INCOME){
					stype = L"I";
				}else{
					stype = L"E";
				}
				fwprintf(fp,L"%s\n",stype);
				//写入结束符
				fwprintf(fp,L"^\n");
				m_Progressdlg.SetCurValue((cnt++)*100/size);
				m_Progressdlg.UpdateProgress();
			}
			fclose(fp);
			wsprintf(_lastErrMsg,
				LOADSTRING(IDS_STR_EXPORT_RESULT).C_Str(),
				size,
				LOADSTRING(IDS_STR_CATEGORY).C_Str());
			m_Progressdlg.EndProgress();
		}
			break;
		case PROCESS_RECORD:
		{
			RECORDATE_t rd1,rd2;
			if(!_dateAll){
				ExportDate_t d1, d2;
				d1.Value = _sdate;
				d2.Value = _edate;
				rd1.Year = d1.Date.Year; rd1.Month = d1.Date.Month; rd1.Day = d1.Date.Day;
				rd2.Year = d2.Date.Year; rd2.Month = d2.Date.Month; rd2.Day = d2.Date.Day;
			}

			int cnt = 0;
			m_Progressdlg.SetTitle(LOADSTRING(IDS_STR_EXPORT_WAIT).C_Str());
			m_Progressdlg.BeginProgress(m_hWnd);
			int nsuccess = 0;

			int size = cash_db.list_account.size();

			list<CASH_ACCOUNT_ptr>::iterator i = cash_db.list_account.begin();
			for(;i != cash_db.list_account.end(); i++){
				CASH_ACCOUNT_ptr account = *i;
				bool baccount = false;
				bool ret;
				if(_dateAll){
					ret = cash_db.getRecordsByToAccount(account->id);
				}else{
					ret = cash_db.getRecordsByToAccount(account->id,&rd1,&rd2);
				}
				if(ret){	//转入
					if(!baccount){
						baccount = true;
						fwprintf(fp,L"!Account\n");
						fwprintf(fp,L"N%s\n",account->name);
						if(lstrcmp(account->name,LOADSTRING(IDS_STR_CASH).C_Str()) == 0){
							fwprintf(fp,L"TCash\n^\n!Type:Cash\n");
						}else{
							fwprintf(fp,L"TBank\n^\n!Type:Bank\n");
						}
						if(account->initval != 0){
							fwprintf(fp,L"D2009-01-01\n");
							fwprintf(fp,L"T%.2f\n",(double)account->initval/100);
							fwprintf(fp,L"U%.2f\n",(double)account->initval/100);
							fwprintf(fp,L"CX\nP账户初始金额\n");
							fwprintf(fp,L"L[%s]\n^\n",account->name);
						}
					}
					list<CASH_RECORD_ptr>::iterator j = cash_db.list_search_record.begin();
					for(; j != cash_db.list_search_record.end(); j++){
						CASH_RECORD_ptr record = *j;
						CMzString datestr = record->date;
						fwprintf(fp,L"D%s\n",datestr.SubStr(0,10).C_Str());
						fwprintf(fp,L"T%.2f\n",(double)record->amount/100);
						fwprintf(fp,L"U%.2f\n",(double)record->amount/100);
						if(lstrlen(record->note)){
							wchar_t snote[1024];
							fwprintf(fp,L"P%s\n",C::removeWrap(snote,record->note));		//描述
						}
						CASH_CATEGORY_ptr category = cash_db.categoryById(record->categoryid);
						fwprintf(fp,L"L[%s]\n^\n",cash_db.accountById(record->accountid)->name);
						nsuccess++;
					}
				}
				if(_dateAll){
					ret = cash_db.getRecordsByAccount(account->id);
				}else{
					ret = cash_db.getRecordsByAccount(account->id,&rd1,&rd2);
				}
				if(ret){	//找到帐号
					//输出帐号信息
					if(!baccount){
						baccount = true;
						fwprintf(fp,L"!Account\n");
						fwprintf(fp,L"N%s\n",account->name);
						if(lstrcmp(account->name,LOADSTRING(IDS_STR_CASH).C_Str()) == 0){
							fwprintf(fp,L"TCash\n^\n!Type:Cash\n");
						}else{
							fwprintf(fp,L"TBank\n^\n!Type:Bank\n");
						}
						if(account->initval != 0){
							fwprintf(fp,L"D2009-01-01\n");
							fwprintf(fp,L"T%.2f\n",(double)account->initval/100);
							fwprintf(fp,L"U%.2f\n",(double)account->initval/100);
							fwprintf(fp,L"CX\nP账户初始金额\n");
							fwprintf(fp,L"L[%s]\n^\n",account->name);
						}
					}
					list<CASH_RECORD_ptr>::iterator j = cash_db.list_search_record.begin();
					for(; j != cash_db.list_search_record.end(); j++){
						CASH_RECORD_ptr record = *j;
						CMzString datestr = record->date;
						fwprintf(fp,L"D%s\n",datestr.SubStr(0,10).C_Str());
						CASH_CATEGORY_ptr category = cash_db.categoryById(record->categoryid);
						if(category->type == CT_INCOME){	//收入
							fwprintf(fp,L"T%.2f\n",(double)record->amount/100);
							fwprintf(fp,L"U%.2f\n",(double)record->amount/100);
						}else{	//支出
							fwprintf(fp,L"T-%.2f\n",(double)record->amount/100);
							fwprintf(fp,L"U-%.2f\n",(double)record->amount/100);
						}
						if(lstrlen(record->note)){
							wchar_t snote[1024];
							fwprintf(fp,L"P%s\n",C::removeWrap(snote,record->note));		//描述
						}
						int level = category->level;
						if(record->isTransfer){
							level = 3;
						}
						switch(level){
						case 0:
							fwprintf(fp,L"L%s\n",category->name);
							break;
						case 1:
							fwprintf(fp,L"L%s:%s\n",cash_db.categoryById(category->parentid)->name,category->name);
							break;
						case 2:
							fwprintf(fp,L"L%s:%s:%s\n",
								cash_db.categoryById(cash_db.categoryById(category->parentid)->parentid)->name,
								cash_db.categoryById(category->parentid)->name,
								category->name);
							break;
						case 3:
							fwprintf(fp,L"L[%s]\n",cash_db.accountById(record->toaccountid)->name);
							break;
						default:
							break;
						}
						fwprintf(fp,L"^\n");
						nsuccess++;
					}
				}
				m_Progressdlg.SetCurValue((cnt++)*100/size);
				m_Progressdlg.UpdateProgress();
			}
			fclose(fp);
			wsprintf(_lastErrMsg,
				LOADSTRING(IDS_STR_EXPORT_RESULT).C_Str(),
				nsuccess,
				LOADSTRING(IDS_STR_RECORDS).C_Str());
			m_Progressdlg.EndProgress();
		}
			break;
		default:
			wsprintf(_lastErrMsg,L"未知导出格式，中止。");
			return false;
	}
	return true;
}
