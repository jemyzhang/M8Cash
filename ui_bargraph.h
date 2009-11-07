#pragma once
// include the MZFC library header file
//#include <windows.h>
#include <mzfc_inc.h>
#include <list>
using std::list;

class UiBarGraph :
	public UiWin
{
public:
	UiBarGraph(void);
	~UiBarGraph(void);
protected:
	void PaintWin(HDC hdc, RECT* prcWin, RECT* prcUpdate);
public:
	virtual void SetPos(int x, int y, int w, int h, UINT flags=0);
	void appendItem(wchar_t* name, int val, int val2);
	virtual void Update() { _reqUpdate = true;  }
	void resetData();	//clear all the data
private:
	list<int>	m_datarray[2];
	list<int>	m_bararray[2];
	list<wchar_t*> m_item;
	int m_w; //视窗的宽
	int m_h; //视窗的高
	int m_b; //条的厚度

	int m_x;//视窗的左上角位置x,y
	int m_y;

	COLORREF color[2];

	HDC pMemDC;             //定义内存DC指针
	HBITMAP pBitmap;        //定义内存位图指针
	                         //将在视的构造函数中构造他们的对象
	int m_nMaxX;             //屏幕大小X、Y值
	int m_nMaxY;
	bool _reqUpdate;
	int maxval;
private:
	int ReformatBarData();
};
