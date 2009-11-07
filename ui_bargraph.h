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
	int m_w; //�Ӵ��Ŀ�
	int m_h; //�Ӵ��ĸ�
	int m_b; //���ĺ��

	int m_x;//�Ӵ������Ͻ�λ��x,y
	int m_y;

	COLORREF color[2];

	HDC pMemDC;             //�����ڴ�DCָ��
	HBITMAP pBitmap;        //�����ڴ�λͼָ��
	                         //�����ӵĹ��캯���й������ǵĶ���
	int m_nMaxX;             //��Ļ��СX��Yֵ
	int m_nMaxY;
	bool _reqUpdate;
	int maxval;
private:
	int ReformatBarData();
};
