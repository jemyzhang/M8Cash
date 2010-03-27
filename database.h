#pragma once
// include the MZFC library header file
#include <mzfc_inc.h>
#include <windows.h>

#include <list>
using namespace std;

#include <sqlite3x.hpp>
using namespace sqlite3x;

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
		id = 0; initval = 0;
		isfix = 0; name = 0;
		note = 0;
	}
	~CASH_ACCOUNT(){
		if(name) { delete[] name; name = 0; }
		if(note) { delete[] note; note = 0; }
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
		this->id = 0; this->type = CT_INCOME;
		this->parentid = 1; this->level = 2;
		this->name = 0; 
	}
	~CASH_CATEGORY(){
		if(name) { delete[] name; name = 0; }
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
		this->transid = 0; this->date = 0;
		this->amount = 0; this->note = 0;
		this->accountid = 0; this->categoryid = 0;
		this->toaccountid = 0; this->isTransfer = 0;
		this->personid = 0;
	}
	~CASH_RECORD(){
		if(date) { delete[] date; date = 0; }
		if(note) { delete[] note; note = 0; }
	}
} CASH_TRANSACT_t, *CASH_TRANSACT_ptr;

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
		if(name) { delete [] name; name = 0; }
	}
} CASH_PERSON_t, *CASH_PERSON_ptr;

class clsCASHDB {
public:
    clsCASHDB();
    ~clsCASHDB();
public:
	bool connect();
	bool disconnect();
	void recover();
	bool load();
	bool checkpwd(wchar_t* pwd,int len);
	bool decrypt(wchar_t* pwd,int len);
	bool encrypt(wchar_t* pwd,int len);
	void clean();	//clean rabish


public:	//账户相关
	bool createDefaultAccounts();	//创建默认账户
    bool appendAccount(CASH_ACCOUNT_ptr);
	bool updateAccount(CASH_ACCOUNT_ptr);
	int deleteAccountById(int);
	int deleteAccountByIndex(int);
	int getAccountSize();
	int getMaxAccountID();
	//检测是否有重复帐号，如果有则返回账号id，无则返回-1
	int checkDupAccount(CASH_ACCOUNT_ptr,bool *bconflict = 0);
private:
	void cleanAccountList();	//载入账户前清除垃圾,防止重复载入
    bool updateAccountList(int,CASH_ACCOUNT_ptr);



public: //分类相关
    bool loadCategories();
    bool appendCategory(CASH_CATEGORY_ptr);
	bool updateCategory(CASH_CATEGORY_ptr);
    int deleteCategoryByIndex(int);
	int deleteCategoryById(int);
	CASH_CATEGORY_ptr categoryById(int id);
	wchar_t* getCategoryNameById(int id);
    wchar_t* getCategoryFullNameById(int id);
	int getCategorySize();
	int getMaxCategoryID();
	bool isChildCategory(int childId, int parentId);
	//(根据名称)检测是否有重复类别，如果有则返回类别id，无则返回-1
	//返回值：冲突情况， true表示冲突
	int checkDupCategory(CASH_CATEGORY_ptr,bool *bconflict = 0);
private:
	bool updateCategoryList(int,CASH_CATEGORY_ptr);
	void clearCategorySearchResults();
	bool createDefaultCategories();
	//恢复默认分类名称
	bool restoreDefaultCategory();
	void cleanCategoryList();	//载入类别前清除垃圾,防止重复载入
	wchar_t catfullname[128];
public:
    list<CASH_CATEGORY_ptr> list_category;
    list<CASH_CATEGORY_ptr> list_search_category;



public: //人员相关
	bool createDefaultPersons();
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



public: //交易明细相关
	bool createDefaultTransaction();
    bool appendTransaction(CASH_TRANSACT_ptr);
	bool updateTransaction(CASH_TRANSACT_ptr);
	bool deleteTransactionById(int);
	int deleteTransactionByIndex(int);
	int getTransactionSize();
	int getMaxTransactionID();
	uRecordDate_t getMinTransactionDate();
	uRecordDate_t getMaxTransactionDate();
	//检测是否有重复记录，如果有则返回记录id，无则返回-1
	int checkDupTransaction(CASH_TRANSACT_ptr);

	int AccountBalanceById(int id,RECORDATE_ptr date = 0, bool binit = true, RECORDATE_ptr datend = 0);
	int AccountInById(int,RECORDATE_ptr date = 0, RECORDATE_ptr datend = 0);
	int AccountOutById(int,RECORDATE_ptr date = 0, RECORDATE_ptr datend = 0);
	void AccountInOutById(int &, int &,int,RECORDATE_ptr date = 0, RECORDATE_ptr datend = 0);
public:

	CASH_ACCOUNT_ptr accountById(int id);
	bool recordById(int id,CASH_TRANSACT_ptr pr);

	wchar_t* getAccountNameById(int id);
public:

protected:
	bool checkChildCategory(CASH_CATEGORY_ptr &a,CASH_CATEGORY_ptr &b);
	void sortRecordsByDate(bool latest = true) {}
	void sortRecordsByAccount() {}
	void sortRecordsByCategory() {}
public:
    list<CASH_ACCOUNT_ptr> list_account;
    //list<CASH_TRANSACT_ptr> list_record;

    list<CASH_ACCOUNT_ptr> list_search_account;
    list<CASH_TRANSACT_ptr> list_search_record;

	 list<CASH_PERSON_ptr> list_search_person;
    //sqlite operations
private:
 	sqlite3_connection sqlconn;
    wchar_t db_path[MAX_PATH];
protected:
    void createDefaultDatabase();
public:
    bool loadAccounts();
	bool loadTransactions();
	bool searchTransactions(sqlite3_command& cmd);
	bool searchAccounts(sqlite3_command& cmd);
	bool searchCategories(sqlite3_command& cmd);
	bool searchPerson(sqlite3_command& cmd);
private:
	void clearAccountSearchResults();
	void clearTransactionSearchResults();
	void clearPersonSearchResults();
private:
	void formatDate(RECORDATE_ptr date, RECORDATE_ptr datend,
				RECORDATE_t &d1, RECORDATE_t &d2);
public:
	bool getTransactionsByDate(RECORDATE_ptr date = 0, RECORDATE_ptr datend = 0);
	//orderby: 0: none			
	//			1: date
	//			2: category
	//			4: account
	bool getTransactionsByDate_v2(RECORDATE_ptr date = 0, RECORDATE_ptr datend = 0, UINT orderby = 1);
	bool getTransactionsByCategory(int id, RECORDATE_ptr date = 0, RECORDATE_ptr datend = 0);
	bool getTransactionsByAccount(int id, RECORDATE_ptr date = 0, RECORDATE_ptr datend = 0);
	bool getTransactionsByToAccount(int id, RECORDATE_ptr date = 0, RECORDATE_ptr datend = 0);
public:
	bool getCategoryByLevel(int level);

//数据库升级等
#if 1
private:
	bool TableExists(wchar_t* tablename);
	//检查数据库版本
	//false:需要更新数据库
	bool checkDatabaseVersion_v1();
	//更新数据库版本
	void updateDatabaseVersion_v1();
	//增加人员分类,需要在RECORDS_v2中增加PERSON_ID字段 2009-9-20
	bool addFieldPerson();
public:
	void versionUpdate(HWND hWnd){
		//检查记录版本
		if(checkDatabaseVersion_v1()){
			MzMessageAutoBoxV2(hWnd,L"数据库升级中，请稍后");
			MzBeginWaitDlg(hWnd,NULL,TRUE);
			updateDatabaseVersion_v1();
			reorgDatebase();
			MzEndWaitDlg();
		}
		addFieldPerson();
	}
#endif
public:
	bool beginTrans();
	bool commitTrans();
	bool reorgDatebase();
};

