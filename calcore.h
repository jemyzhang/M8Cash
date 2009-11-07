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
	Operator _name;	//���������
	int _pm;	//��Ŀ�����
	int _index;	//����
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
	stack<CalcOperator> optr;	//�����ջ
	stack<double> opnd;	//����ջ
};
