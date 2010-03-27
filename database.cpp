#include <cMzCommon.h>
using namespace cMzCommon;

#include "database.h"
#include "resource.h"

#ifdef _DEBUG
#define DEFAULT_DB		L"Program Files\\M8Cash\\cash.db"
#else
#define DEFAULT_DB		L"\\Disk\\Programs\\M8Cash\\cash.db"
#endif

//老版本定义
#define OLD_TABLE_ACCOUNT	L"ACCOUNTS_v1"
#define OLD_TABEL_CATEGORY	L"CATEGORIES_v1"
#define V1_TABLE_TRANSACTION	L"RECORDS_v1"
#define V2_TABLE_TRANSACTION	L"RECORDS_v2"
//新版本定义
#define TABLE_ACCOUNT	L"ACCOUNTS_v1"
#define TABLE_CATEGORY	L"CATEGORIES_v1"
#define TABLE_TRANSACTION	V2_TABLE_TRANSACTION
#define TABLE_PERSON	L"PERSONS_v1"


#ifdef _DEBUG
#define db_out(s) printf("%s:%d: %s\n",__FUNCTION__, __LINE__, s)
#else
#define db_out(s) printf("%s:%d: %s\n",__FUNCTION__, __LINE__, s)
#endif

#define TRY try
#define CATCH catch

extern HINSTANCE LangresHandle;

clsCASHDB::clsCASHDB() {
    wchar_t currpath[MAX_PATH];
    bool ret = true;
    if(File::GetCurrentPath(currpath)){
        wsprintf(db_path,L"%s\\cash.db",currpath);
    }else{
        wsprintf(db_path,DEFAULT_DB);
    }
	//connect();	//构造时立即打开
}

clsCASHDB::~clsCASHDB() {
    disconnect();
	clean();
}

//打开数据库
bool clsCASHDB::connect(){
	bool bRet = true;
	TRY{
		bool bfileNotExists = false;
		if(!File::FileExists(db_path)){
			bfileNotExists = true;
            bRet = false;
		}
		sqlconn.open(db_path);
		sqlconn.createPinyinSearchEngine();
		//文件新建时创建默认表
		if(bfileNotExists) createDefaultDatabase();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	return bRet;
}

//关闭数据库
bool clsCASHDB::disconnect(){
	bool bRet = true;
	TRY{
		sqlconn.close();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	return bRet;
}

//测试密码
bool clsCASHDB::checkpwd(wchar_t* pwd,int len){
	bool bRet = false;

	if(pwd && len != 0){
		decrypt(pwd,len);
	}
	TRY{
		bRet = sqlconn.executeint(L"select count(*) from sqlite_master;") > 0;
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

    if(!bRet){  //如果解密失败，则需要断开数据库后再连接
        disconnect();
        connect();
    }
    return bRet;
}

//解密数据库
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

	bool bRet = true;
    TRY{
	    sqlconn.decypt(temp,bytecnt);
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	delete temp;
    return bRet;
}

//加密数据库
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

	bool bRet = true;
    TRY{
	    sqlconn.encypt(temp,bytecnt);
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	delete temp;
	return bRet;
}

//清除分类列表和帐号列表
void clsCASHDB::clean(){
	cleanAccountList();
	cleanCategoryList();
}

//载入帐号和分类
bool clsCASHDB::load(){
	return true;
	if(!loadAccounts() ||
		!loadCategories()){ 
		return false;
	}
	return true;
}

//恢复数据库
void clsCASHDB::recover(){
	//createDefaultDatabase();
	restoreDefaultCategory();
}



/******************************************************
/* 具体操作过程
*******************************************************/

void clsCASHDB::createDefaultDatabase() {
	createDefaultAccounts();
	createDefaultCategories();
	createDefaultPersons();
	createDefaultTransaction();
}

/////////////////////////账户相关/////////////////////////////
//创建：默认账户
bool clsCASHDB::createDefaultAccounts(){
	bool bRet = true;
	TRY{	//create table
		sqlite3_command cmd(sqlconn,
			L"create table if not exists '"
			TABLE_ACCOUNT
			L"' (ID numeric primary key,NAME text NOT NULL,INITIALVALUE integer,NOTE text,ISFIX integer);"
			);
		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

	if(!bRet) return bRet;	//创建table失败

	CONST_CASH_ACCOUNT_t default_accounts[] = {
        {0, IDS_STR_CASH, 0, IDS_STR_CASH, 1},
        {1, IDS_STR_CREDIT, 0, IDS_STR_CREDIT, 1},
        {2, IDS_STR_SAVING, 0, IDS_STR_SAVING, 1},
    };
    int aSets = sizeof (default_accounts) / sizeof (CONST_CASH_ACCOUNT_t);
	TRY{
		sqlite3_command cmd(sqlconn,
			L"insert into '"
			TABLE_ACCOUNT
			L"' (ID,NAME,INITIALVALUE,NOTE,ISFIX) "
			L"values(?,?,?,?,?);"
			);
		for(int i = 0; i < aSets; i++){
			cmd.bind(1,static_cast<int>(default_accounts[i].id));
			cmd.bind(2,
				LOADSTRING(default_accounts[i].nameID).C_Str(),
				LOADSTRING(default_accounts[i].nameID).Length()*2);
			cmd.bind(3,default_accounts[i].initval);
			cmd.bind(4,
				LOADSTRING(default_accounts[i].noteID).C_Str(),
				LOADSTRING(default_accounts[i].noteID).Length()*2);
			cmd.bind(5,static_cast<int>(default_accounts[i].isfix));
			cmd.executenonquery();
		}
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	return bRet;
}

//新增：账户
bool clsCASHDB::appendAccount(CASH_ACCOUNT_ptr acc) {
    bool bRet = true;
	TRY{
		acc->id = getMaxAccountID() + 1;
		sqlite3_command cmd(this->sqlconn,
		L"insert into '"
		TABLE_ACCOUNT
		L"' (ID,NAME,INITIALVALUE,NOTE,ISFIX)"
		L" values(?,?,?,?,?);");
		cmd.bind(1,static_cast<int>(acc->id));
		cmd.bind(2,acc->name, lstrlen(acc->name)*2);
		cmd.bind(3,static_cast<int>(acc->initval));
		cmd.bind(4,acc->note,lstrlen(acc->note)*2);
		cmd.bind(5,static_cast<int>(acc->isfix));

		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	
	if(bRet){
		//重新载入
		this->loadAccounts();
	}
    return bRet;
}

//删除：账户：对应ID
int clsCASHDB::deleteAccountById(int id){
	const CASH_ACCOUNT_ptr beremoved = accountById(id);

	if(beremoved == NULL) return -3; //无此账户

	//检查是否正在被使用
	bool bUsed = true;
	TRY{
		sqlite3_command cmd(sqlconn,
			L"select count(*) from '"
			TABLE_TRANSACTION
			L"' where ACCOUNTID=? or TOACCOUNTID=?");

		cmd.bind(1,id); cmd.bind(2,id);
		if(cmd.executeint() == 0){
			bUsed = false;
		}
	}CATCH(exception &ex){
		db_out(ex.what());
	}
	if(bUsed) return -1;	//正在被使用，无法删除

	//从数据库中删除分类
	TRY{
		sqlite3_command cmd(sqlconn,
			L"delete from '"
			TABLE_ACCOUNT
			L"' where ID=?");
		cmd.bind(1,id);
		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
	}

	return 0;
}

//更新账户
bool clsCASHDB::updateAccount(CASH_ACCOUNT_ptr a){
	if(a == NULL) return false;
	bool bRet = true;

	TRY{
		sqlite3_command cmd(sqlconn,
			L"update '"
			TABLE_ACCOUNT
			L"' set NAME=?, NOTE=?, INITIALVALUE=? where ID=?");
		cmd.bind(1,a->name,lstrlen(a->name)*2);
		cmd.bind(2,a->note,lstrlen(a->note)*2);
		cmd.bind(3,a->initval);
		cmd.bind(4,static_cast<int>(a->id));
		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

	if(bRet){
		updateAccountList(a->id, a);
	}

	return bRet;
}

//更新账户列表
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

//载入：账户
bool clsCASHDB::loadAccounts() {
	//载入前清除垃圾,防止重复载入
	cleanAccountList();

	bool bRet = true;
	TRY{
		sqlite3_command cmd(this->sqlconn,
			L"select * from '"
			TABLE_ACCOUNT
			L"';");
		
		sqlite3_reader reader = cmd.executereader();
		while(reader.read()){
            CASH_ACCOUNT_ptr accs = new CASH_ACCOUNT_t;

            accs->id = reader.getint(0);
			C::newstrcpy(&accs->name,reader.getstring16(1).c_str());
			accs->initval = reader.getint(2);
			C::newstrcpy(&accs->note,reader.getstring16(3).c_str());
            accs->isfix = reader.getint(4);

            list_account.push_back(accs);

        }
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

    return bRet;
}

//清除：帐号列表
void clsCASHDB::cleanAccountList(){
	if(list_account.size()){
		list<CASH_ACCOUNT_ptr>::iterator c = list_account.begin();
		for(;c != list_account.end(); c++){
			delete *c;
		}
		list_account.clear();
	}
}

//根据id获得帐号
CASH_ACCOUNT_ptr clsCASHDB::accountById(int id) {
	TRY{
		sqlite3_command cmd(sqlconn,
			L"select * from '"
			TABLE_ACCOUNT
			L"' where ID=?;");
		cmd.bind(1,id);
		if(searchAccounts(cmd)){
			return list_search_account.front();
		}
	}CATCH(exception &ex){
		db_out(ex.what());
	}
	return NULL;
}

//根据ID获取账户名称
wchar_t* clsCASHDB::getAccountNameById(int id) {
	CASH_ACCOUNT_ptr paccount = accountById(id);
	return (paccount == NULL ? NULL : paccount->name);
}

//清除搜索结果：账户
void clsCASHDB::clearAccountSearchResults(){
	if(!list_search_account.empty()){
		list<CASH_ACCOUNT_ptr>::iterator i = list_search_account.begin();
		for(;i != list_search_account.end();i++){
			delete *i;
		}
		list_search_account.clear();
	}
}

//搜索：账户
bool clsCASHDB::searchAccounts(sqlite3_command& cmd){
	//do some clear work
	clearAccountSearchResults();

    bool bRet = true;
	TRY{
		sqlite3_reader reader = cmd.executereader();
		while(reader.read()){
			CASH_ACCOUNT_ptr r = new CASH_ACCOUNT_t;
			r->id = reader.getint(0);
			C::newstrcpy(&r->name,reader.getstring16(1).c_str());
			r->initval = reader.getint(2);
			C::newstrcpy(&r->note,reader.getstring16(3).c_str());
			r->isfix = reader.getint(4);

			list_search_account.push_back(r);
		}
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

	if(list_search_account.empty()) bRet = false;
	return bRet;
}

//检查是否重复账户：按照name判断
int clsCASHDB::checkDupAccount(CASH_ACCOUNT_ptr pa, bool *bconflict){
	if(pa == NULL || pa->name == NULL) return -1;

	int retid = -1;
	bool bconf = false;

	TRY{
		sqlite3_command cmd(sqlconn,
			L"select * from '"
			TABLE_ACCOUNT
			L"' where NAME=? COLLATE NOCASE;");
		cmd.bind(1,pa->name,lstrlen(pa->name)*2);
		if(searchAccounts(cmd)){
			CASH_ACCOUNT_ptr r = list_search_account.front();
			retid = r->id;	//返回存在的记录id号

			if(r->initval != pa->initval || 
				r->isfix != pa->isfix ||
				lstrcmp(r->note,pa->note) != 0){	//有冲突
					bconf = true;
			}
			if(bconflict){
				*bconflict = bconf;
			}
		}
	}CATCH(exception &ex){
		db_out(ex.what());
	}

	return retid;
}

//获取账户数量
int clsCASHDB::getAccountSize(){
	int retval = 0;
	TRY{
		sqlite3_command cmd(sqlconn,
			L"select count(ID) from '"
			TABLE_ACCOUNT
			L"';");
		retval = cmd.executeint();
	}CATCH(exception &ex){
		db_out(ex.what());
	}
	return retval;
}

//获取账户ID最大值
int clsCASHDB::getMaxAccountID(){
	int retval = 0;
	TRY{
		sqlite3_command cmd(sqlconn,
			L"select max(ID) from '"
			TABLE_ACCOUNT
			L"';");
		retval = cmd.executeint();
	}CATCH(exception &ex){
		db_out(ex.what());
	}
	return retval;
}

//////////////////分类相关///////////////////////////
//创建：默认分类
bool clsCASHDB::createDefaultCategories(){
	bool bRet = true;
	TRY{	//create table
		sqlite3_command cmd(sqlconn,
			L"create table if not exists '"
			TABLE_CATEGORY
			L"' (ID numeric primary key,NAME text NOT NULL,TYPE integer NOT NULL,PARENTID numeric NOT NULL,LEVEL integer);"
			);
		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

	if(!bRet) return bRet;	//创建table失败

    CONST_CASH_CATEGORY_t default_categories[] = {
        {0, IDS_STR_EXPENSE, CT_OUTGOING, -1, 0},
        {1, IDS_STR_INCOME, CT_INCOME, -1, 0},
        {2, IDS_STR_TRANSFER, CT_TRANSFER, -1, 2},	//设置为无子目录
    };
    int aSets = sizeof (default_categories) / sizeof (CONST_CASH_CATEGORY_t);
	TRY{
		sqlite3_command cmd(sqlconn,
			L"insert into '"
			TABLE_CATEGORY
			L"' (ID,NAME,TYPE,PARENTID,LEVEL) "
			L"values(?,?,?,?,?);"
			);
		for(int i = 0; i < aSets; i++){
			cmd.bind(1,static_cast<int>(default_categories[i].id));
			cmd.bind(2,
				LOADSTRING(default_categories[i].nameID).C_Str(),
				LOADSTRING(default_categories[i].nameID).Length()*2);
			cmd.bind(3,static_cast<int>(default_categories[i].type));
			cmd.bind(4,static_cast<int>(default_categories[i].parentid));
			cmd.bind(5,static_cast<int>(default_categories[i].level));
			cmd.executenonquery();
		}
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	return bRet;
}

//恢复：默认分类
bool clsCASHDB::restoreDefaultCategory() {
    CONST_CASH_CATEGORY_t default_categories[] = {
        {0, IDS_STR_EXPENSE, CT_OUTGOING, -1, 0},
        {1, IDS_STR_INCOME, CT_INCOME, -1, 0},
        {2, IDS_STR_TRANSFER, CT_TRANSFER, -1, 2},	//设置为无子目录
    };

	bool bRet = true;
	TRY{
		sqlite3_command cmd(sqlconn,
			L"update '"
			TABLE_CATEGORY
			L"' set NAME=? where LEVEL=? and TYPE=? and PARENTID=-1;"
			);
		for(int i = 0; i < sizeof(default_categories)/sizeof(CONST_CASH_CATEGORY_t); i++){
			cmd.bind(1,LOADSTRING(default_categories[i].nameID).C_Str(),
				LOADSTRING(default_categories[i].nameID).Length()*2);
			cmd.bind(2,static_cast<int>(default_categories[i].level));
			cmd.bind(3,static_cast<int>(default_categories[i].type));
			cmd.executenonquery();
		}
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	return bRet;
}
//新增：分类
bool clsCASHDB::appendCategory(CASH_CATEGORY_ptr cat) {
    bool bRet = true;
	TRY{
		cat->id = getMaxCategoryID() + 1;
		sqlite3_command cmd(this->sqlconn,
		L"insert into '"
		TABLE_CATEGORY
		L"' (ID,NAME,TYPE,PARENTID,LEVEL)"
		L" values(?,?,?,?,?);");

		cmd.bind(1,static_cast<int>(cat->id));
		cmd.bind(2,cat->name, lstrlen(cat->name)*2);
		cmd.bind(3,static_cast<int>(cat->type));
		cmd.bind(4,static_cast<int>(cat->parentid));
		cmd.bind(5,static_cast<int>(cat->level));

		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	
	//数据库写入成功后，同时增加到分类列表，减少数据库读操作
	if(bRet){
		//重新载入
		this->loadCategories();
	}
    return bRet;
}

//删除：对应ID：分类
int clsCASHDB::deleteCategoryById(int id){
	CASH_CATEGORY_ptr beremoved = categoryById(id);
	if(beremoved == NULL) return -3; //无此分类

	if(beremoved->level == 0) return -2; //根分类，不能删除

	if(beremoved->level < 2){	//检查是否有子类
	    list<CASH_CATEGORY_ptr>::iterator i = list_category.begin();
		for (; i!=list_category.end(); i++){
			CASH_CATEGORY_ptr chk = *i;
			if(chk->parentid == id){
				return -1;	//有子类，无法删除
			}
		}
	}

	//检查是否正在被使用
	bool bUsed = true;
	TRY{
		sqlite3_command cmd(sqlconn,
			L"select count(*) from '"
			TABLE_TRANSACTION
			L"' where CATGORYID=?");
		cmd.bind(1,id);
		if(cmd.executeint() == 0){
			bUsed = false;
		}
	}CATCH(exception &ex){
		db_out(ex.what());
	}

	if(bUsed) return -1;	//正在被使用，无法删除

	//从数据库中删除分类
	TRY{
		sqlite3_command cmd(sqlconn,
			L"delete from '"
			TABLE_CATEGORY
			L"' where ID=?");
		cmd.bind(1,id);
		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
	}

	return 0;
}

//更新：分类列表
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

//更新：分类
bool clsCASHDB::updateCategory(CASH_CATEGORY_ptr a){
	if(a == NULL) return false;

	bool bRet = true;

	TRY{
		sqlite3_command cmd(sqlconn,
			L"update '"
			TABLE_CATEGORY
			L"' set NAME=?,LEVEL=?,PARENTID=?,TYPE=? where ID=?");
		cmd.bind(1,a->name,lstrlen(a->name)*2);
		cmd.bind(2,static_cast<int>(a->level));
		cmd.bind(3,static_cast<int>(a->parentid));
		cmd.bind(4,static_cast<int>(a->type));
		cmd.bind(5,static_cast<int>(a->id));
		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

	if(bRet){
		updateCategoryList(a->id, a);
	}

	return bRet;
}

//载入：分类
bool clsCASHDB::loadCategories() {
	//载入前清除垃圾,防止重复载入
	cleanCategoryList();

	bool bRet = true;
	TRY{
		sqlite3_command cmd(this->sqlconn,
			L"select * from '"
			TABLE_CATEGORY
			L"' order by NAME,LEVEL collate pinyin;");
		
		sqlite3_reader reader = cmd.executereader();
		while(reader.read()){
            CASH_CATEGORY_ptr c = new CASH_CATEGORY_t;

			c->id = reader.getint(0);
			C::newstrcpy(&c->name,reader.getstring16(1).c_str());
			c->type = static_cast<CASH_TRANSACT_TYPE_t>(reader.getint(2));
            c->parentid = reader.getint(3);
            c->level = reader.getint(4);
            list_category.push_back(c);
		}
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

    return bRet;
}

//清除：分类列表
void clsCASHDB::cleanCategoryList(){
	if(list_category.size()){
		list<CASH_CATEGORY_ptr>::iterator t = list_category.begin();
		for(;t != list_category.end(); t++){
			delete *t;
		}
		list_category.clear();
	}
}

//根据ID获得分类
CASH_CATEGORY_ptr clsCASHDB::categoryById(int id) {
	TRY{
		sqlite3_command cmd(sqlconn,
			L"select * from '"
			TABLE_CATEGORY
			L"' where ID=?;");
		cmd.bind(1,id);
		if(searchCategories(cmd)){
			wprintf(L"%s\n",list_search_category.front()->name);
			return list_search_category.front();
		}
	}CATCH(exception &ex){
		db_out(ex.what());
	}
	return NULL;
}

//根据ID获取分类名称
wchar_t* clsCASHDB::getCategoryNameById(int id) {
	CASH_CATEGORY_ptr pcategory = categoryById(id);
	return (pcategory == NULL ? NULL : pcategory->name);
}

//根据ID获取分类全名（包括父类）
wchar_t* clsCASHDB::getCategoryFullNameById(int id) {
	wchar_t* nodename[3] = {0,0,0}; //child max depth = 3
    int depth = 0;
	int szfullname = 0;

	int nodeidx = id;
    CASH_CATEGORY_ptr c;
    do {
        c = categoryById(nodeidx);
		if(c != NULL){
			C::newstrcpy(&nodename[depth++], c->name);
			nodeidx = c->parentid;
			szfullname += lstrlen(c->name);
		}else{
			nodeidx = -1;	//当不能找到对应category时结束
		}
    } while (nodeidx != -1);
	
	//分配字符串空间
	memset(catfullname,0,128);
    for (int i = depth-1; i >= 0; i--) {
        lstrcat(catfullname, nodename[i]);
		delete nodename[i];
        if (i != 0) {
            lstrcat(catfullname, L" - ");
        }
    }
    return catfullname;
}

//清除搜索结果：分类
void clsCASHDB::clearCategorySearchResults(){
	if(!list_search_category.empty()){
		list<CASH_CATEGORY_ptr>::iterator i = list_search_category.begin();
		for(;i != list_search_category.end();i++){
			delete *i;
		}
		list_search_category.clear();
	}
}

//搜索：分类
bool clsCASHDB::searchCategories(sqlite3_command& cmd){
    bool rc = true;

	//do some clear work
	clearCategorySearchResults();

    bool bRet = true;
	TRY{
		sqlite3_reader reader = cmd.executereader();
		while(reader.read()){
			CASH_CATEGORY_ptr r = new CASH_CATEGORY_t;
			r->id = reader.getint(0);
			C::newstrcpy(&r->name,reader.getstring16(1).c_str());
			r->type = static_cast<CASH_TRANSACT_TYPE_t>(reader.getint(2));
			r->parentid = reader.getint(3);
			r->level = reader.getint(4);

			list_search_category.push_back(r);
		}
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

	if(list_search_category.empty()) bRet = false;
	return bRet;
}

//检查是否为子分类
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

//检查是否重复分类：按照name判断
int clsCASHDB::checkDupCategory(CASH_CATEGORY_ptr pc, bool *bconflict){
	if(pc == NULL || pc->name == NULL) return -1;

	int retid = -1;

	TRY{
		sqlite3_command cmd(sqlconn,
			L"select ID,TYPE from '"
			TABLE_CATEGORY
			L"' where NAME=? and LEVEL=? and PARENTID=?;");
		cmd.bind(1,pc->name,lstrlen(pc->name)*2);
		cmd.bind(2,static_cast<int>(pc->level));
		cmd.bind(3,static_cast<int>(pc->parentid));
		sqlite3_reader reader = cmd.executereader();
		while(reader.read()){
			retid = reader.getint(0);
			if(reader.getint(1) != pc->type){
				if(bconflict) *bconflict = true;
			}
		}
	}CATCH(exception &ex){
		db_out(ex.what());
	}

	return retid;
}

//获取分类：根据级别
bool clsCASHDB::getCategoryByLevel(int level){
    bool bRet = true;

	TRY{
		sqlite3_command cmd(sqlconn,
			L"select * from '"
			TABLE_CATEGORY
			L"' where LEVEL=? order by NAME collate pinyin;");
        cmd.bind(1,level);
		bRet = searchCategories(cmd);
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

	return bRet;
}

//获取分类数量
int clsCASHDB::getCategorySize(){
	int retval = 0;

	TRY{
		sqlite3_command cmd(sqlconn,
			L"select count(ID) from '"
			TABLE_CATEGORY
			L"';");
		retval = cmd.executeint();
	}CATCH(exception &ex){
		db_out(ex.what());
	}

	return retval;
}

//获取分类ID最大值
int clsCASHDB::getMaxCategoryID(){
	int retval = 0;

	TRY{
		sqlite3_command cmd(sqlconn,
			L"select max(ID) from '"
			TABLE_CATEGORY
			L"';");
		retval = cmd.executeint();
	}CATCH(exception &ex){
		db_out(ex.what());
	}

	return retval;
}

/////////交易相关
//创建：默认交易明细
bool clsCASHDB::createDefaultTransaction(){
	bool bRet = true;
	TRY{	//create table
		sqlite3_command cmd(sqlconn,
			L"create table if not exists '"
			TABLE_TRANSACTION
			L"' (TRANSACTID numeric primary key,DATE text not null,AMOUNT integer not null,NOTE text,"
			L" ACCOUNTID numeric not null,CATGORYID numeric not null,TOACCOUNTID numeric,ISTRANSFER integer not null,"
			L" PERSON_ID numeric not null default 0);"
			);
		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

	return bRet;
}
//新增：交易明细
bool clsCASHDB::appendTransaction(CASH_TRANSACT_ptr r) {
    bool bRet = true;
	TRY{
		//获取最大的交易ID
		r->transid = getMaxTransactionID() + 1;

		sqlite3_command cmd(this->sqlconn,
		L"insert into '"
		TABLE_TRANSACTION
		L"' (TRANSACTID,DATE,AMOUNT,NOTE,ACCOUNTID,CATGORYID,TOACCOUNTID,ISTRANSFER,PERSON_ID)"
		L" values(?,?,?,?,?,?,?,?,?);");

		cmd.bind(1,static_cast<int>(r->transid));
		cmd.bind(2,r->date, lstrlen(r->date)*2);
		cmd.bind(3,static_cast<int>(r->amount));
		cmd.bind(4,r->note,lstrlen(r->note)*2);
		cmd.bind(5,static_cast<int>(r->accountid));
		cmd.bind(6,static_cast<int>(r->categoryid));
		cmd.bind(7,static_cast<int>(r->toaccountid));
		cmd.bind(8,static_cast<int>(r->isTransfer));
		cmd.bind(9,static_cast<int>(r->personid));

		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	
    return bRet;
}

//载入：交易明细
bool clsCASHDB::loadTransactions() {
    bool bRet = true;
	TRY{
		sqlite3_command cmd(this->sqlconn,
			L"select * from '"
			TABLE_TRANSACTION
			L"';");

		bRet = searchTransactions(cmd);
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

    return bRet;
}

//根据ID获得交易明细
bool clsCASHDB::recordById(int id,CASH_TRANSACT_ptr pr) {
	bool bRet = true;
	if(pr == NULL) return false;	//指针错误

	TRY{
		sqlite3_command cmd(this->sqlconn,
			L"select * from '"
			TABLE_TRANSACTION
			L"' where TRANSACTID=?;");
		cmd.bind(1,id);
		
		sqlite3_reader reader = cmd.executereader();
		while(reader.read()){
			pr->transid = reader.getint(0);
			C::newstrcpy(&pr->date,reader.getstring16(1).c_str());
			pr->amount = reader.getint(2);
			C::newstrcpy(&pr->note,reader.getstring16(3).c_str());
			pr->accountid = reader.getint(4);
			pr->categoryid = reader.getint(5);
			pr->toaccountid = reader.getint(6);
			pr->isTransfer = reader.getint(7);
			pr->personid = reader.getint(8);
		}
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

    return bRet;
}


//删除：对应ID：交易明细
bool clsCASHDB::deleteTransactionById(int id){
	bool bRet = true;
	//从数据库中删除分类
	TRY{
		sqlite3_command cmd(sqlconn,
			L"delete from '"
			TABLE_TRANSACTION
			L"' where TRANSACTID=?");
		cmd.bind(1,id);
		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	return bRet;
}

//更新：交易明细
bool clsCASHDB::updateTransaction(CASH_TRANSACT_ptr a){
	if(a == NULL) return false;

	bool bRet = true;

	TRY{
		sqlite3_command cmd(sqlconn,
			L"update '"
			TABLE_CATEGORY
			L"' set NOTE=?,DATE=?,AMOUNT=?,ACCOUNTID=?,"
			L"CATGORYID=?,TOACCOUNTID=?,ISTRANSFER=?,PERSON_ID=? "
			L"where TRANSACTID=?");
		cmd.bind(1,a->note,lstrlen(a->note)*2);
		cmd.bind(2,a->date,lstrlen(a->date)*2);
		cmd.bind(3,a->amount);
		cmd.bind(4,static_cast<int>(a->accountid));
		cmd.bind(5,static_cast<int>(a->categoryid));
		cmd.bind(6,static_cast<int>(a->toaccountid));
		cmd.bind(7,static_cast<int>(a->isTransfer));
		cmd.bind(8,static_cast<int>(a->personid));
		cmd.bind(9,static_cast<int>(a->transid));
		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

	return bRet;
}

//清除搜索结果：交易明细
void clsCASHDB::clearTransactionSearchResults(){
	if(!list_search_record.empty()){
		list<CASH_TRANSACT_ptr>::iterator i = list_search_record.begin();
		for(;i != list_search_record.end();i++){
			delete *i;
		}
		list_search_record.clear();
	}
}

//搜索：交易明细
bool clsCASHDB::searchTransactions(sqlite3_command& cmd){
    bool bRet = true;
	//do some clear work
	clearTransactionSearchResults();

	TRY{
		sqlite3_reader reader = cmd.executereader();
		while(reader.read()){
				CASH_TRANSACT_ptr r = new CASH_TRANSACT_t;

				r->transid = reader.getint(0);
				C::newstrcpy(&r->date,reader.getstring16(1).c_str());
				r->amount = reader.getint(2);
				C::newstrcpy(&r->note,reader.getstring16(3).c_str());
				r->accountid = reader.getint(4);
				r->categoryid = reader.getint(5);
				r->toaccountid = reader.getint(6);
				r->isTransfer = reader.getint(7);
				r->personid = reader.getint(8);

				list_search_record.push_back(r);
		}
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

	if(list_search_record.empty()) bRet = false;
	return bRet;
}

//获取明细数量
int clsCASHDB::getTransactionSize(){
	int retval = 0;

	TRY{
		sqlite3_command cmd(sqlconn,
			L"select count(TRANSACTID) from '"
			TABLE_TRANSACTION
			L"';");
		retval = cmd.executeint();
	}CATCH(exception &ex){
		db_out(ex.what());
	}

	return retval;
}

//获取明细ID最大值
int clsCASHDB::getMaxTransactionID(){
	int retval = 0;
	TRY{
		sqlite3_command cmd(sqlconn,
			L"select max(TRANSACTID) from '"
			TABLE_TRANSACTION
			L"';");
		retval = cmd.executeint();
	}CATCH(exception &ex){
		db_out(ex.what());
	}

	return retval;
}

//获取明细最小日期
uRecordDate_t clsCASHDB::getMinTransactionDate(){
	uRecordDate_t retval;
	retval.Value = 0;

	TRY{
		sqlite3_command cmd(sqlconn,
			L"select min(DATE) from '"
			TABLE_TRANSACTION
			L"';");
		const wchar_t *date = cmd.executestring16().c_str();
		if(date != 0){
			DWORD y,m,d;
			swscanf(date,L"%04d-%02d-%02d",&y,&m,&d);
			retval.Date.Year = y;
			retval.Date.Month = m;
			retval.Date.Day = d;
		}
	}CATCH(exception &ex){
		db_out(ex.what());
	}

	return retval;
}

//获取明细最大日期
uRecordDate_t clsCASHDB::getMaxTransactionDate(){
	uRecordDate_t retval;
	retval.Value = 0;

	TRY{
		sqlite3_command cmd(sqlconn,
			L"select max(DATE) from '"
			TABLE_TRANSACTION
			L"';");
		const wchar_t *date = cmd.executestring16().c_str();
		if(date != 0){
			DWORD y,m,d;
			swscanf(date,L"%04d-%02d-%02d",&y,&m,&d);
			retval.Date.Year = y;
			retval.Date.Month = m;
			retval.Date.Day = d;
		}
	}CATCH(exception &ex){
		db_out(ex.what());
	}

	return retval;
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

	TRY{
		wchar_t date1[32];
		wchar_t date2[32];
		wsprintf(date1,L"%04d-%02d-%02d 00:00:00",d1.Year,d1.Month,d1.Day);
		wsprintf(date2,L"%04d-%02d-%02d 23:59:59",d2.Year,d2.Month,d2.Day);

		if(id == -1){
			sqlite3_command cmd(sqlconn,
				L"select sum(amount) from '"
				TABLE_TRANSACTION
				L"' where catgoryid in (select id from '"
				TABLE_CATEGORY
				L"' where type=0) and DATE between ? and ?;");
			cmd.bind(1,date1,lstrlen(date1)*2);	//date 1
			cmd.bind(2,date2,lstrlen(date2)*2);	//date 2

			amount = cmd.executeint();
		}else{
			sqlite3_command cmd(sqlconn,
				L"select sum(amount) from '"
				TABLE_TRANSACTION
				L"' where ((accountid=? and catgoryid in (select id from '"
				TABLE_CATEGORY
				L"' where type=0)) or (toaccountid=? and catgoryid in (select id from '"
				TABLE_CATEGORY
				L"' where type=2))) and DATE between ? and ?;");
			cmd.bind(1,id);	//accountid
			cmd.bind(2,id);	//toaccountid
			cmd.bind(3,date1,lstrlen(date1)*2);	//date 1
			cmd.bind(4,date2,lstrlen(date2)*2);	//date 2

			amount = cmd.executeint();
		}
	}CATCH(exception &ex){
		db_out(ex.what());
	}

	return amount;
}

int clsCASHDB::AccountOutById(int id,RECORDATE_ptr date, RECORDATE_ptr datend){
	RECORDATE_t d1,d2;
	int amount = 0;
	formatDate(date,datend,d1,d2);

	TRY{
		wchar_t date1[32];
		wchar_t date2[32];
		wsprintf(date1,L"%04d-%02d-%02d 00:00:00",d1.Year,d1.Month,d1.Day);
		wsprintf(date2,L"%04d-%02d-%02d 23:59:59",d2.Year,d2.Month,d2.Day);

		if(id == -1){
			sqlite3_command cmd(sqlconn,
				L"select sum(amount) from '"
				TABLE_TRANSACTION
				L"' where catgoryid in (select id from '"
				TABLE_CATEGORY
				L"' where type=1) and DATE between ? and ?;");
			cmd.bind(1,date1,lstrlen(date1)*2);	//date 1
			cmd.bind(2,date2,lstrlen(date2)*2);	//date 2

			amount = cmd.executeint();
		}else{
			sqlite3_command cmd(sqlconn,
				L"select sum(amount) from '"
				TABLE_TRANSACTION
				L"' where accountid=? and catgoryid in (select id from '"
				TABLE_CATEGORY
				L"' where type!=0) and DATE between ? and ?;");
			cmd.bind(1,id);	//accountid
			cmd.bind(2,date1,lstrlen(date1)*2);	//date 1
			cmd.bind(3,date2,lstrlen(date2)*2);	//date 2

			amount = cmd.executeint();
		}
	}CATCH(exception &ex){
		db_out(ex.what());
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
		d2.Year = datend->Year > 0 ? datend->Year : 3000;
		d2.Month = datend->Month > 0 ? datend->Month : 12;
		d2.Day = datend->Day > 0 ? datend->Day : 31;
	}else{
		d2.Year = 3000; d2.Month = 12; d2.Day = 31;
	}
}

bool clsCASHDB::getTransactionsByDate(RECORDATE_ptr date, RECORDATE_ptr datend){
	RECORDATE_t d1, d2;
	formatDate(date,datend,d1,d2);

	bool bRet = false;
	TRY{
		wchar_t date1[32];
		wchar_t date2[32];
		wsprintf(date1,L"%04d-%02d-%02d 00:00:00",d1.Year,d1.Month,d1.Day);
		wsprintf(date2,L"%04d-%02d-%02d 23:59:59",d2.Year,d2.Month,d2.Day);

		sqlite3_command cmd(sqlconn,
			L"select * from '"
			TABLE_TRANSACTION
			L"' where DATE between ? and ? order by DATE desc;");
		cmd.bind(1,date1,lstrlen(date1)*2);	//date 1
		cmd.bind(2,date2,lstrlen(date2)*2);	//date 2

		bRet = searchTransactions(cmd);
	}CATCH(exception &ex){
		db_out(ex.what());
	}
	return bRet;
}

bool clsCASHDB::getTransactionsByDate_v2(
									RECORDATE_ptr date, 
									RECORDATE_ptr datend, 
									UINT orderby){
	RECORDATE_t d1, d2;
	formatDate(date,datend,d1,d2);
	wchar_t *sql = new wchar_t[256];
	wsprintf(sql,
		L"select * from '"
		TABLE_TRANSACTION
		L"' where DATE between ? and ? order by ");
	switch(orderby){
		case 1:
			wcscat(sql,L"ACCOUNTID,DATE DESC;");
			break;
		case 2:
			wcscat(sql,L"CATGORYID,DATE DESC;");
			break;
		case 0:
		default:
			wcscat(sql,L"DATE DESC;");
			break;
	}
	bool bRet = false;
	TRY{
		wchar_t date1[32];
		wchar_t date2[32];
		wsprintf(date1,L"%04d-%02d-%02d 00:00:00",d1.Year,d1.Month,d1.Day);
		wsprintf(date2,L"%04d-%02d-%02d 23:59:59",d2.Year,d2.Month,d2.Day);
		sqlite3_command cmd(sqlconn,sql);
		cmd.bind(1,date1,lstrlen(date1)*2);	//date 1
		cmd.bind(2,date2,lstrlen(date2)*2);	//date 2

		bRet = searchTransactions(cmd);
	}CATCH(exception &ex){
		db_out(ex.what());
	}
	delete sql;
	return bRet;
}

bool clsCASHDB::getTransactionsByCategory(int id, RECORDATE_ptr date, RECORDATE_ptr datend){
	RECORDATE_t d1, d2;
	formatDate(date,datend,d1,d2);

	bool bRet = false;
	TRY{
		wchar_t date1[32];
		wchar_t date2[32];
		wsprintf(date1,L"%04d-%02d-%02d 00:00:00",d1.Year,d1.Month,d1.Day);
		wsprintf(date2,L"%04d-%02d-%02d 23:59:59",d2.Year,d2.Month,d2.Day);
		sqlite3_command cmd(sqlconn,
			L"select * from '"
			TABLE_TRANSACTION
			L"' where CATGORYID=? and DATE between ? and ? order by DATE desc;");

		cmd.bind(1,id);
		cmd.bind(2,date1,lstrlen(date1)*2);	//date 1
		cmd.bind(3,date2,lstrlen(date2)*2);	//date 2

		bRet = searchTransactions(cmd);
	}CATCH(exception &ex){
		db_out(ex.what());
	}

	return bRet;
}
bool clsCASHDB::getTransactionsByAccount(int id, RECORDATE_ptr date, RECORDATE_ptr datend){
	RECORDATE_t d1, d2;
	formatDate(date,datend,d1,d2);

	bool bRet = false;
	TRY{
		wchar_t date1[32];
		wchar_t date2[32];
		wsprintf(date1,L"%04d-%02d-%02d 00:00:00",d1.Year,d1.Month,d1.Day);
		wsprintf(date2,L"%04d-%02d-%02d 23:59:59",d2.Year,d2.Month,d2.Day);
		sqlite3_command cmd(sqlconn,
			L"select * from '"
			TABLE_TRANSACTION
			L"' where ACCOUNTID=? and DATE between ? and ? order by DATE desc;");

		cmd.bind(1,id);
		cmd.bind(2,date1,lstrlen(date1)*2);	//date 1
		cmd.bind(3,date2,lstrlen(date2)*2);	//date 2

		bRet = searchTransactions(cmd);
	}CATCH(exception &ex){
		db_out(ex.what());
	}

	return bRet;
}

bool clsCASHDB::getTransactionsByToAccount(int id, RECORDATE_ptr date, RECORDATE_ptr datend){
	RECORDATE_t d1, d2;
	formatDate(date,datend,d1,d2);

	bool bRet = false;
	TRY{
		wchar_t date1[32];
		wchar_t date2[32];
		wsprintf(date1,L"%04d-%02d-%02d 00:00:00",d1.Year,d1.Month,d1.Day);
		wsprintf(date2,L"%04d-%02d-%02d 23:59:59",d2.Year,d2.Month,d2.Day);
		sqlite3_command cmd(sqlconn,
			L"select * from '"
			TABLE_TRANSACTION
			L"' where TOACCOUNTID=? and DATE between ? and ? order by DATE desc;");

		cmd.bind(1,id);
		cmd.bind(2,date1,lstrlen(date1)*2);	//date 1
		cmd.bind(3,date2,lstrlen(date2)*2);	//date 2

		bRet = searchTransactions(cmd);
	}CATCH(exception &ex){
		db_out(ex.what());
	}

	return bRet;
}


//检查是否是重复记录：按照date, amount, account id, category id判断
int clsCASHDB::checkDupTransaction(CASH_TRANSACT_ptr pr){
	if(pr == NULL || pr->date == NULL) return -1;

	int retid = -1;	//返回存在的记录ID

	TRY{
		sqlite3_command cmd(sqlconn,
			L"select TRANSACTID from '"
			TABLE_TRANSACTION
			L"' where DATE like ? and AMOUNT=? "
			L"and CATGORYID=? and ACCOUNTID=? and TOACCOUNTID=? "
			L"and ISTRANSFER=?;");

		wchar_t sqldate[24];
		wcscpy(sqldate,pr->date);
		sqldate[10] = '%';	sqldate[11] = '\0';
		cmd.bind(1,sqldate,lstrlen(sqldate)*2);
		cmd.bind(2,pr->amount);
		cmd.bind(3,static_cast<int>(pr->categoryid));
		cmd.bind(4,static_cast<int>(pr->accountid));
		cmd.bind(5,static_cast<int>(pr->toaccountid));
		cmd.bind(6,static_cast<int>(pr->isTransfer));
		
		sqlite3_reader reader = cmd.executereader();
		while(reader.read()){
			retid = reader.getint(0);
		}
	}CATCH(exception &ex){
		db_out(ex.what());
	}
	return retid;
}

//////////////////////人员相关////////////////////
//创建：默认人员
bool clsCASHDB::createDefaultPersons(){
	bool bRet = true;
	TRY{	//create table
		sqlite3_command cmd(sqlconn,
			L"create table if not exists '"
			TABLE_PERSON
			L"' (ID integer primary key not null, NAME text not null, TYPE numeric not null);"
			);
		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

	if(!bRet) return bRet;	//创建table失败

	TRY{
		sqlite3_command cmd(sqlconn,
			L"insert into '"
			TABLE_PERSON
			L"' (ID,NAME,TYPE) "
			L"values(?,?,?);"
			);
			cmd.bind(1,0);
			cmd.bind(2,
				LOADSTRING(IDS_STR_PERSON_DEFAULT).C_Str(),
				LOADSTRING(IDS_STR_PERSON_DEFAULT).Length()*2);
			cmd.bind(3,static_cast<int>(PRSN_FAMILY));
			cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	return bRet;
}

//新增：人员
bool clsCASHDB::appendPerson(CASH_PERSON_ptr p){
	if(p == NULL || p->name == NULL) return false;

	bool bRet = true;
	TRY{
		p->id = getMaxPersonID() + 1;
		//check if is being used
		sqlite3_command cmd(sqlconn,
			L"insert into '"
			TABLE_PERSON
			L"' (ID, NAME, TYPE) values(?,?,?);");
		cmd.bind(1,static_cast<int>(p->id));
		cmd.bind(2,p->name,lstrlen(p->name)*2);
		cmd.bind(3,static_cast<int>(p->type));
		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	
	return bRet;
}

//删除：人员：根据ID
bool clsCASHDB::deletePersonById(int id){
	bool bRet = true;

	TRY{
		//检查是否正在被使用
		sqlite3_command cmd(sqlconn,
			L"select count(*) from '"
			TABLE_TRANSACTION
			L"' where PERSON_ID=?;");
		cmd.bind(1,id);
		if(cmd.executeint() > 0){
			bRet = false;
		}
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

	if(!bRet) return bRet;

	TRY{
		//从数据库中删除
		sqlite3_command cmd(sqlconn,
			L"delete from '"
			TABLE_PERSON
			L"' where ID=?;");
		cmd.bind(1,id);
		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

	return bRet;
}

//更新：人员
bool clsCASHDB::updatePerson(CASH_PERSON_ptr p){
	if(p == NULL || p->name == NULL) return false;

	bool bRet = true;

	TRY{
		//check if is being used
		sqlite3_command cmd(sqlconn,
			L"update '"
			TABLE_PERSON
			L"' set NAME=?, TYPE=?, ID=?;");
		cmd.bind(1,p->name,lstrlen(p->name)*2);
		cmd.bind(2,static_cast<int>(p->type));
		cmd.bind(3,static_cast<int>(p->id));
		cmd.executenonquery();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	
	return bRet;
}

//获取：人员：根据ID
CASH_PERSON_ptr clsCASHDB::personById(int id){
	CASH_PERSON_ptr pPerson = NULL;
	TRY{
		sqlite3_command cmd(sqlconn,
			L"select * from '"
			TABLE_PERSON
			L"' where ID=?");
		cmd.bind(1,id);
		if(searchPerson(cmd)){
			pPerson = list_search_person.front();
		}
	}CATCH(exception ex){
		db_out(ex.what());
	}
	return pPerson;
}

//获取：人员名称：根据ID
wchar_t* clsCASHDB::getPersonNameById(int id){
	CASH_PERSON_ptr p = personById(id);
	if(p){
		return p->name;
	}
	return NULL;
}

//获取：人员：最大编号
int clsCASHDB::getMaxPersonID(){
	int nRet = 0;

	TRY{
		sqlite3_command cmd(sqlconn,
			L"select max(id) from '"
			TABLE_PERSON
			L"';");
		nRet = cmd.executeint();
	}CATCH(exception &ex){
		db_out(ex.what());
	}

	return nRet;
}

//人员类型列表名称ID
const int PersonTypeStrID[] = {
	IDS_STR_PERSON_FAMILY,
	IDS_STR_PERSON_CONTACT,
	IDS_STR_PERSON_INSTITUTION
};

//获取：人员类型：名称
int clsCASHDB::getPersonTypeNameStrID(CASH_PERSON_TYPE_t tid){
	int tsize = sizeof(PersonTypeStrID)/sizeof(PersonTypeStrID[0]);
	if(tid >= tsize) tid = PRSN_FAMILY;
	return PersonTypeStrID[tid];
}

//获取：人员：类型名称列表
const int* clsCASHDB::getPersonTypeNameStrSet(int *nsize){
	if(nsize) *nsize = sizeof(PersonTypeStrID)/sizeof(PersonTypeStrID[0]);
	return PersonTypeStrID;
}

//获取：人员列表：根据类型
bool clsCASHDB::getPersonByType(CASH_PERSON_TYPE_t t){
	bool bRet = false;
	TRY{
		if( t == PRSN_ALL ){
			sqlite3_command cmd(sqlconn,
				L"select * from '"
				TABLE_PERSON
				L"' order by type");
			bRet = searchPerson(cmd);
		}else{
			sqlite3_command cmd(sqlconn,
				L"select * from '"
				TABLE_PERSON
				L"' where TYPE=?");
			cmd.bind(1,static_cast<int>(t));
			bRet = searchPerson(cmd);
		}
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	return bRet;
}

//清除：人员：搜索结果列表
void clsCASHDB::clearPersonSearchResults(){
	if(!list_search_person.empty()){
		list<CASH_PERSON_ptr>::iterator i = list_search_person.begin();
		for(;i != list_search_person.end();i++){
			delete *i;
		}
		list_search_person.clear();
	}
}

//搜索：人员
bool clsCASHDB::searchPerson(sqlite3_command& cmd){
	int resCount = 0;

	//do some clear work
	clearPersonSearchResults();

    bool bRet = true;
	TRY{
		sqlite3_reader reader = cmd.executereader();
		while(reader.read()){
			CASH_PERSON_ptr r = new CASH_PERSON_t;

			r->id = reader.getint(0);
			C::newstrcpy(&r->name,reader.getstring16(1).c_str());
			r->type = static_cast<CASH_PERSON_TYPE_t>(reader.getint(2));

			list_search_person.push_back(r);
		}
		//当无记录时
		if(list_search_person.empty()) bRet = false;
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

	return bRet;
}

//检查：人员名称：是否已存在
//如果存在则返回ID
int	clsCASHDB::checkDupPerson(CASH_PERSON_ptr pP){
	if(pP == NULL || pP->name == NULL) return -1;

	int nRet = -1;
	TRY{
		sqlite3_command cmd(sqlconn,
			L"select ID from '"
			TABLE_PERSON
			L"' where NAME=? collate nocase;");
		cmd.bind(1,pP->name);
		sqlite3_reader reader = cmd.executereader();
		while(reader.read()){
			nRet = reader.getint(0);
		}
	}CATCH(exception &ex){
		db_out(ex.what());
		nRet = -1;
	}

	return nRet;
}

#if 1
////数据库更新
bool clsCASHDB::checkDatabaseVersion_v1(){
	bool rc = false;
	TRY{
		sqlite3_command cmd(sqlconn,
			L"select count(*) from sqlite_master where name= '"
			V1_TABLE_TRANSACTION
			L"';");
        int count = cmd.executeint();
        if(count == 1){
            rc = true;
        }
	}CATCH(exception &ex){
		db_out(ex.what());
	}
	return rc;
}

void clsCASHDB::updateDatabaseVersion_v1(){
    bool bRet = true;
	TRY{
        sqlconn.executenonquery(L"ALTER TABLE '"
            V1_TABLE_TRANSACTION
            L"' RENAME TO '"
            V2_TABLE_TRANSACTION
            L"';");
	}CATCH(exception &ex){
		db_out(ex.what());
        bRet = false;
	}
    if(!bRet) return;


	if(loadTransactions()){
		list<CASH_TRANSACT_ptr>::iterator i = list_search_record.begin();
        TRY{
            sqlite3_transaction trans(sqlconn);
            sqlite3_command cmd(sqlconn,L"UPDATE '"
                TABLE_TRANSACTION
                L"' SET DATE=? WHERE TRANSACTID=?;");
            for(; i != list_search_record.end(); i++){
                CASH_TRANSACT_ptr r = *i;
                wchar_t newdate[32];
                wsprintf(newdate,L"%0s 00:00:00",r->date);
                C::newstrcpy(&r->date,newdate);
                //更新日期格式
                cmd.bind(1,r->date,lstrlen(r->date)*2);
                cmd.bind(2,(int)r->transid);
                cmd.executenonquery();
            }
            trans.commit();
        }CATCH(exception &ex){
            db_out(ex.what());
        }
		//清理工作
		clean();
	}
}

bool clsCASHDB::TableExists(wchar_t* tablename){
	bool bRet = false;
	TRY{
		sqlite3_command cmd(sqlconn,
			L"select count(*) from sqlite_master where type='table' and name= ?;");
        cmd.bind(1,tablename,lstrlen(tablename)*2);
        int count = cmd.executeint();
        if(count != 0){
            bRet = true;
        }
	}CATCH(exception &ex){
		db_out(ex.what());
	}
	return bRet;
}

bool clsCASHDB::addFieldPerson(){
	//检查是否存在PERSON_ID字段
	bool bfield = true;
	int rc = 0;
	TRY{
		sqlite3_command cmd(sqlconn,
			L"select count(PERSON_ID) from '"
            TABLE_TRANSACTION
            L"';");
        cmd.executeint();
	}CATCH(exception &ex){
		db_out(ex.what());
        bfield = false;
	}

	//字段不存在，增加字段
	bool retval = true;
	if(!bfield){
        TRY{
            sqlconn.executenonquery(L"ALTER TABLE '"
                TABLE_TRANSACTION
                L"'  ADD COLUMN PERSON_ID NUMERIC NOT NULL  DEFAULT 0;");
        }CATCH(exception &ex){
            db_out(ex.what());
            retval = false;
        }
	}
	return retval;
}

#endif
/////////////数据库操作
bool clsCASHDB::beginTrans(){
	bool bRet = true;
	TRY{
        sqlconn.executenonquery(L"begin;");
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	return bRet;
}

bool clsCASHDB::commitTrans(){
	bool bRet = true;
	TRY{
        sqlconn.executenonquery(L"commit;");
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	return bRet;
}

bool clsCASHDB::reorgDatebase(){
	bool bRet = true;
	TRY{
        sqlconn.executenonquery(L"VACUUM");
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	return bRet;
}