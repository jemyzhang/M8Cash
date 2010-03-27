#include <cMzCommon.h>
using namespace cMzCommon;

#include "database.h"
#include "resource.h"

#ifdef _DEBUG
#define DEFAULT_DB		L"Program Files\\M8Cash\\cash.db"
#else
#define DEFAULT_DB		L"\\Disk\\Programs\\M8Cash\\cash.db"
#endif

//�ϰ汾����
#define OLD_TABLE_ACCOUNT	L"ACCOUNTS_v1"
#define OLD_TABEL_CATEGORY	L"CATEGORIES_v1"
#define V1_TABLE_TRANSACTION	L"RECORDS_v1"
#define V2_TABLE_TRANSACTION	L"RECORDS_v2"
//�°汾����
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
	//connect();	//����ʱ������
}

clsCASHDB::~clsCASHDB() {
    disconnect();
	clean();
}

//�����ݿ�
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
		//�ļ��½�ʱ����Ĭ�ϱ�
		if(bfileNotExists) createDefaultDatabase();
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}
	return bRet;
}

//�ر����ݿ�
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

//��������
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

    if(!bRet){  //�������ʧ�ܣ�����Ҫ�Ͽ����ݿ��������
        disconnect();
        connect();
    }
    return bRet;
}

//�������ݿ�
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

//�������ݿ�
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

//��������б���ʺ��б�
void clsCASHDB::clean(){
	cleanAccountList();
	cleanCategoryList();
}

//�����ʺźͷ���
bool clsCASHDB::load(){
	return true;
	if(!loadAccounts() ||
		!loadCategories()){ 
		return false;
	}
	return true;
}

//�ָ����ݿ�
void clsCASHDB::recover(){
	//createDefaultDatabase();
	restoreDefaultCategory();
}



/******************************************************
/* �����������
*******************************************************/

void clsCASHDB::createDefaultDatabase() {
	createDefaultAccounts();
	createDefaultCategories();
	createDefaultPersons();
	createDefaultTransaction();
}

/////////////////////////�˻����/////////////////////////////
//������Ĭ���˻�
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

	if(!bRet) return bRet;	//����tableʧ��

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

//�������˻�
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
		//��������
		this->loadAccounts();
	}
    return bRet;
}

//ɾ�����˻�����ӦID
int clsCASHDB::deleteAccountById(int id){
	const CASH_ACCOUNT_ptr beremoved = accountById(id);

	if(beremoved == NULL) return -3; //�޴��˻�

	//����Ƿ����ڱ�ʹ��
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
	if(bUsed) return -1;	//���ڱ�ʹ�ã��޷�ɾ��

	//�����ݿ���ɾ������
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

//�����˻�
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

//�����˻��б�
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

//���룺�˻�
bool clsCASHDB::loadAccounts() {
	//����ǰ�������,��ֹ�ظ�����
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

//������ʺ��б�
void clsCASHDB::cleanAccountList(){
	if(list_account.size()){
		list<CASH_ACCOUNT_ptr>::iterator c = list_account.begin();
		for(;c != list_account.end(); c++){
			delete *c;
		}
		list_account.clear();
	}
}

//����id����ʺ�
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

//����ID��ȡ�˻�����
wchar_t* clsCASHDB::getAccountNameById(int id) {
	CASH_ACCOUNT_ptr paccount = accountById(id);
	return (paccount == NULL ? NULL : paccount->name);
}

//�������������˻�
void clsCASHDB::clearAccountSearchResults(){
	if(!list_search_account.empty()){
		list<CASH_ACCOUNT_ptr>::iterator i = list_search_account.begin();
		for(;i != list_search_account.end();i++){
			delete *i;
		}
		list_search_account.clear();
	}
}

//�������˻�
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

//����Ƿ��ظ��˻�������name�ж�
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
			retid = r->id;	//���ش��ڵļ�¼id��

			if(r->initval != pa->initval || 
				r->isfix != pa->isfix ||
				lstrcmp(r->note,pa->note) != 0){	//�г�ͻ
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

//��ȡ�˻�����
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

//��ȡ�˻�ID���ֵ
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

//////////////////�������///////////////////////////
//������Ĭ�Ϸ���
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

	if(!bRet) return bRet;	//����tableʧ��

    CONST_CASH_CATEGORY_t default_categories[] = {
        {0, IDS_STR_EXPENSE, CT_OUTGOING, -1, 0},
        {1, IDS_STR_INCOME, CT_INCOME, -1, 0},
        {2, IDS_STR_TRANSFER, CT_TRANSFER, -1, 2},	//����Ϊ����Ŀ¼
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

//�ָ���Ĭ�Ϸ���
bool clsCASHDB::restoreDefaultCategory() {
    CONST_CASH_CATEGORY_t default_categories[] = {
        {0, IDS_STR_EXPENSE, CT_OUTGOING, -1, 0},
        {1, IDS_STR_INCOME, CT_INCOME, -1, 0},
        {2, IDS_STR_TRANSFER, CT_TRANSFER, -1, 2},	//����Ϊ����Ŀ¼
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
//����������
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
	
	//���ݿ�д��ɹ���ͬʱ���ӵ������б��������ݿ������
	if(bRet){
		//��������
		this->loadCategories();
	}
    return bRet;
}

//ɾ������ӦID������
int clsCASHDB::deleteCategoryById(int id){
	CASH_CATEGORY_ptr beremoved = categoryById(id);
	if(beremoved == NULL) return -3; //�޴˷���

	if(beremoved->level == 0) return -2; //�����࣬����ɾ��

	if(beremoved->level < 2){	//����Ƿ�������
	    list<CASH_CATEGORY_ptr>::iterator i = list_category.begin();
		for (; i!=list_category.end(); i++){
			CASH_CATEGORY_ptr chk = *i;
			if(chk->parentid == id){
				return -1;	//�����࣬�޷�ɾ��
			}
		}
	}

	//����Ƿ����ڱ�ʹ��
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

	if(bUsed) return -1;	//���ڱ�ʹ�ã��޷�ɾ��

	//�����ݿ���ɾ������
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

//���£������б�
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

//���£�����
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

//���룺����
bool clsCASHDB::loadCategories() {
	//����ǰ�������,��ֹ�ظ�����
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

//����������б�
void clsCASHDB::cleanCategoryList(){
	if(list_category.size()){
		list<CASH_CATEGORY_ptr>::iterator t = list_category.begin();
		for(;t != list_category.end(); t++){
			delete *t;
		}
		list_category.clear();
	}
}

//����ID��÷���
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

//����ID��ȡ��������
wchar_t* clsCASHDB::getCategoryNameById(int id) {
	CASH_CATEGORY_ptr pcategory = categoryById(id);
	return (pcategory == NULL ? NULL : pcategory->name);
}

//����ID��ȡ����ȫ�����������ࣩ
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
			nodeidx = -1;	//�������ҵ���Ӧcategoryʱ����
		}
    } while (nodeidx != -1);
	
	//�����ַ����ռ�
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

//����������������
void clsCASHDB::clearCategorySearchResults(){
	if(!list_search_category.empty()){
		list<CASH_CATEGORY_ptr>::iterator i = list_search_category.begin();
		for(;i != list_search_category.end();i++){
			delete *i;
		}
		list_search_category.clear();
	}
}

//����������
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

//����Ƿ�Ϊ�ӷ���
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

//����Ƿ��ظ����ࣺ����name�ж�
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

//��ȡ���ࣺ���ݼ���
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

//��ȡ��������
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

//��ȡ����ID���ֵ
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

/////////�������
//������Ĭ�Ͻ�����ϸ
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
//������������ϸ
bool clsCASHDB::appendTransaction(CASH_TRANSACT_ptr r) {
    bool bRet = true;
	TRY{
		//��ȡ���Ľ���ID
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

//���룺������ϸ
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

//����ID��ý�����ϸ
bool clsCASHDB::recordById(int id,CASH_TRANSACT_ptr pr) {
	bool bRet = true;
	if(pr == NULL) return false;	//ָ�����

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


//ɾ������ӦID��������ϸ
bool clsCASHDB::deleteTransactionById(int id){
	bool bRet = true;
	//�����ݿ���ɾ������
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

//���£�������ϸ
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

//������������������ϸ
void clsCASHDB::clearTransactionSearchResults(){
	if(!list_search_record.empty()){
		list<CASH_TRANSACT_ptr>::iterator i = list_search_record.begin();
		for(;i != list_search_record.end();i++){
			delete *i;
		}
		list_search_record.clear();
	}
}

//������������ϸ
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

//��ȡ��ϸ����
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

//��ȡ��ϸID���ֵ
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

//��ȡ��ϸ��С����
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

//��ȡ��ϸ�������
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


//����Ƿ����ظ���¼������date, amount, account id, category id�ж�
int clsCASHDB::checkDupTransaction(CASH_TRANSACT_ptr pr){
	if(pr == NULL || pr->date == NULL) return -1;

	int retid = -1;	//���ش��ڵļ�¼ID

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

//////////////////////��Ա���////////////////////
//������Ĭ����Ա
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

	if(!bRet) return bRet;	//����tableʧ��

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

//��������Ա
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

//ɾ������Ա������ID
bool clsCASHDB::deletePersonById(int id){
	bool bRet = true;

	TRY{
		//����Ƿ����ڱ�ʹ��
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
		//�����ݿ���ɾ��
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

//���£���Ա
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

//��ȡ����Ա������ID
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

//��ȡ����Ա���ƣ�����ID
wchar_t* clsCASHDB::getPersonNameById(int id){
	CASH_PERSON_ptr p = personById(id);
	if(p){
		return p->name;
	}
	return NULL;
}

//��ȡ����Ա�������
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

//��Ա�����б�����ID
const int PersonTypeStrID[] = {
	IDS_STR_PERSON_FAMILY,
	IDS_STR_PERSON_CONTACT,
	IDS_STR_PERSON_INSTITUTION
};

//��ȡ����Ա���ͣ�����
int clsCASHDB::getPersonTypeNameStrID(CASH_PERSON_TYPE_t tid){
	int tsize = sizeof(PersonTypeStrID)/sizeof(PersonTypeStrID[0]);
	if(tid >= tsize) tid = PRSN_FAMILY;
	return PersonTypeStrID[tid];
}

//��ȡ����Ա�����������б�
const int* clsCASHDB::getPersonTypeNameStrSet(int *nsize){
	if(nsize) *nsize = sizeof(PersonTypeStrID)/sizeof(PersonTypeStrID[0]);
	return PersonTypeStrID;
}

//��ȡ����Ա�б���������
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

//�������Ա����������б�
void clsCASHDB::clearPersonSearchResults(){
	if(!list_search_person.empty()){
		list<CASH_PERSON_ptr>::iterator i = list_search_person.begin();
		for(;i != list_search_person.end();i++){
			delete *i;
		}
		list_search_person.clear();
	}
}

//��������Ա
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
		//���޼�¼ʱ
		if(list_search_person.empty()) bRet = false;
	}CATCH(exception &ex){
		db_out(ex.what());
		bRet = false;
	}

	return bRet;
}

//��飺��Ա���ƣ��Ƿ��Ѵ���
//��������򷵻�ID
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
////���ݿ����
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
                //�������ڸ�ʽ
                cmd.bind(1,r->date,lstrlen(r->date)*2);
                cmd.bind(2,(int)r->transid);
                cmd.executenonquery();
            }
            trans.commit();
        }CATCH(exception &ex){
            db_out(ex.what());
        }
		//������
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
	//����Ƿ����PERSON_ID�ֶ�
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

	//�ֶβ����ڣ������ֶ�
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
/////////////���ݿ����
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