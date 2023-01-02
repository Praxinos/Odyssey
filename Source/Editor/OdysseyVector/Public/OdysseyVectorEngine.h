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
        uint64 mDrawingFlags;

    public:
        static const uint64 RENDER_OBJECT_STRUCTURE = ( 1 << 0 );
        static const uint64 RENDER_OBJECT_BBOX      = ( 1 << 1 );

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
        void InvalidateRegion( ::ULIS::FRectI& iRegion );
        ::ULIS::FRectD& GetInvalidateRegion();
        void SetDrawingFlags( uint64 iDrawingFlags );
};
