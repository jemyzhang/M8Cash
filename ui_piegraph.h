#pragma once
// include the MZFC library header file
//#include <windows.h>
#include <mzfc_inc.h>
#include <list>
using std::list;

class UiPieGraph :
	public UiWin
{
public:
	UiPieGraph(void);
	~UiPieGraph(void);
protected:
	void PaintWin(HDC hdc, RECT* prcWin, RECT* prcUpdate);
public:
	virtual void SetPos(int x, int y, int w, int h, UINT flags=0);
	void appendItem(wchar_t* name, int val);
	virtual void Update() { _reqUpdate = true; }
	void resetData();	//clear all the data
	void setLegendVisable(bool v) { _showLegend = v; }
	COLORREF *GetColorTable(int &size) { size = 20; return upcolor; }
private:
	list<int>	m_datarray;
	list<int>	m_piearray;
	list<wchar_t*> m_item;
	int m_w; //椭圆的宽
	int m_h; //椭圆的高
	int m_B; //饼图的厚度

	int m_x;//椭圆的左上角位置x,y
	int m_y;

	COLORREF dwcolor[20];
	COLORREF upcolor[20];

	HDC pMemDC;             //定义内存DC指针
	HBITMAP pBitmap;        //定义内存位图指针
	                         //将在视的构造函数中构造他们的对象
	int m_nMaxX;             //屏幕大小X、Y值
	int m_nMaxY;
	bool _reqUpdate;
	bool _showLegend;
private:
	int ReformatPieary();
public:
	void drawPie(int s,int e,int xr,int yr, int w, int h,bool isDownPart = true);
};
