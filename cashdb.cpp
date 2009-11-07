#include "cashdb.h"
#include "pinyin_sort.h"
#include "..\MzCommon\MzCommon.h"
using namespace MzCommon;

#include "m8cash.h"
#pragma comment(lib,"libsqlitece_s.lib")

clsCASHDB::clsCASHDB(char* dbpath) {

}

clsCASHDB::~clsCASHDB() {
    disconnectDatabase();
	clean();
}

bool clsCASHDB::connect(wchar_t* dbpath){
	bool rc = false;
	if(File::FileExists(dbpath)){	//file exists
		rc = true;
	}
	connectDatabase(dbpath);
	//createDefaultDatabase(dbpath);
	return rc;
}
void clsCASHDB::recover(){
	createDefaultDatabase();
	restoreDefaultCategory();
}

bool clsCASHDB::load(){
	if(!loadAccounts() ||
		!loadCategories()){ 
		return false;
	}
	return true;
}

bool clsCASHDB::decrypt(wchar_t* pwd, int len){
	char* temp = new char[len*2+1];
	int bytecnt = 0;
	wchar_t *p = pwd;
	char* b_pwd = temp;
	for(int i = 0; i < len; i++){
		wchar_t w = *p++;
		if(w&0xff){
			*b_pwd++ = w&0xff;
			bytecnt++;
		}
		if((w>>8)&0xff){
			*b_pwd++ = (w>>8)&0xff;
			bytecnt++;
		}
	}
	*b_pwd = '\0';
	bool ret = decrytpDatabase(temp,bytecnt);
	delete temp;
	return ret;
}

bool clsCASHDB::encrypt(wchar_t* pwd, int len){
	char* temp = new char[len*2+1];
	int bytecnt = 0;
	wchar_t *p = pwd;
	char* b_pwd = temp;
	for(int i = 0; i < len; i++){
		wchar_t w = *p++;
		if(w&0xff){
			*b_pwd++ = w&0xff;
			bytecnt++;
		}
		if((w>>8)&0xff){
			*b_pwd++ = (w>>8)&0xff;
			bytecnt++;
		}
	}
	*b_pwd = '\0';
	bool ret =  setDatabasePassword(temp,bytecnt);
	delete temp;
	return ret;
}

void clsCASHDB::cleanAccountList(){
	if(list_account.size()){
		list<CASH_ACCOUNT_ptr>::iterator c = list_account.begin();
		for(;c != list_account.end(); c++){
			delete *c;
		}
		list_account.clear();
	}
}

void clsCASHDB::cleanCategoryList(){
	if(list_category.size()){
		list<CASH_CATEGORY_ptr>::iterator t = list_category.begin();
		for(;t != list_category.end(); t++){
			delete *t;
		}
		list_category.clear();
	}
}

void clsCASHDB::clean(){
	cleanAccountList();
	cleanCategoryList();
}

int clsCASHDB::appendAccount(CASH_ACCOUNT_ptr acc) {
    int rc = 0;
    wchar_t cmdtemp[256];

	acc->id = getMaxAccountID() + 1;

	wsprintf(cmdtemp, L"INSERT INTO %s %s", TABLE_ACCOUNT, ACCOUNT_TBL_INSERT);
    wsprintf(sqlcmdw, cmdtemp,
            acc->id,
            acc->name,
            acc->initval,
            acc->note,
            acc->isfix);
	rc = sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail);
    if (!rc) {
		rc = sqlite3_step(pStmt);
		//create new account
		CASH_ACCOUNT_ptr newAcc = new CASH_ACCOUNT_t;
		newAcc->id = acc->id;
		C::newstrcpy(&newAcc->name,acc->name);
		C::newstrcpy(&newAcc->note,acc->note);
		newAcc->initval = acc->initval;
		newAcc->isfix = acc->isfix;
        list_account.push_back(newAcc);
    }
	rc = sqlite3_finalize(pStmt);
    return rc;
}

int clsCASHDB::appendCategory(CASH_CATEGORY_ptr cat) {
    int rc = 0;
    wchar_t cmdtemp[256];

	cat->id = getMaxCategoryID() + 1;

    wsprintf(cmdtemp, L"INSERT INTO %s %s", TABLE_CATEGORY, CATEGORY_TBL_INSERT);
    wsprintf(sqlcmdw, cmdtemp,
            cat->id,
            cat->name,
            cat->type,
            cat->parentid,
            cat->level);
    rc = sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail);
    if (rc == SQLITE_OK) {
		sqlite3_step(pStmt);
		CASH_CATEGORY_ptr newCat = new CASH_CATEGORY_t;
		newCat->id = cat->id;
		newCat->level = cat->level;
		C::newstrcpy(&newCat->name,cat->name);
		newCat->parentid = cat->parentid;
		newCat->type = cat->type;
        list_category.push_back(newCat);
    }
	sqlite3_finalize(pStmt);
    return rc;
}

int clsCASHDB::appendTransaction(CASH_RECORD_ptr r) {
    int rc = 0;
    wchar_t cmdtemp[256];

	r->transid = getMaxRecordID() + 1;

    wsprintf(cmdtemp, L"INSERT INTO %s %s", TABLE_RECORD, RECORD_TBL_INSERT);
    wsprintf(sqlcmdw, cmdtemp,
		r->transid,
		r->date,
		r->amount,
		r->note,
		r->accountid,
		r->categoryid,
		r->toaccountid,
		r->isTransfer,
		r->personid);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
		sqlite3_step(pStmt);
    }
	sqlite3_finalize(pStmt);
    return rc;
}

bool clsCASHDB::loadRecords() {
	wsprintf(sqlcmdw,L"select * from %s",TABLE_RECORD);
	return searchRecords();
}

bool clsCASHDB::loadCategories() {
	//载入前清除垃圾,防止重复载入
	cleanCategoryList();

    bool rc = true;

    wsprintf(sqlcmdw, L"select * from %s order by NAME collate pinyin", TABLE_CATEGORY);
    if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
        while (sqlite3_step(pStmt) == SQLITE_ROW) {
            CASH_CATEGORY_ptr c = new CASH_CATEGORY_t;
            c->id = sqlite3_column_int(pStmt, 0);

			C::newstrcpy(&c->name,(LPWSTR) sqlite3_column_text16(pStmt, 1));
            c->type = (CASH_TRANSACT_TYPE_t) sqlite3_column_int(pStmt, 2);
            c->parentid = sqlite3_column_int(pStmt, 3);
            c->level = sqlite3_column_int(pStmt, 4);
            list_category.push_back(c);
        }
	}else{
		rc = false;
	}
    sqlite3_finalize(pStmt);
	return rc;
}

bool clsCASHDB::loadAccounts() {
	//载入前清除垃圾,防止重复载入
	cleanAccountList();

    bool rc = true;
	wsprintf(sqlcmdw,L"select * from %s",TABLE_ACCOUNT);
    if (sqlite3_prepare16(db, (const void*)sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
        while (sqlite3_step(pStmt) == SQLITE_ROW) {
            CASH_ACCOUNT_ptr accs = new CASH_ACCOUNT_t;

			accs->id = sqlite3_column_int(pStmt, 0);

			C::newstrcpy(&accs->name,(LPWSTR) sqlite3_column_text16(pStmt, 1));
            accs->initval = sqlite3_column_int(pStmt, 2);
			C::newstrcpy(&accs->note,(LPWSTR) sqlite3_column_text16(pStmt, 3));
            accs->isfix = sqlite3_column_int(pStmt, 4);

            list_account.push_back(accs);
        }
    }else{
		rc = false;
	}
    sqlite3_finalize(pStmt);
	return rc;
}


//sqlite database operation
unsigned short zName[] = {'p', 'i', 'n', 'y', 'i', 'n', 0};

void clsCASHDB::connectDatabase(const wchar_t * dbfile) {
    const wchar_t* f = dbfile;
    if (f == NULL) {
        f = DEFAULT_DB;
    }
    int rc = sqlite3_open16(f, &db);
	//创建拼音搜索库
	sqlite3_create_collation16(db, (const char*)zName, SQLITE_UTF16, 0, pinyin_cmp);

    return;
}

void clsCASHDB::disconnectDatabase() {
    sqlite3_close(db);
    return;
}

void clsCASHDB::restoreDefaultCategory() {
	int rc = 0;
	wsprintf(sqlcmdw, L"UPDATE %s SET NAME=\"%s\" WHERE LEVEL=0 and TYPE=%d and PARENTID=-1", 
		TABLE_CATEGORY,
		LOADSTRING(IDS_STR_EXPENSE).C_Str(),
		CT_OUTGOING);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
		sqlite3_step(pStmt);
    }
	sqlite3_finalize(pStmt);

	wsprintf(sqlcmdw, L"UPDATE %s SET NAME=\"%s\" WHERE LEVEL=0 and TYPE=%d and PARENTID=-1", 
		TABLE_CATEGORY,
		LOADSTRING(IDS_STR_INCOME).C_Str(),
		CT_INCOME);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
		sqlite3_step(pStmt);
    }
	sqlite3_finalize(pStmt);

	wsprintf(sqlcmdw, L"UPDATE %s SET NAME=\"%s\" WHERE LEVEL=2 and TYPE=%d and PARENTID=-1", 
		TABLE_CATEGORY,
		LOADSTRING(IDS_STR_TRANSFER).C_Str(),
		CT_TRANSFER);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
		sqlite3_step(pStmt);
    }
	sqlite3_finalize(pStmt);
}

void clsCASHDB::createDefaultDatabase() {
    int rc = 0;
    //what ever create db
    //UNIQUE solved duplication problem
    //create account table
    wsprintf(sqlcmdw, L"CREATE TABLE %s (%s);", TABLE_ACCOUNT, ACCOUNT_TBL_ITEM);

    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
		sqlite3_step(pStmt);
    }
	sqlite3_finalize(pStmt);

    wsprintf(sqlcmdw, L"CREATE TABLE %s (%s);", TABLE_CATEGORY, CATEGORY_TBL_ITEM);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
		sqlite3_step(pStmt);
    }
	sqlite3_finalize(pStmt);

    wsprintf(sqlcmdw, L"CREATE TABLE %s (%s);", TABLE_RECORD, RECORD_TBL_ITEM);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
		sqlite3_step(pStmt);
    }
	sqlite3_finalize(pStmt);

    wsprintf(sqlcmdw, L"CREATE TABLE %s (%s);", TABLE_PERSON, PERSON_TBL_ITEM);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
		sqlite3_step(pStmt);
    }
	sqlite3_finalize(pStmt);

    CONST_CASH_ACCOUNT_t default_accounts[] = {
        {0, IDS_STR_CASH, 0, IDS_STR_CASH, 1},
        {1, IDS_STR_CREDIT, 0, IDS_STR_CREDIT, 1},
        {2, IDS_STR_SAVING, 0, IDS_STR_SAVING, 1},
    };

    CONST_CASH_CATEGORY_t default_category[] = {
        {0, IDS_STR_EXPENSE, CT_OUTGOING, -1, 0},
        {1, IDS_STR_INCOME, CT_INCOME, -1, 0},
        {2, IDS_STR_TRANSFER, CT_TRANSFER, -1, 2},	//设置为无子目录
    };

    int aSets = sizeof (default_accounts) / sizeof (CASH_ACCOUNT_t);
    wchar_t cmdtemp[256];
    for (int i = 0; i < aSets; i++) {
        wsprintf(cmdtemp, L"INSERT INTO %s %s", TABLE_ACCOUNT, ACCOUNT_TBL_INSERT);
        wsprintf(sqlcmdw, cmdtemp,
                default_accounts[i].id,
				LOADSTRING(default_accounts[i].nameID).C_Str(),
                default_accounts[i].initval,
				LOADSTRING(default_accounts[i].noteID).C_Str(),
                default_accounts[i].isfix);
        rc = sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail);
        if (rc == SQLITE_OK) {
			sqlite3_step(pStmt);
		}
		sqlite3_finalize(pStmt);
    }
    int cSets = sizeof (default_category) / sizeof (CASH_CATEGORY_t);
    for (int i = 0; i < cSets; i++) {
        wsprintf(cmdtemp, L"INSERT INTO %s %s", TABLE_CATEGORY, CATEGORY_TBL_INSERT);
        wsprintf(sqlcmdw, cmdtemp,
                default_category[i].id,
                LOADSTRING(default_category[i].nameID).C_Str(),
                default_category[i].type,
                default_category[i].parentid,
                default_category[i].level);
        rc = sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail);
        if (rc == SQLITE_OK){
			sqlite3_step(pStmt);
		}
		sqlite3_finalize(pStmt);
    }

	//初始化人员类别表
	wsprintf(sqlcmdw, L"INSERT INTO %s "PERSON_TBL_INSERT,
		TABLE_PERSON,
		0,
		LOADSTRING(IDS_STR_PERSON_DEFAULT).C_Str(),
		PRSN_FAMILY);
	rc = sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail);
	if (rc == SQLITE_OK){
		sqlite3_step(pStmt);
	}
	sqlite3_finalize(pStmt);

	return;
}

bool clsCASHDB::updateAccountList(int id,CASH_ACCOUNT_ptr t) {
    list<CASH_ACCOUNT_ptr>::iterator i = list_account.begin();
	CASH_ACCOUNT_ptr a;
	for (; i != list_account.end(); i++){
		a = *i;
		if(a->id == id){
			if(a == t) return true;
			a->initval = t->initval;
			a->isfix = t->isfix;
			C::newstrcpy(&a->name,t->name);
			C::newstrcpy(&a->note,t->note);
			return true;
		}
	}
    return false;
}

bool clsCASHDB::updateCategoryList(int id,CASH_CATEGORY_ptr t) {
    list<CASH_CATEGORY_ptr>::iterator i = list_category.begin();
	CASH_CATEGORY_ptr a;
	for (; i != list_category.end(); i++){
		a = *i;
		if(a->id == id){
			if(a == t) return true;
			a->level = t->level;
			a->parentid = t->parentid;
			a->type = t->type;
			C::newstrcpy(&a->name,t->name);
			return true;
		}
	}
    return false;
}

CASH_ACCOUNT_ptr clsCASHDB::accountById(int id) {
    list<CASH_ACCOUNT_ptr>::iterator i = list_account.begin();
	CASH_ACCOUNT_ptr a;
	for (; i != list_account.end(); i++){
		a = *i;
		if(a->id == id){
			return a;
		}
	}
	return list_account.front();
}

CASH_CATEGORY_ptr clsCASHDB::categoryById(int id) {
    list<CASH_CATEGORY_ptr>::iterator i = list_category.begin();
	CASH_CATEGORY_ptr a;
	for (; i != list_category.end(); i++){
		a = *i;
		if(a->id == id){
			return a;
		}
	}
	return list_category.front();
}

bool clsCASHDB::recordById(int id,CASH_RECORD_ptr pr) {
	bool retval = false;
	if(pr == 0) return retval;

	wsprintf(sqlcmdw,L"select * from %s where TRANSACTID=%d",TABLE_RECORD,id);
	if(searchRecords()){
		retval = true;
		CASH_RECORD_ptr r = *list_search_record.begin();
		pr->accountid = r->accountid;
		pr->amount = r->amount;
		pr->categoryid = r->categoryid;
		pr->isTransfer = r->isTransfer;
		pr->toaccountid = r->toaccountid;
		pr->transid = r->transid;
		pr->personid = r->personid;
		C::newstrcpy(&pr->date,r->date);
		C::newstrcpy(&pr->note,r->note);
	}
	return retval;
}

wchar_t* clsCASHDB::getAccountNameById(int id) {
    return accountById(id)->name;
}

wchar_t* clsCASHDB::getCategoryNameById(int id) {
    return categoryById(id)->name;
}


//from root
wchar_t* clsCASHDB::getCategoryFullNameById(int id) {
    static wchar_t fullname[512];
    wchar_t* nodename[3]; //child max depth = 3
    int depth = 0;

	int nodeidx = id;
    CASH_CATEGORY_ptr c;
    do {
        c = categoryById(nodeidx);
		nodename[depth++] = c->name;
		nodeidx = c->parentid;
    } while (nodeidx != -1);
    memset(fullname, 0, sizeof (fullname));
    for (int i = depth-1; i >= 0; i--) {
        lstrcat(fullname, nodename[i]);
        if (i != 0) {
            lstrcat(fullname, L" - ");
        }
    }
    return fullname;
}


int clsCASHDB::deleteCategoryById(int id){
	CASH_CATEGORY_ptr beremoved = categoryById(id);
	if(beremoved->level == 0) return -2;
	if(beremoved->level<2){	//check if there is a child
	    list<CASH_CATEGORY_ptr>::iterator i = list_category.begin();
		for (; i!=list_category.end(); i++){
			CASH_CATEGORY_ptr chk = *i;
			if(chk->parentid == id){
				return -1;
			}
		}
	}
	//check if is being used
	wsprintf(sqlcmdw,L"select * from %s where CATGORYID=%d",TABLE_RECORD,id);
	if(searchRecords(false)){
		return -1;
	}

	list_category.remove(beremoved);

	wsprintf(sqlcmdw,L"DELETE FROM %s WHERE ID=%d",TABLE_CATEGORY,beremoved->id);
	int rc = sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail);
	if(rc == SQLITE_OK){
		rc = sqlite3_step(pStmt);
	}
	sqlite3_finalize(pStmt);
	delete beremoved;
	return 0;
}

int clsCASHDB::deleteAccountById(int id){
	const CASH_ACCOUNT_ptr beremoved = accountById(id);
	//check if is being used
	wsprintf(sqlcmdw,L"select * from %s where ACCOUNTID=%d or TOACCOUNTID=%d",TABLE_RECORD,id,id);
	if(searchRecords(false)){
		return -1;
	}

	list_account.remove(beremoved);

	wsprintf(sqlcmdw,L"DELETE FROM %s WHERE ID=%d",TABLE_ACCOUNT,beremoved->id);
	int rc = sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail);
	if(rc == SQLITE_OK){
		rc = sqlite3_step(pStmt);
	}
	sqlite3_finalize(pStmt);
	delete beremoved;
	return 0;
}

int clsCASHDB::deleteTransactionById(int id){
	wsprintf(sqlcmdw,L"DELETE FROM %s WHERE TRANSACTID=%d",TABLE_RECORD,id);
	int rc = sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail);
	if(rc == SQLITE_OK){
		rc = sqlite3_step(pStmt);
	}
	sqlite3_finalize(pStmt);
	return 0;
}

int clsCASHDB::updateAccount(CASH_ACCOUNT_ptr a){
	wsprintf(sqlcmdw,L"UPDATE %s SET \
					  NAME=\"%s\",NOTE=\"%s\",INITIALVALUE=%d \
					  WHERE ID=%d",
					  TABLE_ACCOUNT,
					  a->name,a->note,a->initval,
					  a->id);
	int rc = sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail);
	if(rc == SQLITE_OK){
		rc = sqlite3_step(pStmt);
	}
	sqlite3_finalize(pStmt);

	updateAccountList(a->id, a);
	return 0;
}

int clsCASHDB::updateCategory(CASH_CATEGORY_ptr a){
	wsprintf(sqlcmdw,L"UPDATE %s SET NAME=\"%s\",LEVEL=%d,PARENTID=%d,TYPE=%d WHERE ID=%d",
		TABLE_CATEGORY,
		a->name,a->level,a->parentid,a->type,
		a->id);
	int rc = sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail);
	if(rc == SQLITE_OK){
		rc = sqlite3_step(pStmt);
	}
	sqlite3_finalize(pStmt);

	updateCategoryList(a->id, a);
	return 0;
}

int clsCASHDB::updateTransaction(CASH_RECORD_ptr a){
	wsprintf(sqlcmdw,L"UPDATE %s SET \
					  NOTE=\"%s\",DATE=\"%s\",AMOUNT=%d,ACCOUNTID=%d,CATGORYID=%d,TOACCOUNTID=%d,ISTRANSFER=%d, \
					  PERSON_ID=%d \
					  WHERE TRANSACTID=%d",
					  TABLE_RECORD,
					  a->note,a->date,a->amount,a->accountid,a->categoryid,a->toaccountid,a->isTransfer,
					  a->personid,a->transid);
	int rc = sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail);
	if(rc == SQLITE_OK){
		rc = sqlite3_step(pStmt);
	}
	sqlite3_finalize(pStmt);

	return 0;
}

void clsCASHDB::AccountInOutById(int &in, int &out, int id,RECORDATE_ptr date, RECORDATE_ptr datend){
	in = AccountInById(id,date,datend);
	out = AccountOutById(id,date,datend);
	return;
}

int clsCASHDB::AccountInById(int id,RECORDATE_ptr date, RECORDATE_ptr datend){
	RECORDATE_t d1,d2;
	int amount = 0;
	formatDate(date,datend,d1,d2);

	if(id == -1){
		wsprintf(sqlcmdw,L"select sum(amount) from %s where catgoryid in (select id from %s where type=%d) and \
						  DATE between \"%04d-%02d-%02d 00:00:00\" and \"%04d-%02d-%02d 23:59:59\"",
			TABLE_RECORD,TABLE_CATEGORY,CT_INCOME,
			d1.Year,d1.Month,d1.Day,d2.Year,d2.Month,d2.Day);//收入
	}else{
		wsprintf(sqlcmdw,L"select sum(amount) from %s where ((accountid=%d and catgoryid in (select id from %s where type=%d) ) or \
						  (toaccountid=%d and catgoryid in (select id from %s where type=%d))) and \
						  DATE between \"%04d-%02d-%02d 00:00:00\" and \"%04d-%02d-%02d 23:59:59\"",
			TABLE_RECORD,id,TABLE_CATEGORY,CT_INCOME,
			id,TABLE_CATEGORY,CT_TRANSFER,
			d1.Year,d1.Month,d1.Day,d2.Year,d2.Month,d2.Day);//收入
	}

	if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
		while (sqlite3_step(pStmt) == SQLITE_ROW) {
			amount = sqlite3_column_int(pStmt, 0);
		}
	}

	return amount;
}

int clsCASHDB::AccountOutById(int id,RECORDATE_ptr date, RECORDATE_ptr datend){
	RECORDATE_t d1,d2;
	int amount = 0;
	formatDate(date,datend,d1,d2);

	if(id == -1){
		wsprintf(sqlcmdw,L"select sum(amount) from %s where catgoryid in (select id from %s where type=%d) and \
						  DATE between \"%04d-%02d-%02d 00:00:00\" and \"%04d-%02d-%02d 23:59:59\"",
			TABLE_RECORD,TABLE_CATEGORY,CT_OUTGOING,
			d1.Year,d1.Month,d1.Day,d2.Year,d2.Month,d2.Day);//支出
	}else{
		wsprintf(sqlcmdw,L"select sum(amount) from %s where accountid=%d and \
						  catgoryid in (select id from %s where type!=%d) and \
						  DATE between \"%04d-%02d-%02d 00:00:00\" and \"%04d-%02d-%02d 23:59:59\"",
			TABLE_RECORD,id,TABLE_CATEGORY,CT_INCOME,
			d1.Year,d1.Month,d1.Day,d2.Year,d2.Month,d2.Day);//支出
	}

	if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
		while (sqlite3_step(pStmt) == SQLITE_ROW) {
			amount = sqlite3_column_int(pStmt, 0);
		}
	}

	return amount;
}
int clsCASHDB::AccountBalanceById(int id,RECORDATE_ptr date, bool binit, RECORDATE_ptr datend){
	int in = 0;
	int out = 0;
	CASH_ACCOUNT_ptr a = accountById(id);
	int balance = binit ? a->initval : 0;
	AccountInOutById(in,out,id,date,datend);
	return (balance + in - out);
}


bool clsCASHDB::decrytpDatabase(const char* pwd,int len){
	return (sqlite3_key(db,pwd,len) == SQLITE_OK);
}

bool clsCASHDB::setDatabasePassword(const char* pwd,int len){
	return (sqlite3_rekey(db,pwd,len) == SQLITE_OK);
}

void clsCASHDB::clearRecordSearchResults(){
	if(!list_search_record.empty()){
		list<CASH_RECORD_ptr>::iterator i = list_search_record.begin();
		for(;i != list_search_record.end();i++){
			delete *i;
		}
		list_search_record.clear();
	}
}

void clsCASHDB::clearAccountSearchResults(){
	if(!list_search_account.empty()){
		list<CASH_ACCOUNT_ptr>::iterator i = list_search_account.begin();
		for(;i != list_search_account.end();i++){
			delete *i;
		}
		list_search_account.clear();
	}
}

void clsCASHDB::clearCategorySearchResults(){
	if(!list_search_category.empty()){
		list<CASH_CATEGORY_ptr>::iterator i = list_search_category.begin();
		for(;i != list_search_category.end();i++){
			delete *i;
		}
		list_search_category.clear();
	}
}
bool clsCASHDB::searchRecords(bool blist){
    bool rc = true;
	int resCount = 0;

	//do some clear work
	clearRecordSearchResults();

    if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
        while (sqlite3_step(pStmt) == SQLITE_ROW) {
			if(blist){
				CASH_RECORD_ptr r = new CASH_RECORD_t;
				r->transid = sqlite3_column_int(pStmt, 0);

				C::newstrcpy(&r->date,(LPWSTR) sqlite3_column_text16(pStmt, 1));

				r->amount = sqlite3_column_int(pStmt, 2);

				C::newstrcpy(&r->note,(LPWSTR) sqlite3_column_text16(pStmt, 3));

				r->accountid = sqlite3_column_int(pStmt, 4);

				r->categoryid = sqlite3_column_int(pStmt, 5);

				r->toaccountid = sqlite3_column_int(pStmt, 6);

				r->isTransfer = sqlite3_column_int(pStmt, 7);

				r->personid = sqlite3_column_int(pStmt, 8);
				list_search_record.push_back(r);
			}
			resCount++;
        }
	}else{
		rc = false;
	}
    sqlite3_finalize(pStmt);

	if(resCount == 0) rc = false;
	return rc;
}

bool clsCASHDB::searchAccounts(){
    bool rc = true;

	//do some clear work
	clearAccountSearchResults();

    if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
        while (sqlite3_step(pStmt) == SQLITE_ROW) {
            CASH_ACCOUNT_ptr r = new CASH_ACCOUNT_t;
			r->id = sqlite3_column_int(pStmt, 0);
			C::newstrcpy(&r->name,(LPWSTR) sqlite3_column_text16(pStmt, 1));
			r->initval = sqlite3_column_int(pStmt, 2);
			C::newstrcpy(&r->note,(LPWSTR) sqlite3_column_text16(pStmt, 3));
			r->isfix = sqlite3_column_int(pStmt, 4);

			list_search_account.push_back(r);
        }
	}else{
		rc = false;
	}
    sqlite3_finalize(pStmt);

	if(list_search_account.empty()) rc = false;
	return rc;
}

bool clsCASHDB::searchCategories(){
    bool rc = true;

	//do some clear work
	clearCategorySearchResults();

    if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
        while (sqlite3_step(pStmt) == SQLITE_ROW) {
            CASH_CATEGORY_ptr r = new CASH_CATEGORY_t;
			r->id = sqlite3_column_int(pStmt, 0);
			C::newstrcpy(&r->name,(LPWSTR) sqlite3_column_text16(pStmt, 1));
			r->type = (CASH_TRANSACT_TYPE_t)sqlite3_column_int(pStmt, 2);
			r->parentid = sqlite3_column_int(pStmt, 3);
			r->level = sqlite3_column_int(pStmt, 4);

			list_search_category.push_back(r);
        }
	}else{
		rc = false;
	}
    sqlite3_finalize(pStmt);

	if(list_search_category.empty()) rc = false;
	return rc;
}


//////////////////////////////////////////////
void clsCASHDB::formatDate(RECORDATE_ptr date, RECORDATE_ptr datend,
				RECORDATE_t &d1, RECORDATE_t &d2){
	if(date){
		d1.Year = date->Year > 0 ? date->Year : 0;
		d1.Month = date->Month > 0 ? date->Month : 0;
		d1.Day = date->Day > 0 ? date->Day : 0;
	}else{
		d1.Year = 0; d1.Month = 0; d1.Day = 0;
	}

	if(datend){
		d2.Year = datend->Year > 0 ? datend->Year : 0xfff;
		d2.Month = datend->Month > 0 ? datend->Month : 12;
		d2.Day = datend->Day > 0 ? datend->Day : 31;
	}else{
		d2.Year = 0xfff; d2.Month = 12; d2.Day = 31;
	}
}

bool clsCASHDB::getRecordsByDate(RECORDATE_ptr date, RECORDATE_ptr datend){
	RECORDATE_t d1, d2;
	formatDate(date,datend,d1,d2);

	wsprintf(sqlcmdw,
		L"select * from %s where DATE between \"%04d-%02d-%02d 00:00:00\" and \"%04d-%02d-%02d 23:59:59\" \
		 order by DATE DESC",
			TABLE_RECORD,d1.Year,d1.Month,d1.Day,d2.Year,d2.Month,d2.Day);
	return searchRecords();
}

bool clsCASHDB::getRecordsByDate_v2(
									RECORDATE_ptr date, 
									RECORDATE_ptr datend, 
									UINT orderby){
	RECORDATE_t d1, d2;
	formatDate(date,datend,d1,d2);
	if(orderby == 0){
		wsprintf(sqlcmdw,
			L"select * from %s \
			 where DATE between \
			 \"%04d-%02d-%02d 00:00:00\" and \
			 \"%04d-%02d-%02d 23:59:59\"",
				TABLE_RECORD,d1.Year,d1.Month,d1.Day,d2.Year,d2.Month,d2.Day);
	}else{
		CMzString s = L"order by ";
		bool isE = false;
		if(orderby & 0x2){
			s = s + L"ACCOUNTID";
			isE = true;
		}
		if(orderby & 0x4){
			if(isE) s = s + L",";
			s = s + L"CATGORYID";
			isE = true;
		}
		if(orderby & 0x1){
			if(isE) s = s + L",";
			s = s + L"DATE DESC";
		}
		wsprintf(sqlcmdw,
			L"select * from %s \
			 where DATE between \
			 \"%04d-%02d-%02d 00:00:00\" and \
			 \"%04d-%02d-%02d 23:59:59\" %s",
				TABLE_RECORD,d1.Year,d1.Month,d1.Day,d2.Year,d2.Month,d2.Day,
				s.C_Str());
	}
	return searchRecords();
}

bool clsCASHDB::getRecordsByCategory(int id, RECORDATE_ptr date, RECORDATE_ptr datend){
	RECORDATE_t d1, d2;
	formatDate(date,datend,d1,d2);

	wsprintf(sqlcmdw,
			L"select * from %s where CATGORYID=%d and \
			 DATE between \"%04d-%02d-%02d 00:00:00\" and \"%04d-%02d-%02d 23:59:59\" \
			 order by DATE DESC",
			TABLE_RECORD,id, d1.Year,d1.Month,d1.Day,d2.Year,d2.Month,d2.Day);
	return searchRecords();
}
bool clsCASHDB::getRecordsByAccount(int id, RECORDATE_ptr date, RECORDATE_ptr datend){
	RECORDATE_t d1, d2;
	formatDate(date,datend,d1,d2);

	wsprintf(sqlcmdw,
			L"select * from %s where ACCOUNTID=%d and \
			 DATE between \"%04d-%02d-%02d 00:00:00\" and \"%04d-%02d-%02d 23:59:59\" \
			 order by DATE DESC",
			TABLE_RECORD,id, d1.Year,d1.Month,d1.Day,d2.Year,d2.Month,d2.Day);
	return searchRecords();
}

bool clsCASHDB::getRecordsByToAccount(int id, RECORDATE_ptr date, RECORDATE_ptr datend){
	RECORDATE_t d1, d2;
	formatDate(date,datend,d1,d2);

	wsprintf(sqlcmdw,
			L"select * from %s where TOACCOUNTID=%d and \
			 DATE between \"%04d-%02d-%02d 00:00:00\" and \"%04d-%02d-%02d 23:59:59\" \
			 order by DATE DESC",
			TABLE_RECORD,id, d1.Year,d1.Month,d1.Day,d2.Year,d2.Month,d2.Day);
	return searchRecords();
}

bool clsCASHDB::isChildCategory(int childId, int parentId){
	CASH_CATEGORY_ptr child = categoryById(childId);
	CASH_CATEGORY_ptr parent = categoryById(parentId);

	if(child->level <= parent->level || child->parentid == -1) return false;

	if(child->level == 2 && parent->level == 0){
		CASH_CATEGORY_ptr parent1 = categoryById(child->parentid);
		if(parent1->parentid == parent->id){
			return true;
		}else{
			return false;
		}
	}

	if(child->parentid == parent->id){
		return true;
	}
	return false;
}

//按照name判断
int clsCASHDB::checkDupAccount(CASH_ACCOUNT_ptr pa, bool *bconflict){
	int retid = -1;
	bool bconf = false;

	if(pa->name){
		wsprintf(sqlcmdw,L"select * from %s where NAME=\"%s\" COLLATE NOCASE",TABLE_ACCOUNT,pa->name);
		searchAccounts();
		if(!list_search_account.empty()){	//有重复
			CASH_ACCOUNT_ptr r = list_search_account.front();
			retid = r->id;	//返回存在的记录id号

			if(r->initval != pa->initval || 
				r->isfix != pa->isfix ||
				lstrcmp(r->note,pa->note) != 0){	//有冲突
					bconf = true;
			}
		}
	}
	if(bconflict){
		*bconflict = bconf;
	}
	return retid;
}

//按照name判断
int clsCASHDB::checkDupCategory(CASH_CATEGORY_ptr pc, bool *bconflict){
	int retid = -1;
	bool bconf = false;

	if(pc->name){
		wsprintf(sqlcmdw,L"select * from %s where NAME=\"%s\" and \
						  LEVEL=%d and PARENTID=%d",TABLE_CATEGORY,
			pc->name,
			pc->level,
			pc->parentid);
		searchCategories();
		if(!list_search_category.empty()){	//有重复
			CASH_CATEGORY_ptr r = list_search_category.front();
			retid = r->id;	//返回存在的记录id号

			if(r->type != pc->type){	//有冲突
					bconf = true;
			}
		}
	}
	if(bconflict){
		*bconflict = bconf;
	}
	return retid;
}

//按照date, amount, account id, category id判断
int clsCASHDB::checkDupRecord(CASH_RECORD_ptr pr, bool *bconflict){
	int retid = -1;
	bool bconf = false;

	if(pr->date){
		CMzString datestr = pr->date;

		wsprintf(sqlcmdw,
			L"select * from %s where DATE like \"%s%%\" and \
			 AMOUNT=%d and CATGORYID=%d and ACCOUNTID=%d and TOACCOUNTID=%d and \
			 ISTRANSFER=%d and NOTE=\"%s\"",
			TABLE_RECORD,
			datestr.SubStr(0,10).C_Str(),
			pr->amount,
			pr->categoryid,
			pr->accountid,
			pr->toaccountid,
			pr->isTransfer,
			pr->note);
		searchRecords();

		if(!list_search_record.empty()){	//有重复
			CASH_RECORD_ptr r = list_search_record.front();
			retid = r->transid;	//返回存在的记录id号
#if 0
			if(lstrcmp(r->note,pr->note) != 0){	//有冲突
					bconf = true;
			}
#endif
		}
	}
	if(bconflict){
		*bconflict = bconf;
	}
	return retid;
}

bool clsCASHDB::getCategoryByLevel(int level){
    bool rc = true;

    wsprintf(sqlcmdw, L"select * from %s where LEVEL=%d order by NAME collate pinyin", TABLE_CATEGORY,level);
	rc = searchCategories();
	return rc;
}


bool clsCASHDB::checkDatabaseVersion_v1(){
	bool rc = false;
	wsprintf(sqlcmdw,L"select count(*) from sqlite_master where name=\"%s\"",V1_TABLE_RECORD);
    if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
        while (sqlite3_step(pStmt) == SQLITE_ROW) {
            int count = sqlite3_column_int(pStmt, 0);
			if(count == 1){
				rc = true;
			}
        }
	}else{
		rc = false;
	}
    sqlite3_finalize(pStmt);
	return rc;
}

void clsCASHDB::updateDatabaseVersion_v1(){
	beginTrans();
	wsprintf(sqlcmdw,L"ALTER TABLE %s RENAME TO %s",V1_TABLE_RECORD,V2_TABLE_RECORD);
	int rc = sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail);
	if(rc == SQLITE_OK){
		rc = sqlite3_step(pStmt);
	}else{
		return;
	}
    sqlite3_finalize(pStmt);
	commitTrans();

	if(loadRecords()){
		list<CASH_RECORD_ptr>::iterator i = list_search_record.begin();
		beginTrans();
		for(; i != list_search_record.end(); i++){
			CASH_RECORD_ptr r = *i;
			wchar_t newdate[32];
			wsprintf(newdate,L"%0s 00:00:00",r->date);
			C::newstrcpy(&r->date,newdate);
			//更新日期格式
			wsprintf(sqlcmdw,L"UPDATE %s SET DATE=\"%s\" WHERE TRANSACTID=%d",TABLE_RECORD,r->date,r->transid);
			rc = sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail);
			if(rc == SQLITE_OK){
				rc = sqlite3_step(pStmt);
			}
		    sqlite3_finalize(pStmt);
		}
		commitTrans();
		//清理工作
		clean();
	}
}


//获取账户数量
int clsCASHDB::getAccountSize(){
	int retval = 0;
	wsprintf(sqlcmdw,L"select count(ID) from %s",TABLE_ACCOUNT);
    if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
        while (sqlite3_step(pStmt) == SQLITE_ROW) {
            retval = sqlite3_column_int(pStmt, 0);
        }
	}
	return retval;
}

//获取分类数量
int clsCASHDB::getCategorySize(){
	int retval = 0;
	wsprintf(sqlcmdw,L"select count(ID) from %s",TABLE_CATEGORY);
    if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
        while (sqlite3_step(pStmt) == SQLITE_ROW) {
            retval = sqlite3_column_int(pStmt, 0);
        }
	}
	return retval;
}

//获取明细数量
int clsCASHDB::getRecordSize(){
	int retval = 0;
	wsprintf(sqlcmdw,L"select count(TRANSACTID) from %s",TABLE_RECORD);
    if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
        while (sqlite3_step(pStmt) == SQLITE_ROW) {
            retval = sqlite3_column_int(pStmt, 0);
        }
	}
	return retval;
}

//获取账户ID最大值
int clsCASHDB::getMaxAccountID(){
	int retval = 0;
	if(getAccountSize() > 0){
		wsprintf(sqlcmdw,L"select max(ID) from %s",TABLE_ACCOUNT);
		if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
			while (sqlite3_step(pStmt) == SQLITE_ROW) {
				retval = sqlite3_column_int(pStmt, 0);
			}
		}
	}
	return retval;
}

//获取分类ID最大值
int clsCASHDB::getMaxCategoryID(){
	int retval = 0;
	if(getCategorySize() > 0){
		wsprintf(sqlcmdw,L"select max(ID) from %s",TABLE_CATEGORY);
		if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
			while (sqlite3_step(pStmt) == SQLITE_ROW) {
				retval = sqlite3_column_int(pStmt, 0);
			}
		}
	}
	return retval;
}

//获取明细ID最大值
int clsCASHDB::getMaxRecordID(){
	int retval = 0;
	if(getRecordSize() > 0){
		wsprintf(sqlcmdw,L"select max(TRANSACTID) from %s",TABLE_RECORD);
		if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
			while (sqlite3_step(pStmt) == SQLITE_ROW) {
				retval = sqlite3_column_int(pStmt, 0);
			}
		}
	}
	return retval;
}

//获取明细最小日期
uRecordDate_t clsCASHDB::getMinRecordDate(){
	uRecordDate_t retval;
	wchar_t *date = 0;
	retval.Value = 0;
	if(getRecordSize() > 0){
		wsprintf(sqlcmdw,L"select min(DATE) from %s",TABLE_RECORD);
		if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
			while (sqlite3_step(pStmt) == SQLITE_ROW) {
				C::newstrcpy(&date,(LPWSTR) sqlite3_column_text16(pStmt, 0),10);
			}
		}
	}
	if(date != 0){
		DWORD y,m,d;
		swscanf(date,L"%04d-%02d-%02d",&y,&m,&d);
		retval.Date.Year = y;
		retval.Date.Month = m;
		retval.Date.Day = d;
	}
	return retval;
}

//获取明细最大日期
uRecordDate_t clsCASHDB::getMaxRecordDate(){
	uRecordDate_t retval;
	wchar_t *date = 0;
	retval.Value = 0;
	if(getRecordSize() > 0){
		wsprintf(sqlcmdw,L"select max(DATE) from %s",TABLE_RECORD);
		if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
			while (sqlite3_step(pStmt) == SQLITE_ROW) {
				C::newstrcpy(&date,(LPWSTR) sqlite3_column_text16(pStmt, 0),10);
			}
		}
	}
	if(date != 0){
		DWORD y,m,d;
		swscanf(date,L"%04d-%02d-%02d",&y,&m,&d);
		retval.Date.Year = y;
		retval.Date.Month = m;
		retval.Date.Day = d;
	}
	return retval;
}

bool clsCASHDB::TableExists(wchar_t* tablename){
	bool nRet = false;

	wsprintf(sqlcmdw,L"select count(*) from sqlite_master where type='table' and name = '%s'",tablename);
    if (sqlite3_prepare16(db,sqlcmdw,-1,&pStmt,&pzTail) == SQLITE_OK) {
		if(sqlite3_step(pStmt) == SQLITE_ROW){
			nRet = (sqlite3_column_int(pStmt, 0) != 0);
		}
    }
    sqlite3_finalize(pStmt);

	return nRet;
}

bool clsCASHDB::addFieldPerson(){
	//检查是否存在PERSON_ID字段
	bool bfield = false;
	int rc = 0;
	wsprintf(sqlcmdw,L"select PERSON_ID from %s",TABLE_RECORD);
    if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
		rc = sqlite3_step(pStmt);
        if(rc == SQLITE_ROW || rc == SQLITE_DONE) {
            bfield = true;
        }
	}
	sqlite3_finalize(pStmt);
	//字段不存在，增加字段
	bool retval = false;
	if(!bfield){
		wsprintf(sqlcmdw,
			L"ALTER TABLE '%s' ADD COLUMN PERSON_ID NUMERIC NOT NULL  DEFAULT 0",
			TABLE_RECORD);
		if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
			if(sqlite3_step(pStmt) == SQLITE_DONE) {
				retval = true;
			}
		}
		sqlite3_finalize(pStmt);
	}
	return retval;
}

const int PersonTypeStrID[] = {
	IDS_STR_PERSON_FAMILY,
	IDS_STR_PERSON_CONTACT,
	IDS_STR_PERSON_INSTITUTION
};

int clsCASHDB::getPersonTypeNameStrID(CASH_PERSON_TYPE_t tid){
	int tsize = sizeof(PersonTypeStrID)/sizeof(PersonTypeStrID[0]);
	if(tid >= tsize) tid = PRSN_FAMILY;
	return PersonTypeStrID[tid];
}

const int* clsCASHDB::getPersonTypeNameStrSet(int *nsize){
	if(nsize) *nsize = sizeof(PersonTypeStrID)/sizeof(PersonTypeStrID[0]);
	return PersonTypeStrID;
}

void clsCASHDB::clearPersonSearchResults(){
	if(!list_search_person.empty()){
		list<CASH_PERSON_ptr>::iterator i = list_search_person.begin();
		for(;i != list_search_person.end();i++){
			delete *i;
		}
		list_search_person.clear();
	}
}

bool clsCASHDB::searchPerson(){
    bool rc = true;
	int resCount = 0;

	//do some clear work
	clearPersonSearchResults();

	if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK) {
		while (sqlite3_step(pStmt) == SQLITE_ROW) {
			CASH_PERSON_ptr r = new CASH_PERSON_t;
			r->id = sqlite3_column_int(pStmt, 0);

			C::newstrcpy(&r->name,(LPWSTR) sqlite3_column_text16(pStmt, 1));

			r->type = (CASH_PERSON_TYPE_t)sqlite3_column_int(pStmt, 2);

			list_search_person.push_back(r);
		}
		resCount++;
	}else{
		rc = false;
	}
	sqlite3_finalize(pStmt);

	if(resCount == 0) rc = false;
	return rc;
}

bool clsCASHDB::getPersonByType(CASH_PERSON_TYPE_t t){
	if( t == PRSN_ALL ){
		wsprintf(sqlcmdw,L"select * from '%s' order by type",TABLE_PERSON);
	}else{
		wsprintf(sqlcmdw,L"select * from '%s' where TYPE=%d",TABLE_PERSON,t);
	}
	return searchPerson();
}

int	clsCASHDB::checkDupPerson(CASH_PERSON_ptr pP){
	int ret = -1;
	if(pP->name){
		wsprintf(sqlcmdw,L"select * from '%s' where NAME='%s' COLLATE NOCASE",TABLE_PERSON,pP->name);
		searchPerson();
		if(!list_search_person.empty()){
			CASH_PERSON_ptr p = list_search_person.front();
			ret = p->id;
		}
	}
	return ret;
}

CASH_PERSON_ptr clsCASHDB::personById(int id){
	wsprintf(sqlcmdw,L"select * from '%s' where ID=%d",TABLE_PERSON,id);
	searchPerson();
	if(!list_search_person.empty()){
		return list_search_person.front();
	}
	return NULL;
}

wchar_t* clsCASHDB::getPersonNameById(int id){
	CASH_PERSON_ptr p = personById(id);
	if(p){
		return p->name;
	}
	return NULL;
}

bool clsCASHDB::appendPerson(CASH_PERSON_ptr p){
	bool bret = false;
	if(p){
		if(p->name){
			p->id = getMaxPersonID() + 1;
			wsprintf(sqlcmdw,L"insert into '%s' "PERSON_TBL_INSERT,
				TABLE_PERSON,p->id,p->name,p->type);
			if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK){
				if(sqlite3_step(pStmt) == SQLITE_DONE){
					bret = true;
				}
			}
			sqlite3_finalize(pStmt);
		}
	}
	return bret;
}

bool clsCASHDB::updatePerson(CASH_PERSON_ptr p){
	bool bret = false;
	if(p){
		if(p->name){
			wsprintf(sqlcmdw,L"update '%s' set name='%s', type=%d where id=%d",
				TABLE_PERSON,p->name,p->type,p->id);
			if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK){
				if(sqlite3_step(pStmt) == SQLITE_DONE){
					bret = true;
				}
			}
			sqlite3_finalize(pStmt);
		}
	}
	return bret;
}

bool clsCASHDB::deletePersonById(int id){
	bool bret = false;
	//check if is being used
	wsprintf(sqlcmdw,L"select * from %s where PERSON_ID=%d",TABLE_RECORD,id);
	if(searchRecords()){
		return bret;
	}

	wsprintf(sqlcmdw,L"delete from '%s' where id=%d",
		TABLE_PERSON,id);
	if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK){
		if(sqlite3_step(pStmt) == SQLITE_DONE){
			bret = true;
		}
	}
	sqlite3_finalize(pStmt);
	return bret;
}

int clsCASHDB::getMaxPersonID(){
	int nret = 0;
	wsprintf(sqlcmdw,L"select max(id) from '%s'",TABLE_PERSON);
	if (sqlite3_prepare16(db, sqlcmdw, -1, &pStmt, &pzTail) == SQLITE_OK){
		if(sqlite3_step(pStmt) == SQLITE_ROW){
			nret = sqlite3_column_int(pStmt, 0);
		}
	}
	sqlite3_finalize(pStmt);
	return nret;
}