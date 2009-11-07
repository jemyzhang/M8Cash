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
	int m_w; //��Բ�Ŀ�
	int m_h; //��Բ�ĸ�
	int m_B; //��ͼ�ĺ��

	int m_x;//��Բ�����Ͻ�λ��x,y
	int m_y;

	COLORREF dwcolor[20];
	COLORREF upcolor[20];

	HDC pMemDC;             //�����ڴ�DCָ��
	HBITMAP pBitmap;        //�����ڴ�λͼָ��
	                         //�����ӵĹ��캯���й������ǵĶ���
	int m_nMaxX;             //��Ļ��СX��Yֵ
	int m_nMaxY;
	bool _reqUpdate;
	bool _showLegend;
private:
	int ReformatPieary();
public:
	void drawPie(int s,int e,int xr,int yr, int w, int h,bool isDownPart = true);
};
