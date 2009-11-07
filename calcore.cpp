#include "calcore.h"

Compare op[][4]={
		//+,-,*,/
/* + */  {B,B,S,S},
/* - */  {B,B,S,S},
/* * */  {B,B,B,B},
/* / */  {B,B,B,B},
/* # */  {S,S,S,S},
};

CalcOperator optrs[]={
	CalcOperator(OP_ADD,2,0),
	CalcOperator(OP_DEC,2,1),
	CalcOperator(OP_PLUS,2,2),
	CalcOperator(OP_DIV,2,3),
	CalcOperator(OP_NONE,0,4),
};

Compare CalcCore::GetPriority(int preop,int postop)
{
	return op[preop][postop];	
}

void CalcCore::InitStack()
{
	//清空两个栈
	while(!optr.empty()) optr.pop();
	while(!opnd.empty()) opnd.pop();
	//初始化optr，放入#
	optr.push(optrs[4]);
}

void CalcCore::PushOptr(CalcOperator oper)
{
	if(oper.GetPM()!=1)
	{
		//前一个符号比后一个符号优先级高
		while(GetPriority(optr.top().GetIndex(),oper.GetIndex())==B)
		{
			opnd.push(Calculate());
		}
		optr.push(oper);
	}
	else
	{
		//对于1目运算符作特别处理
		optr.push(oper);
		opnd.push(Calculate());
	}
}

void CalcCore::PushOpnd(double num)
{
	opnd.push(num);
}

double CalcCore::Calculate()
{
	double a,b;
	Operator optrName;
	optrName=optr.top().GetName();
	switch(optr.top().GetPM())
	{
	case 2:
		b=opnd.top();
		opnd.pop();
		a=opnd.top();
		opnd.pop();
		break;
	case 1:
		a=opnd.top();
		//弹出数字
		opnd.pop();
		b=0;
		break;
	case 0:
		a=0;b=0;
		break;
	}
	//弹出符号
	optr.pop();
	switch(optrName){
		case OP_ADD:
			return a+b;
			break;
		case OP_DEC:
			return a-b;
			break;
		case OP_PLUS:
			return a*b;
			break;
		case OP_DIV:
			if(b != 0){
				return a/b;
			}else{
				return 0.0;
			}
			break;
		default:
			return 0.0;
			break;
	}
}

CMzString CalcCore::GetResult()
{
	wchar_t result[32];
	wsprintf(result,L"%.2f",opnd.top());
	return result;
}

double CalcCore::CalculateAll()
{
	while(optr.top().GetName() != OP_NONE)
	{
		//前一个符号比后一个符号优先级高
		opnd.push(this->Calculate());
	}
	if(opnd.empty())
		return 0;
	else
		return opnd.top();
}

