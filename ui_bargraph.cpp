#include "ui_bargraph.h"
#include "..\MzCommon\MzCommon.h"
using namespace MzCommon;

#include "m8cash.h"

UiBarGraph::UiBarGraph(void)
:UiWin()
{
	color[0] = RGB(42,235,235);
	color[1] = RGB(235,53,51);
	_reqUpdate = true;
	maxval = 0;
}

UiBarGraph::~UiBarGraph(void)
{
	resetData();
	if(pMemDC) ReleaseDC(GetParentWnd(),pMemDC);
	if(pBitmap) DeleteObject(pBitmap);
}

int UiBarGraph::ReformatBarData()//转换为合适的长度
{
	list<int>::iterator i0 = m_datarray[0].begin();
	list<int>::iterator i1 = m_datarray[1].begin();
	for(;i0 != m_datarray[0].end();i0++,i1++)
	{
		if(*i0 > maxval) maxval = *i0;
		if(*i1 > maxval) maxval = *i1;
	}

	i0 = m_datarray[0].begin();
	i1 = m_datarray[1].begin();

	if(!m_bararray[0].empty()) {
		m_bararray[0].clear();
		m_bararray[1].clear();
	}

	for(;i0 != m_datarray[0].end();i0++,i1++)
	{
		double tmp=*i0;
		m_bararray[0].push_back((int)(tmp/maxval*(m_w/2 - 10)+0.5));
		tmp=*i1;
		m_bararray[1].push_back((int)(tmp/maxval*(m_w/2 - 10)+0.5));
	}
	return 0;

}

void UiBarGraph::appendItem(wchar_t* name, int val, int val2){
	m_datarray[0].push_back(val);
	m_datarray[1].push_back(val2);
	wchar_t *pname = 0;
	C::newstrcpy(&pname,name);
	m_item.push_back(pname);
}

void UiBarGraph::PaintWin(HDC hdc, RECT* prcWin, RECT* prcUpdate){
	UiWin::PaintWin(hdc,prcWin,prcUpdate);
	m_x = 40;
	m_y = 15;
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
		if(m_datarray[0].size() == 0){
			HFONT font = FontHelper::GetFont(30);
			SelectObject(pMemDC,font);
			HPEN pen = CreatePen(PS_SOLID, 0,RGB(128,128,128));
			HPEN poldpen = (HPEN)SelectObject(pMemDC,pen);
			int cx = 0;//m_x + m_nMaxX / 2 - 20 * 5;
			int cy = 0;//m_y + m_nMaxY / 2 - 20 * 5;
			int ch = m_nMaxY;//m_nMaxX / 2;
			int cw = m_nMaxX;//m_nMaxY / 2;
			RECT textrect = {cx,cy,cx+cw,cy+ch};
			wchar_t text[16];
			wsprintf(text,LOADSTRING(IDS_STR_NO_DATA).C_Str());
			DrawText(pMemDC,text,lstrlen(text),&textrect,DT_CENTER | DT_VCENTER);//DT_TOP|DT_LEFT);
			SelectObject(pMemDC,poldpen);//恢复系统笔
			BitBlt(hdc,prcWin->left,prcWin->top,m_nMaxX,m_nMaxY,pMemDC,0,0,SRCCOPY);
			return;
		}
		
		ReformatBarData();
		//绘制坐标
		//纵坐标 月份
		int xr = m_x + (m_w/2 - 10);
		int yr = m_y + m_h - 20;
		int x1 = m_x + (m_w/2 - 10);
		int y1 = m_y;
		MoveToEx(pMemDC,xr,yr,NULL);
		LineTo(pMemDC,x1,y1);
		//横坐标 金额
		MoveToEx(pMemDC,xr - (m_w/2 - 10),yr,NULL);
		x1 = m_x + m_w;
		y1 = m_y + m_h - 20;
		LineTo(pMemDC,x1,y1);
		//绘制坐标文字
		HFONT font = FontHelper::GetFont(17);
		SelectObject(pMemDC,font);
		::SetTextColor(pMemDC,RGB(0,0,0));
		//月份
		RECT textrect;//(m_x - 34,m_y);
		textrect.top = m_y - 10; textrect.bottom = textrect.top + 20;
		textrect.left = m_x  + (m_w/2 - 10) - 34; textrect.right = textrect.left + 34;
		DrawText(pMemDC,L"月份",2,&textrect,DT_TOP|DT_LEFT);
		//金额
		textrect.top = m_y + m_h - 15; textrect.bottom = textrect.top + 20;
		textrect.left = m_x + m_w - 60; textrect.right = m_x + m_w + 10;
		DrawText(pMemDC,L"金额(元)",5,&textrect,DT_TOP|DT_RIGHT);
		//结余
		list<int>::iterator v0 = m_datarray[0].begin();
		list<int>::iterator v1 = m_datarray[1].begin();
		double balval = 0;
		for(; v0 != m_datarray[0].end(); v0++,v1++){
			balval += *v0;
			balval -= *v1;
		}
		::SetTextColor(pMemDC,balval >= 0 ? RGB(145,211,234) : RGB(255,151,151));
		textrect.top = m_y + m_h - 15; textrect.bottom = textrect.top + 20;
		textrect.left = m_x + 20; textrect.right = m_x + 200;
		wchar_t strbalval[32];
		wsprintf(strbalval,L"结余(元) %.2f",balval/100);
		DrawText(pMemDC,strbalval,lstrlen(strbalval),&textrect,DT_TOP|DT_LEFT);

		::SetTextColor(pMemDC,RGB(0,0,0));
		//绘制条状
		list<int>::iterator i0 = m_bararray[0].begin();
		list<int>::iterator i1 = m_bararray[1].begin();
		v0 = m_datarray[0].begin();
		v1 = m_datarray[1].begin();
		int cnt = 0;
		RECT rect0,rect00;
		RECT rect1,rect10;
		int middle_x = m_x + (m_w/2 - 10);
		rect0.top = m_y + 10; rect0.bottom = rect0.top + m_b;
		rect0.left = middle_x; rect0.right = middle_x;

		rect1.top = rect0.bottom; rect1.bottom = rect0.bottom + m_b;
		rect1.left = middle_x; rect1.right = middle_x;
		RECT textrect0,textrect1;
		for(; i0 != m_bararray[0].end(); i0++,i1++,v0++,v1++){
			rect0.top = m_y + 10 + cnt * ( m_b * 3 / 2 );
			rect0.bottom = rect0.top + m_b;
			rect0.right = rect0.left + *i0;
			rect00 = rect0;
			rect00.left = rect0.right;
			rect00.right = rect0.right + 12;

			rect1.top = rect0.top;
			rect1.bottom = rect1.top + m_b;
			rect1.left = rect1.right - *i1;
			rect10 = rect1;
			rect10.right = rect1.left;
			rect10.left = rect1.left - 12;
			pimg[IDB_PNG_IN_BAR - IDB_PNG_BEGIN]->Draw(pMemDC,&rect0,true);
			pimg[IDB_PNG_IN_HEAD - IDB_PNG_BEGIN]->Draw(pMemDC,&rect00);

			pimg[IDB_PNG_OUT_BAR - IDB_PNG_BEGIN]->Draw(pMemDC,&rect1,true);
			pimg[IDB_PNG_OUT_HEAD - IDB_PNG_BEGIN]->Draw(pMemDC,&rect10);

			//绘制数值
			wchar_t valstr[16];
			double val = *v0;
			if(val > 0){
				wsprintf(valstr,L"%.2f",val/100);
				textrect0.top = rect0.top; textrect0.bottom = rect0.bottom;
				if(rect0.right < middle_x + 100){
					textrect0.left = rect0.right + 12; textrect0.right = textrect0.left + 100;
					DrawText(pMemDC,valstr,lstrlen(valstr),&textrect0,DT_VCENTER|DT_LEFT);
				}else{
					textrect0.left = rect0.right - 100; textrect0.right = rect0.right;
					DrawText(pMemDC,valstr,lstrlen(valstr),&textrect0,DT_VCENTER|DT_RIGHT);
				}
			}
			val = *v1;
			if(val > 0){
				wsprintf(valstr,L"%.2f",val/100);
				textrect1.top = rect1.top; textrect1.bottom = rect1.bottom;
				if(rect1.left > middle_x - 100){
					textrect1.left = rect1.left - 100 - 12; textrect1.right = rect1.left - 12;
					DrawText(pMemDC,valstr,lstrlen(valstr),&textrect1,DT_VCENTER|DT_RIGHT);
				}else{
					textrect1.left = rect1.left; textrect1.right = rect1.left + 100;
					DrawText(pMemDC,valstr,lstrlen(valstr),&textrect1,DT_VCENTER|DT_LEFT);
				}
			}
			cnt++;
		}
		//月份标记
		textrect.left = m_x  + (m_w/2 - 10) - 12; textrect.right = textrect.left + 24;
		list<wchar_t*>::iterator i = m_item.begin();
		cnt = 0;
		::SetTextColor(pMemDC,RGB(255,255,255));
		for(; i != m_item.end(); i++){
			wchar_t* str = *i;
			textrect.top = m_y + 10 + cnt * (m_b*3/2); textrect.bottom = textrect.top + 24;
			DrawText(pMemDC,str,lstrlen(str),&textrect,DT_VCENTER|DT_CENTER);
			cnt ++;
		}
	}
	BitBlt(hdc,prcWin->left,prcWin->top,m_nMaxX,m_nMaxY,pMemDC,0,0,SRCCOPY);
	//ReleaseDC(GetParentWnd(),pMemDC);
}

void UiBarGraph::SetPos(int x, int y, int w, int h, UINT flags){
	UiWin::SetPos(x,y,w,h,flags);
	m_w = w - 60;
	m_h = h - 20*2;
	if(h > 30){
		m_b = m_h/30;
	}else{
		m_b = 1;
	}
	m_b = 24;
	m_nMaxX = w;
	m_nMaxY = h;
	if(pMemDC) ReleaseDC(GetParentWnd(),pMemDC);
	if(pBitmap) DeleteObject(pBitmap);
	pMemDC = CreateCompatibleDC(GetDC(GetParentWnd()));
	pBitmap = CreateCompatibleBitmap(GetDC(GetParentWnd()),m_nMaxX,m_nMaxY);//GetWidth(),GetHeight());
}

void UiBarGraph::resetData(){
	for(int i = 0; i < 2; i++){
		if(!m_datarray[i].empty()) m_datarray[i].clear();
		if(!m_bararray[i].empty()) m_bararray[i].clear();
	}
	if(!m_item.empty()){
		list<wchar_t*>::iterator i = m_item.begin();
		for(;i != m_item.end(); i++){
			delete[] *i;
		}
		m_item.clear();
	}
	maxval = 0;
}
