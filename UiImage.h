#pragma once
// include the MZFC library header file
#include <mzfc_inc.h>

class UiImage : public UiWin
{
public:
	UiImage(void){
		pimg = 0;
        pimgPath = 0;
	}
	~UiImage(void){
	}
	void setupImage(ImagingHelper* img) {
        if(img){
            pimg = img;
            if(pimgPath){
                delete [] pimgPath;
                pimgPath = 0;
            }
        }
	}
    void setupImagePath(LPWSTR path);
	virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate);
private:
	ImagingHelper *pimg;
    LPWSTR pimgPath;
};
