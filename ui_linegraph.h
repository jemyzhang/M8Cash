#pragma once
// include the MZFC library header file
//#include <windows.h>
#include <mzfc_inc.h>
#include <list>
using std::list;

typedef struct LineGraphInfo{
	CMzString name;
	list<int> data;
	list<int> formatdata;
	COLORREF color;
}LineGraphInfo_t,*LineGraphInfo_ptr;

class UiLineGraph :
	public UiWin
{
public:
	UiLineGraph(void);
	~UiLineGraph(void);
protected:
	void PaintWin(HDC hdc, RECT* prcWin, RECT* prcUpdate);
public:
	virtual void SetPos(int x, int y, int w, int h, UINT flags=0);
	void appendItem(LineGraphInfo_ptr pItem);
	virtual void Update() { _reqUpdate = true; }
	void resetData();	//clear all the data
	void setLegendVisable(bool v) { _showLegend = v; }
	list<CMzString> m_xnames;

private:
	list<LineGraphInfo_ptr>	m_items;
	int m_w; //宽
	int m_h; //高
	int m_B; //画笔粗细

	int m_x;//左上角位置x,y
	int m_y;

	HDC pMemDC;             //定义内存DC指针
	HBITMAP pBitmap;        //定义内存位图指针
	                         //将在视的构造函数中构造他们的对象
	int m_nMaxX;             //屏幕大小X、Y值
	int m_nMaxY;
	int m_nItems;			//数据个数，最多的
	bool _reqUpdate;
	bool _showLegend;
	int maxval;				//数据中的最大值
private:
	int ReformatArray();
public:
	void drawLine(int x1,int y1,int x2,int y2);
};
