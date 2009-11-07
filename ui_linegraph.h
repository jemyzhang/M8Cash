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
	int m_w; //��
	int m_h; //��
	int m_B; //���ʴ�ϸ

	int m_x;//���Ͻ�λ��x,y
	int m_y;

	HDC pMemDC;             //�����ڴ�DCָ��
	HBITMAP pBitmap;        //�����ڴ�λͼָ��
	                         //�����ӵĹ��캯���й������ǵĶ���
	int m_nMaxX;             //��Ļ��СX��Yֵ
	int m_nMaxY;
	int m_nItems;			//���ݸ���������
	bool _reqUpdate;
	bool _showLegend;
	int maxval;				//�����е����ֵ
private:
	int ReformatArray();
public:
	void drawLine(int x1,int y1,int x2,int y2);
};
