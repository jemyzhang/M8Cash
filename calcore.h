#include <mzfc_inc.h>
#include <stack>
//using namespace std;
using std::stack;

enum Operator{
	OP_NONE = 0,
	OP_ADD = 1,
	OP_DEC = 2,
	OP_PLUS = 3,
	OP_DIV = 4,
};

class CalcOperator
{
public:
	CalcOperator(Operator name = OP_NONE,int pm = 0,int index = 0){
		_name = name;
		_pm = pm;
		_index = index;
	}
	Operator GetName(){return _name;}
	int GetPM(){return _pm;}
	int GetIndex(){return _index;}
private:
	Operator _name;	//运算符名称
	int _pm;	//几目运算符
	int _index;	//索引
};

enum Compare{B,S,E,U};		//bigger,smaller, equal, unknown

class CalcCore
{
public:
	CalcCore(){
	}
	Compare GetPriority(int preop,int postop);
	double CalculateAll();
	double Calculate();
	void InitStack();
	void PushOptr(CalcOperator myop);
	void PushOpnd(double mynum);
	CMzString GetResult();

private:
	stack<CalcOperator> optr;	//运算符栈
	stack<double> opnd;	//数字栈
};
