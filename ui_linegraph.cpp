#include "ui_linegraph.h"
#include <MzCommon.h>
using namespace MzCommon;
#include "m8cash.h"

UiLineGraph::UiLineGraph(void)
:UiWin()
{
	_showLegend = true;
	m_nItems = 0;
	maxval = 0;
}

UiLineGraph::~UiLineGraph(void)
{
	resetData();
	if(pMemDC) ReleaseDC(GetParentWnd(),pMemDC);
	if(pBitmap) DeleteObject(pBitmap);
}

int UiLineGraph::ReformatArray()//转换为合适的坐标
{
	int sz = m_items.size();
	if(sz == 0) return 0;

	//find the max value
	list<LineGraphInfo_ptr>::iterator it = m_items.begin();
	for(; it != m_items.end(); it++){
		LineGraphInfo_ptr lg = *it;
		if(lg->data.size() == 0) continue;

		list<int>::iterator idt = lg->data.begin();
		for(; idt != lg->data.end(); idt++){
			int dt = *idt;
			dt = dt < 0 ? dt * -1 : dt;
			if(dt > maxval) maxval = dt;
		}
	}

	maxval = (maxval/10000 + 1) * 10000;	//整数，四舍五入

	//reformat value
	it = m_items.begin();
	for(; it != m_items.end(); it++){
		LineGraphInfo_ptr lg = *it;
		int dsz = lg->data.size();
		if(dsz == 0) continue;
		if(dsz > m_nItems) m_nItems = dsz;

		list<int>::iterator idt = lg->data.begin();
		for(; idt != lg->data.end(); idt++){
			int dt = *idt;
			if(maxval == 0){
				lg->formatdata.push_back(0);
			}else{
				lg->formatdata.push_back(dt*100/maxval);	//放大64倍
			}
		}
	}

	return 0;
}

void UiLineGraph::appendItem(LineGraphInfo_ptr pItem){
	if(pItem == 0) return;

	LineGraphInfo_ptr p = new LineGraphInfo_t;
	//data copy
	p->name = pItem->name;
	p->color = pItem->color;
	int sz = pItem->data.size();
	if(sz){
		list<int>::iterator it = pItem->data.begin();
		for(;it != pItem->data.end(); it++){
			p->data.push_back(*it);
		}
	}
	//push back item
	m_items.push_back(p);
}
void UiLineGraph::drawLine(int x1,int y1,int x2,int y2){
	if(x1 == x2 && y1 == y2) return;
	MoveToEx(pMemDC,x1,y1,NULL);
	LineTo(pMemDC,x2,y2);
}

void UiLineGraph::PaintWin(HDC hdc, RECT* prcWin, RECT* prcUpdate){
	UiWin::PaintWin(hdc,prcWin,prcUpdate);
	m_x = 70;
	m_y = m_h - 20;

	if(_reqUpdate){
		_reqUpdate = false;

		SelectObject(pMemDC, pBitmap);
		HBRUSH myBrush = CreateSolidBrush(RGB(255-16,255-16,255-16));
		RECT rect;
		rect.top = 0;
		rect.bottom = m_nMaxY;
		rect.left = 0;
		rect.right = m_nMaxX;

		FillRect(pMemDC,&rect,myBrush);//画之前先擦除.
		SetTextColor(RGB(0,0,0));
		SetBkMode(pMemDC,TRANSPARENT);

		//无数据是显示提示
		if(m_items.size() == 0){
			HFONT font = FontHelper::GetFont(30);
			SelectObject(pMemDC,font);
			HPEN pen = CreatePen(PS_SOLID, 0,RGB(128,128,128));
			HPEN poldpen = (HPEN)SelectObject(pMemDC,pen);
			int cx = m_x + m_nMaxX / 2;
			int cy = m_y + m_nMaxY / 2 - 20 * 5;
			int ch = m_nMaxX / 2;
			int cw = m_nMaxY / 2;
			RECT textrect = {cx,cy,cx+cw,cy+ch};
			wchar_t text[16];
			wsprintf(text,LOADSTRING(IDS_STR_NO_DATA).C_Str());
			DrawText(pMemDC,text,lstrlen(text),&textrect,DT_TOP|DT_LEFT);
			SelectObject(pMemDC,poldpen);//恢复系统笔
			BitBlt(hdc,prcWin->left,prcWin->top,m_nMaxX,m_nMaxY,pMemDC,0,0,SRCCOPY);
			return;
		}
		
		
		HPEN poldpen;//用来保存系统笔

		//转换成相对值,用户只需直接输入数值就好了
		ReformatArray();
		//设置绘画高度和间隔
		int stepwidth = m_w / m_nItems;
		int viewRectHeight = (m_h/2 - 15);
		int sy = m_y/2 + 10;
		int stepheight = (m_h - 15) / 10;
		m_y = sy + stepheight*5;
		//绘制方格
		//竖线
		HBRUSH brush = CreateSolidBrush(RGB(128,128,128));
		SelectObject(pMemDC,brush);
		HPEN pen = CreatePen(PS_DASH, 1,RGB(128,128,128));
		SelectObject(pMemDC,pen);
		for(int i = 0; i <= m_nItems; i++){
			drawLine(m_x + i*stepwidth,sy - viewRectHeight,m_x + i*stepwidth,m_y);
		}
		
		int i = 0;
		drawLine(m_x,sy - viewRectHeight,m_x + m_nItems*stepwidth, sy - viewRectHeight);
		drawLine(m_x,m_y,m_x + m_nItems*stepwidth, m_y);
		while(1){
			int y1 = sy - stepheight*i;
			int y2 = sy + stepheight*i;
			if(y1 < sy - viewRectHeight || y2 > m_y) break;
			drawLine(m_x,y1,m_x + m_nItems*stepwidth, y1);
			drawLine(m_x,y2,m_x + m_nItems*stepwidth, y2);
			i++;
		}
		//横线
		DeleteObject(brush);
		DeleteObject(pen);
		//绘制折线
		//viewRectHeight -= 10;
		list<LineGraphInfo_ptr>::iterator it = m_items.begin();
		int gsy = sy + (m_B + 1);
		for(; it != m_items.end(); it++){
			LineGraphInfo_ptr lg = *it;
			if(lg->formatdata.size() == 0) continue;

			HBRUSH brush = CreateSolidBrush(lg->color);
			SelectObject(pMemDC,brush);
			HPEN pen = CreatePen(PS_SOLID, m_B,lg->color);
			SelectObject(pMemDC,pen);

			list<int>::iterator idt = lg->formatdata.begin();
			int predt = *idt++;
			int x0 = m_x;
			int y0 = gsy - ((viewRectHeight*predt)/100);
			int cnt = 1;
			for(; idt != lg->formatdata.end(); idt++){
				int dt = *idt;
				int x1 = m_x + cnt*stepwidth;
				int y1 = gsy - ((viewRectHeight*dt)/100);
				drawLine(x0,y0,x1,y1);
				x0 = x1;
				y0 = y1;
				cnt++;
			}
			DeleteObject(brush);
			DeleteObject(pen);
			gsy -= (m_B + 1);
		}

		//绘制y轴
		HFONT font = FontHelper::GetFont(15);
		SelectObject(pMemDC,font);
		pen = CreatePen(PS_SOLID, 4,RGB(0,0,0));
		poldpen = (HPEN)SelectObject(pMemDC,pen);

		RECT textrect = {0, sy - 10,65,sy + 10};
		wchar_t strval[36];
		strval[0] = '0';
		strval[1] = 0;
		DrawText(pMemDC,strval,1,&textrect,DT_VCENTER|DT_RIGHT);

		RECT textrect2;
		textrect2.left = textrect.left;
		textrect2.right = textrect.right;
		int maxYs = 5;
		if(maxval < 600){
			maxYs = maxval/100-1;
		}
		int stepval = maxval/(100*5);
		for(int i = 1; i <= maxYs; i++){
			textrect2.top = textrect.top - i * stepheight;
			textrect2.bottom = textrect2.top + 20;
			wsprintf(strval,L"%d",stepval*i);
			DrawText(pMemDC,strval,lstrlen(strval),&textrect2,DT_VCENTER|DT_RIGHT);
			textrect2.top = textrect.top + i * stepheight;
			textrect2.bottom = textrect2.top + 20;
			wsprintf(strval,L"%d",stepval*i*(-1));
			DrawText(pMemDC,strval,lstrlen(strval),&textrect2,DT_VCENTER|DT_RIGHT);
		}
		
		//m_x + i*stepwidth,sy - viewRectHeight,m_x + i*stepwidth,m_y
		textrect.top = m_y + 5;
		textrect.bottom = m_y + 20;
		list<CMzString>::iterator istr = m_xnames.begin();
		int cnt = 0;
		for(; istr != m_xnames.end(); istr++){
			if(cnt%2 == 0){
				textrect.left = m_x - stepwidth/2 + cnt * stepwidth;
				textrect.right = textrect.left + 60;
				CMzString s = *istr;
				DrawText(pMemDC,s.C_Str(),s.Length(),&textrect,DT_LEFT|DT_VCENTER);
			}
			cnt++;
		}
		DeleteObject(pen);

		if(_showLegend){
			////////////////
			//绘制图例标签:
			list<LineGraphInfo_ptr>::iterator it = m_items.begin();

			HFONT font = FontHelper::GetFont(15);
			SelectObject(pMemDC,font);
			int cnt = 0;
			int lsx = m_x + m_w - 200;
			int lsy = sy - viewRectHeight;
			for(;it != m_items.end();it++)
			{
				LineGraphInfo_ptr pitem = *it;
				int x1 = lsx;
				int y1 = lsy + 30 * cnt;
				int x2 = x1 + 30;
				int y2 = y1;

				HPEN pen = CreatePen(PS_SOLID, 4,pitem->color);
				poldpen = (HPEN)SelectObject(pMemDC,pen);
				drawLine(x1,y1 + 10,x2,y2 + 10);

				RECT textrect = {x1 + 35, y1,x1 + 200,y1 + 30};
				DrawText(pMemDC,pitem->name.C_Str(),pitem->name.Length(),&textrect,DT_TOP|DT_LEFT);
				cnt++;
				DeleteObject(pen);
			}
			/////////////////////////////////////////*/
			SelectObject(pMemDC,poldpen);//恢复系统笔
		}

	}
	BitBlt(hdc,prcWin->left,prcWin->top,m_nMaxX,m_nMaxY,pMemDC,0,0,SRCCOPY);
}

void UiLineGraph::SetPos(int x, int y, int w, int h, UINT flags){
	UiWin::SetPos(x,y,w,h,flags);
	m_w = w - 80;
	m_h = h - 40;

	m_B = 3;

	m_nMaxX = w;
	m_nMaxY = h;

	if(pMemDC) ReleaseDC(GetParentWnd(),pMemDC);
	if(pBitmap) DeleteObject(pBitmap);
	pMemDC = CreateCompatibleDC(GetDC(GetParentWnd()));
	pBitmap = CreateCompatibleBitmap(GetDC(GetParentWnd()),m_nMaxX,m_nMaxY);//GetWidth(),GetHeight());
}

void UiLineGraph::resetData(){
	int sz = m_items.size();
	if(sz == 0) return;

	list<LineGraphInfo_ptr>::iterator it = m_items.begin();
	for(; it != m_items.end(); it++){
		LineGraphInfo_ptr lg = *it;
		lg->name.SetBufferSize(0);
		if(lg->data.size()) lg->data.clear();
		if(lg->formatdata.size()) lg->formatdata.clear();
	}
	m_items.clear();
}
