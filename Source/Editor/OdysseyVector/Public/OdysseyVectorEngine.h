#pragma once

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorRoot.h"

class DLLEXPORT FOdysseyVectorEngine
{
    private:
        UOdysseyVectorRoot *mScene;
        ::ULIS::FRectD mRoi;
        BLImage* mBLImage;
        BLImage* mBLMask;

    public:
        static BLContext& GetBLContext();
        BLImage& GetBLImage();
        ~FOdysseyVectorEngine();
        FOdysseyVectorEngine( double iWidth, double iHeight );
        void RenderSelected();
        void Render();
        UOdysseyVectorRoot* GetScene();
        /*void Init( double iWidth, double iHeight );*/
        void InvalidateRegion( double x, double y, double w, double h );
        void InvalidateRegion( ::ULIS::FRectD& iRegion );
        ::ULIS::FRectD& GetInvalidateRegion();
};
