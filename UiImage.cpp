#include "UiImage.h"
#include <cMzCommon.h>
using namespace cMzCommon;

void UiImage::PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
    UiWin::PaintWin(hdcDst,prcWin,prcUpdate);
    if(pimg){
        pimg->Draw(hdcDst,prcWin,true,false);
    }
    if(pimgPath){
        ImagingHelper::DrawImage(hdcDst,prcWin,pimgPath,true,false);
    }
}

void UiImage::setupImagePath(LPWSTR path){
    if(path){
        C::newstrcpy(&pimgPath,path);
        if(pimg){
            delete pimg;
            pimg = 0;
        }
    }
}