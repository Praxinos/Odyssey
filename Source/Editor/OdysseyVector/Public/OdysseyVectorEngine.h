#pragma once

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorRoot.h"
#include "HUD/OdysseyVectorHUD.h"

class ODYSSEYVECTOR_API FOdysseyVectorEngine
{
    private:
        ::ULIS::FRectD mRoi;
        BLContext* mBLContext;
        BLImage* mBLImage;
        BLImage* mBLMask;
        uint64 mDrawingFlags;
        std::list<FOdysseyVectorHUD*> mHUDList;

    public:
        static const uint64 RENDER_OBJECT_STRUCTURE = ( 1 << 0 );
        static const uint64 RENDER_OBJECT_BBOX      = ( 1 << 1 );

        BLContext* GetBLContext();
        BLImage* GetBLImage();
        BLImage* GetBLMask();
        ~FOdysseyVectorEngine();
        FOdysseyVectorEngine( double iWidth, double iHeight );
        void RenderSelected( UOdysseyVectorRoot& iScene );
        void RenderHUD( UOdysseyVectorRoot& iScene );
        void Render( UOdysseyVectorRoot& iScene );
        void Pick( UOdysseyVectorRoot& iScene, std::vector<::ULIS::FVec2D>& iPointArray, uint32 iSelectionFlags );

        static void RecursivePick( UOdysseyVectorObject& iObj, std::vector<UOdysseyVectorObject*>& iSelectedObjectArray, ::ULIS::FRectD& iRoi, uint32 iSelectionFlags );
        static void RecursiveErase( UOdysseyVectorObject& iObj
                                  , std::vector<UOdysseyVectorObject*>& iErasedObjectArray
                                  ,::ULIS::FRectD &iRoi
                                  , bool iSelectedOnly );
        void Erase( UOdysseyVectorRoot& iScene, ::ULIS::FRectD &iRoi, bool iSelectedOnly );

        /*void Init( double iWidth, double iHeight );*/
        void InvalidateRegion( double x, double y, double w, double h );
        void InvalidateRegion( ::ULIS::FRectD& iRegion );
        void InvalidateRegion( ::ULIS::FRectI& iRegion );
        ::ULIS::FRectD& GetInvalidateRegion();
        void SetDrawingFlags( uint64 iDrawingFlags );
        void AddHUD( FOdysseyVectorHUD* iHUDObject );
        void RemoveHUD( FOdysseyVectorHUD* iHUDObject );
        void ClearHUD();
        ::ULIS::FRectD GenerateMask( std::vector<::ULIS::FVec2D>& iPointArray );
        void UseMaskImage();
        void UseColorImage();
};

ODYSSEYVECTOR_API FArchive& operator<<(FArchive &Ar, FOdysseyVectorEngine* iVectorEngine );
