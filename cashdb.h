#pragma once
// include the MZFC library header file
#include <mzfc_inc.h>
#include <windows.h>

#include <list>
#include "sqlite3\sqlite3.h"

using namespace std;

#ifdef _DEBUG
#define DEFAULT_DB		L"Program Files\\M8Cash\\cash.db"
#else
#define DEFAULT_DB		L"\\Disk\\Programs\\M8Cash\\cash.db"
//#define DEFAULT_DB		"Program Files\\M8Cash\\cash.db"
#endif
//老版本定义
#define OLD_TABLE_ACCOUNT	L"ACCOUNTS_v1"
#define OLD_TABEL_CATEGORY	L"CATEGORIES_v1"
#define V1_TABLE_RECORD	L"RECORDS_v1"
#define V2_TABLE_RECORD	L"RECORDS_v2"
//新版本定义
#define TABLE_ACCOUNT	L"ACCOUNTS_v1"
#define TABLE_CATEGORY	L"CATEGORIES_v1"
#define TABLE_RECORD	V2_TABLE_RECORD
#define TABLE_PERSON	L"PERSONS_v1"

#define ACCOUNT_TBL_ITEM L"ID numeric primary key,NAME text NOT NULL,INITIALVALUE integer,NOTE text,ISFIX integer"
#define CATEGORY_TBL_ITEM L"ID numeric primary key,NAME text NOT NULL,TYPE integer NOT NULL,PARENTID numeric NOT NULL,LEVEL integer"
#define RECORD_TBL_ITEM L"TRANSACTID numeric primary key,DATE text NOT NULL,AMOUNT integer NOT NULL,NOTE text, \
 ACCOUNTID numeric NOT NULL,CATGORYID numeric NOT NULL,TOACCOUNTID numeric,ISTRANSFER integer NOT NULL, \
 PERSON_ID numeric NOT NULL DEFAULT 0"
#define PERSON_TBL_ITEM L"ID INTEGER PRIMARY KEY  NOT NULL, NAME text not null, TYPE numeric not NULL"

#define ACCOUNT_TBL_INSERT L" \
 (ID,NAME,INITIALVALUE,NOTE,ISFIX) \
 values(%d,\"%s\",%d,\"%s\",%d)"
#define CATEGORY_TBL_INSERT L" \
 (ID,NAME,TYPE,PARENTID,LEVEL) \
 values(%d,\"%s\",%d,%d,%d)"
#define RECORD_TBL_INSERT L" \
(TRANSACTID,DATE,AMOUNT,NOTE,ACCOUNTID,CATGORYID,TOACCOUNTID,ISTRANSFER,PERSON_ID) \
 values(%d,\"%s\",%d,\"%s\",%d,%d,%d,%d,%d)"
#define PERSON_TBL_INSERT L"(ID,NAME, TYPE) values(%d,'%s',%d)"

#if 0
#define RECORD_DATETIME_FORMAT L"%04d-%02d-%02d %02d:%02d%02d"

#define RECORD_SEARCH_DATETIME_FORMAT1 L"%04d-%%"
#define RECORD_SEARCH_DATETIME_FORMAT2 L"%04d-%02d-%%"
#define RECORD_SEARCH_DATETIME_FORMAT3 L"%04d-%02d-%02d"
#endif

typedef struct _RECORDATE{
	DWORD Day	:	5;
	DWORD Month	:	4;
	DWORD Year	:	12;
} RECORDATE_t, *RECORDATE_ptr;


typedef union uRecordDate{
    DWORD Value;
	RECORDATE_t Date;
}uRecordDate_t,*uRecordDate_ptr;

typedef struct _RECORDTIME {
    WORD Year;
    WORD Month;
    WORD Day;
    WORD Hour;
    WORD Minute;
    WORD Second;
} RECORDTIME_t, *RECORDTIME_ptr;

typedef struct CONST_CASH_ACCOUNT{
    unsigned int id;
    UINT nameID;	//multilanguage id
    int initval;
    UINT noteID;
    char isfix;
}CONST_CASH_ACCOUNT_t, *CONST_CASH_ACCOUNT_ptr;

typedef struct CASH_ACCOUNT {
    unsigned int id;
    wchar_t* name;
    int initval;
    wchar_t* note;
    char isfix;
	CASH_ACCOUNT(){
		id = 0;
		initval = 0;
		isfix = 0;
		name = 0;
		note = 0;
	}
	~CASH_ACCOUNT(){
		if(name) delete[] name;
		if(note) delete[] note;
		name = 0;
		note = 0;
	}
} CASH_ACCOUNT_t, *CASH_ACCOUNT_ptr;

typedef enum CASH_TRANSACT_TYPE {
    CT_INCOME = 0,
    CT_OUTGOING = 1,
    CT_TRANSFER = 2,
} CASH_TRANSACT_TYPE_t;

typedef struct CONST_CASH_CATEGORY {
    unsigned int id;
    UINT nameID;	//multilanguage id
    CASH_TRANSACT_TYPE_t type;
    unsigned int parentid;
    char level; //0-1-2
} CONST_CASH_CATEGORY_t, *CONST_CASH_CATEGORY_ptr;

typedef struct CASH_CATEGORY {
    unsigned int id;
    wchar_t* name;
    CASH_TRANSACT_TYPE_t type;
    unsigned int parentid;
    char level; //0-1-2
	CASH_CATEGORY(){
		this->id = 0;
		this->type = CT_INCOME;
		this->parentid = 1;
		this->level = 2;
		this->name = 0;
	}
	~CASH_CATEGORY(){
		if(name) delete[] name;
		name = 0;
	}
} CASH_CATEGORY_t, *CASH_CATEGORY_ptr;

typedef struct CASH_RECORD {
    unsigned int transid;
    wchar_t* date;
    int amount; //*100
    wchar_t* note;
    unsigned int accountid;
    unsigned int categoryid;
    unsigned int toaccountid;
    char isTransfer;
	unsigned int personid;
	CASH_RECORD(){
		this->transid = 0;
		this->date = 0;
		this->amount = 0;
		this->note = 0;
		this->accountid = 0;
		this->categoryid = 0;
		this->toaccountid = 0;
		this->isTransfer = 0;
		this->personid = 0;
	}
	~CASH_RECORD(){
		if(date) delete[] date;
		if(note) delete[] note;
		date = 0;
		note = 0;
	}
} CASH_RECORD_t, *CASH_RECORD_ptr;

typedef enum CASH_PERSON_TYPE {
	PRSN_ALL	=	-1,
    PRSN_FAMILY		=	0,
    PRSN_CONTACT	=	1,
    PRSN_INSTITUTION	=	2,
} CASH_PERSON_TYPE_t;

typedef struct CASH_PERSON {
	unsigned int id;
	wchar_t *name;
	CASH_PERSON_TYPE_t type;
	CASH_PERSON(){
		id = 0;
		name = 0;
		type = PRSN_FAMILY;
	}
	~CASH_PERSON(){
		if(name) delete [] name;
	}
} CASH_PERSON_t, *CASH_PERSON_ptr;

class clsCASHDB {
public:
    clsCASHDB(char* dbpath = 0);
    ~clsCASHDB();
public:
	bool connect(wchar_t *path);
	bool disconnect() { disconnectDatabase(); return true; }
	void recover();
	bool load();
	bool decrypt(wchar_t* pwd,int len);
	bool encrypt(wchar_t* pwd,int len);
	void clean();	//clean rabish
private:
	//主要为了避免超时显示密码界面后的重复载入问题@2009-10-9
	void cleanAccountList();	//载入账户前清除垃圾,防止重复载入
	void cleanCategoryList();	//载入类别前清除垃圾,防止重复载入
public:
	//(根据名称)检测是否有重复类别，如果有则返回类别id，无则返回-1
	//返回值：冲突情况， true表示冲突
	int checkDupCategory(CASH_CATEGORY_ptr,bool *bconflict = 0);
	//检测是否有重复帐号，如果有则返回账号id，无则返回-1
	int checkDupAccount(CASH_ACCOUNT_ptr,bool *bconflict = 0);
	//检测是否有重复记录，如果有则返回记录id，无则返回-1
	int checkDupRecord(CASH_RECORD_ptr,bool *bconflict = 0);
public:
	bool isChildCategory(int childId, int parentId);
public:
	int getAccountSize();
	int getCategorySize();
	int getRecordSize();

	int getMaxAccountID();
	int getMaxCategoryID();
	int getMaxRecordID();

	uRecordDate_t getMinRecordDate();
	uRecordDate_t getMaxRecordDate();
public:
    int appendAccount(CASH_ACCOUNT_ptr);
    int appendCategory(CASH_CATEGORY_ptr);
    int appendTransaction(CASH_RECORD_ptr);

    int deleteCategoryByIndex(int);
	int deleteAccountByIndex(int);
	int deleteTransactionByIndex(int);

	int deleteCategoryById(int);
	int deleteAccountById(int);
	int deleteTransactionById(int);

	int updateCategory(CASH_CATEGORY_ptr);
	int updateAccount(CASH_ACCOUNT_ptr);
	int updateTransaction(CASH_RECORD_ptr);

	int AccountBalanceById(int id,RECORDATE_ptr date = 0, bool binit = true, RECORDATE_ptr datend = 0);
	int AccountInById(int,RECORDATE_ptr date = 0, RECORDATE_ptr datend = 0);
	int AccountOutById(int,RECORDATE_ptr date = 0, RECORDATE_ptr datend = 0);
	void AccountInOutById(int &, int &,int,RECORDATE_ptr date = 0, RECORDATE_ptr datend = 0);
public:
    bool updateAccountList(int,CASH_ACCOUNT_ptr);
    bool updateCategoryList(int,CASH_CATEGORY_ptr);

	CASH_ACCOUNT_ptr accountById(int id);
	CASH_CATEGORY_ptr categoryById(int id);
	bool recordById(int id,CASH_RECORD_ptr pr);

	wchar_t* getAccountNameById(int id);
	wchar_t* getCategoryNameById(int id);
    wchar_t* getCategoryFullNameById(int id);
public:
	int getPersonTypeNameStrID(CASH_PERSON_TYPE_t tid);
	const int* getPersonTypeNameStrSet(int *nsize);
	wchar_t* getPersonNameById(int id);
	CASH_PERSON_ptr personById(int id);
	bool appendPerson(CASH_PERSON_ptr);
	bool updatePerson(CASH_PERSON_ptr);
	bool deletePersonById(int id);
	//检测是否有重复记录，如果有则返回记录id，无则返回-1
	int	checkDupPerson(CASH_PERSON_ptr);
	bool getPersonByType(CASH_PERSON_TYPE_t);
	int getMaxPersonID();

protected:
	bool checkChildCategory(CASH_CATEGORY_ptr &a,CASH_CATEGORY_ptr &b);
	void sortRecordsByDate(bool latest = true) {}
	void sortRecordsByAccount() {}
	void sortRecordsByCategory() {}
public:
    list<CASH_ACCOUNT_ptr> list_account;
    list<CASH_CATEGORY_ptr> list_category;
    //list<CASH_RECORD_ptr> list_record;

    list<CASH_ACCOUNT_ptr> list_search_account;
    list<CASH_CATEGORY_ptr> list_search_category;
    list<CASH_RECORD_ptr> list_search_record;

	 list<CASH_PERSON_ptr> list_search_person;
    //sqlite operations
private:
    sqlite3* db;
	sqlite3_stmt* pStmt;
	const void* pzTail;
	wchar_t sqlcmdw[512];
protected:
    void connectDatabase(const wchar_t*);
    void disconnectDatabase();
    void createDefaultDatabase();
	bool decrytpDatabase(const char* pwd,int len);	//true: successful
	bool setDatabasePassword(const char* pwd,int len);	//true: successful
public:
    bool loadAccounts();
    bool loadCategories();
	bool loadRecords();
	bool searchRecords(bool blist = true);	//blist: true: store to search list
	bool searchAccounts();
	bool searchCategories();
	bool searchPerson();
private:
	void clearAccountSearchResults();
	void clearCategorySearchResults();
	void clearRecordSearchResults();
	void clearPersonSearchResults();
private:
	void formatDate(RECORDATE_ptr date, RECORDATE_ptr datend,
				RECORDATE_t &d1, RECORDATE_t &d2);
public:
	bool getRecordsByDate(RECORDATE_ptr date = 0, RECORDATE_ptr datend = 0);
	//orderby: 0: none			
	//			1: date
	//			2: category
	//			4: account
	bool getRecordsByDate_v2(RECORDATE_ptr date = 0, RECORDATE_ptr datend = 0, UINT orderby = 1);
	bool getRecordsByCategory(int id, RECORDATE_ptr date = 0, RECORDATE_ptr datend = 0);
	bool getRecordsByAccount(int id, RECORDATE_ptr date = 0, RECORDATE_ptr datend = 0);
	bool getRecordsByToAccount(int id, RECORDATE_ptr date = 0, RECORDATE_ptr datend = 0);
public:
	bool getCategoryByLevel(int level);

//数据库升级等
private:
	bool TableExists(wchar_t* tablename);
	//检查数据库版本
	//false:需要更新数据库
	bool checkDatabaseVersion_v1();
	//更新数据库版本
	void updateDatabaseVersion_v1();
	//恢复默认分类名称
	void restoreDefaultCategory();
	//增加人员分类,需要在RECORDS_v2中增加PERSON_ID字段 2009-9-20
	bool addFieldPerson();
public:
	void versionUpdate(HWND hWnd){
		//检查记录版本
		if(checkDatabaseVersion_v1()){
			MzAutoMsgBoxEx(hWnd,L"数据库升级中，请稍后",1000);
			MzBeginWaitDlg(hWnd);
			updateDatabaseVersion_v1();
			reorgDatebase();
			MzEndWaitDlg();
		}
		addFieldPerson();
	}
public:
	int beginTrans(){
		bool rc = false;
		wsprintf(sqlcmdw,L"begin transaction;");
		if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
			sqlite3_step(pStmt);
		}
		sqlite3_finalize(pStmt);
		return rc;
	}
	int commitTrans(){
		bool rc = false;
		wsprintf(sqlcmdw,L"commit transaction;");
		if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
			sqlite3_step(pStmt);
		}
		sqlite3_finalize(pStmt);
		return rc;
	}
	bool reorgDatebase(){
		bool rc = false;
		wsprintf(sqlcmdw,L"VACUUM");
		if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
			sqlite3_step(pStmt);
		}
		sqlite3_finalize(pStmt);
		return rc;
	}
};

