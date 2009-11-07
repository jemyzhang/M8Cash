#include "ui_piegraph.h"
#include "..\MzCommon\MzCommon.h"
using namespace MzCommon;

#include "m8cash.h"

UiPieGraph::UiPieGraph(void)
:UiWin()
{
	//�û����������ñ�ͼ��λ�á���С����Ⱥ���ɫ�����
#if 0
	dwcolor[0]=RGB(204,0,51);
	dwcolor[1]=RGB(51,102,255);
	dwcolor[2]=RGB(51,204,0);
	dwcolor[3]=RGB(204,0,204);
	dwcolor[4]=RGB(255,204,51);
	dwcolor[5]=RGB(204,102,0);
	dwcolor[6]=RGB(51,51,255);
	dwcolor[7]=RGB(0,102,51);
	dwcolor[8]=RGB(153,102,255);
	dwcolor[9]=RGB(255,204,0);

	upcolor[0]=RGB(255,0,51);
	upcolor[1]=RGB(51,153,255);
	upcolor[2]=RGB(51,255,0);
	upcolor[3]=RGB(255,0,204);
	upcolor[4]=RGB(255,255,51);
	upcolor[5]=RGB(255,102,0);
	upcolor[6]=RGB(51,102,255);
	upcolor[7]=RGB(0,153,51);
	upcolor[8]=RGB(204,102,255);
	upcolor[9]=RGB(255,255,0);
#else
	//color definitions  20����ɫ
	dwcolor[0]=RGB(235,0,0);//��ɫ
	dwcolor[1]=RGB(0,235,0) ;//��ɫ
	dwcolor[2]=RGB(0,0,225);//��ɫ
	dwcolor[3]=RGB(235,235,0);//��ɫ
	dwcolor[4]=RGB(235,153,51); //��ɫ
	dwcolor[5]=RGB(235,51,153) ; //�ۺ�ɫ
	dwcolor[6]=RGB(153,0,204);//��ɫ
	dwcolor[7]=RGB(0,225,225);//����ɫ, ��ɫ
	dwcolor[8]=RGB(189,167,225);//����ɫ��
	dwcolor[9]=RGB(235,216,177);//��ɫ
	dwcolor[10]=RGB(142,235,235);//����ɫ
	dwcolor[11]=RGB(0,182,0);//������ɫ
	dwcolor[12]=RGB(180,50,0);//��ɫ��, ��ɫ��
	dwcolor[13]=RGB(0,192,192);//����ɫ
	dwcolor[14]=RGB(0,0,192);//����ɫ
	dwcolor[15]=RGB(192,192,192);//��ɫ
	dwcolor[16]=RGB(128,128,128);// ����ɫ
	dwcolor[17]=RGB(255,198,107);//�غ�ɫ��, ��ɫ
	dwcolor[18]=RGB(0,0,128);//����ɫ
	dwcolor[19]=RGB(128,0,0);//��ɫ
	/************************************/	
	upcolor[0]=RGB(255,0,0);
	upcolor[1]=RGB(0,255,0) ;
	upcolor[2]=RGB(0,0,255);
	upcolor[3]=RGB(255,255,0);
	upcolor[4]=RGB(255,153,51); //��ɫ
	upcolor[5]=RGB(255,51,153) ; //�ۺ�ɫ
	upcolor[6]=RGB(153,0,244);//��ɫ
	upcolor[7]=RGB(0,255,255);//����ɫ, ��ɫ
	upcolor[8]=RGB(199,177,255);//����ɫ��
	upcolor[9]=RGB(255,226,177);//��ɫ
	upcolor[10]=RGB(142,255,255);//����ɫ
	upcolor[11]=RGB(0,192,0);//������ɫ
	upcolor[12]=RGB(200,50,0);//��ɫ��, ��ɫ��
	upcolor[13]=RGB(0,192,192);//����ɫ
	upcolor[14]=RGB(0,0,192);//����ɫ
	upcolor[15]=RGB(192,192,192);//��ɫ
	upcolor[16]=RGB(128,128,128);// ����ɫ
	upcolor[17]=RGB(255,198,107);//�غ�ɫ��, ��ɫ
	upcolor[18]=RGB(0,0,128);//����ɫ
	upcolor[19]=RGB(128,0,0);//��ɫ

#endif
	_showLegend = false;
}

UiPieGraph::~UiPieGraph(void)
{
	resetData();
	if(pMemDC) ReleaseDC(GetParentWnd(),pMemDC);
	if(pBitmap) DeleteObject(pBitmap);
}

int UiPieGraph::ReformatPieary()//ת��Ϊ���ʵĽǶ�
{
	int num=m_datarray.size();
	double totail=0.00;
	list<int>::iterator i = m_datarray.begin();
	for(;i != m_datarray.end();i++)
	{
		totail+=*i;
	}
	i = m_datarray.begin();

	if(!m_piearray.empty()) m_piearray.clear();

	for(;i != m_datarray.end();i++)
	{
		double tmp=*i;
		m_piearray.push_back((int)(tmp/totail*360.00+0.5));
	}
	return 0;

}

void UiPieGraph::appendItem(wchar_t* name, int val){
	wchar_t *pname = 0;
	C::newstrcpy(&pname,name);

	list<int>::iterator i = m_datarray.begin();
	list<wchar_t *>::iterator ii = m_item.begin();
	for(; i != m_datarray.end(); i++,ii++){
		int d = *i;
		if(val > d){
			m_datarray.insert(i,val);
			m_item.insert(ii,pname);
			return;
		}
	}
	m_datarray.push_back(val);
	m_item.push_back(pname);
}

void UiPieGraph::drawPie(int s,int e,int xr,int yr, int w, int h,bool isDownPart){
	double pi =3.141592;
	for(int i = s; i < e; i++){
		double arc = i * pi / 180.0;
		int x1 = xr + w/2*cos(arc);
		int y1 = yr - h/2*sin(arc);
		if(isDownPart && y1 < yr) continue;
		MoveToEx(pMemDC,xr,yr,NULL);
		LineTo(pMemDC,x1,y1);
	}
}

void UiPieGraph::PaintWin(HDC hdc, RECT* prcWin, RECT* prcUpdate){
	UiWin::PaintWin(hdc,prcWin,prcUpdate);
	if(_showLegend){
		m_x = 10;
		m_y = 15;
	}else{
		m_x = 40;
		m_y = 15;
	}
	if(_reqUpdate){
		_reqUpdate = false;

		SelectObject(pMemDC, pBitmap);
		HBRUSH myBrush = CreateSolidBrush(RGB(255-16,255-16,255-16));
		RECT rect;
		rect.top = 0;
		rect.bottom = m_nMaxY;
		rect.left = 0;
		rect.right = m_nMaxX;

		FillRect(pMemDC,&rect,myBrush);//��֮ǰ�Ȳ���.
		SetTextColor(RGB(0,0,0));
		SetBkMode(pMemDC,TRANSPARENT);

		//����������ʾ��ʾ
		if(m_datarray.size() == 0){
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
			SelectObject(pMemDC,poldpen);//�ָ�ϵͳ��
			BitBlt(hdc,prcWin->left,prcWin->top,m_nMaxX,m_nMaxY,pMemDC,0,0,SRCCOPY);
			return;
		}
		
		
		double pi =3.141592; //
		int jd=0;//������ȡ���θ����ֵ�ֵ
		int prev = 0; //����������һ��ֵ
		HPEN poldpen;//��������ϵͳ��

		//ת���ɽǶ�,�û�ֻ��ֱ��������ֵ�ͺ���
		ReformatPieary();

		//�ϼ�
		list<int>::iterator v = m_datarray.begin();
		double val = 0;
		for(; v != m_datarray.end(); v++){
			val += *v;
		}
		::SetTextColor(pMemDC,RGB(0,0,0));
		RECT textrect;
		textrect.top = m_y - 15; textrect.bottom = textrect.top + 20;
		textrect.left = m_x + 10; textrect.right = m_x + 200;
		wchar_t strval[32];
		wsprintf(strval,L"�ϼ�(Ԫ) %.2f",val/100);
		HFONT font = FontHelper::GetFont(15);
		SelectObject(pMemDC,font);
		DrawText(pMemDC,strval,lstrlen(strval),&textrect,DT_TOP|DT_LEFT);
		DeleteObject(font);

		//���ָ����Σ��Ȼ������,һ��һ�������������һ��Բ����
		int cnt = 0;
		for(int j=0;j<=m_B;j++)
		{
			cnt = 0; jd = 0; prev = 0;
			COLORREF* color;
			if(j == m_B){
				color = upcolor;
			}else{
				color = dwcolor;
			}
			list<int>::iterator i = m_piearray.begin();
			for(;i != m_piearray.end();i++)
			{
				jd+=*i;
				int p=cnt%20; //10,��ɫ��
				HBRUSH brush = CreateSolidBrush(color[p]);
				SelectObject(pMemDC,brush);
				HPEN pen = CreatePen(PS_SOLID, 4,color[p]);
				SelectObject(pMemDC,pen);
				if(j == m_B){
					drawPie(prev,jd,m_x+m_w/2,m_y+m_h/2+m_B-j,m_w,m_h,false);
				}else{
					drawPie(prev,jd,m_x+m_w/2,m_y+m_h/2+m_B-j,m_w,m_h);
				}
				prev = jd;
				cnt++;
			}
		}

		if(_showLegend){
			////////////////
			//����ͼ����ǩ:
			list<wchar_t*>::iterator ii = m_item.begin();
			list<int>::iterator i = m_piearray.begin();
			list<int>::iterator m = m_datarray.begin();
			cnt = 0;
			HFONT font = FontHelper::GetFont(10);
			SelectObject(pMemDC,font);
			HPEN pen = CreatePen(PS_SOLID, 0,RGB(128,128,128));
			poldpen = (HPEN)SelectObject(pMemDC,pen);
			for(;ii != m_item.end();ii++)
			{
				int cx = m_x + m_w + 20;
				int cy = m_y;
				int ch = 15;
				int cw = 15;
				RECT rect= {cx,cy + ch*cnt,cx + cw - 2,cy + ch*cnt + ch - 2};
				Rectangle(pMemDC,rect.left,rect.top,rect.right,rect.bottom);
				RECT frect = {cx+1,cy + ch*cnt + 1,cx + cw - 3,cy + ch*cnt + ch - 3};
				HBRUSH bqbrush = CreateSolidBrush(upcolor[cnt%20]);
				FillRect(pMemDC,&frect,bqbrush);
				wchar_t text[256];
				wsprintf(text,L"%s %2.2f[%2.2f%%]",*ii,(double)(*m++)/100,(double)(*i++)*10/36);
				RECT textrect = {cx + cw + 5,cy + ch*cnt,cx + cw + m_nMaxX/2 - ch * 2,cy + ch*cnt + ch - 2};
				DrawText(pMemDC,text,lstrlen(text),&textrect,DT_TOP|DT_LEFT);
				cnt++;
			}
			/////////////////////////////////////////*/
			SelectObject(pMemDC,poldpen);//�ָ�ϵͳ��
		}
	}
	BitBlt(hdc,prcWin->left,prcWin->top,m_nMaxX,m_nMaxY,pMemDC,0,0,SRCCOPY);
	//ReleaseDC(GetParentWnd(),pMemDC);
}

void UiPieGraph::SetPos(int x, int y, int w, int h, UINT flags){
	UiWin::SetPos(x,y,w,h,flags);
	if(_showLegend){
		m_w = w/2;
	}else{
		m_w = w - 80;
	}
	if(h > 15){
		m_B = h/15;
	}else{
		m_B = 1;
	}
	m_h = h - m_B - 30;
	m_nMaxX = w;
	m_nMaxY = h;
	if(pMemDC) ReleaseDC(GetParentWnd(),pMemDC);
	if(pBitmap) DeleteObject(pBitmap);
	pMemDC = CreateCompatibleDC(GetDC(GetParentWnd()));
	pBitmap = CreateCompatibleBitmap(GetDC(GetParentWnd()),m_nMaxX,m_nMaxY);//GetWidth(),GetHeight());
}

void UiPieGraph::resetData(){
	if(!m_datarray.empty()) m_datarray.clear();
	if(!m_piearray.empty()) m_piearray.clear();
	if(!m_item.empty()){
		list<wchar_t*>::iterator i = m_item.begin();
		for(;i != m_item.end(); i++){
			delete[] *i;
		}
		m_item.clear();
	}
}
