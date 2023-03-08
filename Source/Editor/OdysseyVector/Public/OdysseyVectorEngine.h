#pragma once

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorSegment.h"
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
        UOdysseyVectorGroup* mSelectionSpace;

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
        void PickSegments( UOdysseyVectorRoot* iScene
                         , double iX
                         , double iY
                         , double iRadius
                         , std::vector<FOdysseyVectorSegment*>& oPickedSegmentArray
                         , std::vector<double>& oDistance );
        void PickPoints( UOdysseyVectorRoot* iScene
                       , double iX
                       , double iY
                       , double iRadius
                       , std::vector<FOdysseyVectorPoint*>& oPickedPointArray );
        bool Knot( FOdysseyVectorVertex* iVertexA
                 , FOdysseyVectorVertex* iVertexB
                 , FOdysseyVectorSegment** oCreatedSegment
                 , FOdysseyVectorSegment** oRemovedSegment
                 , bool iSmooth );
        void UseImage( BLImage* iImage );
        static void RecursivePick( UOdysseyVectorGroup* iSelectionSpace, UOdysseyVectorObject& iObj, std::vector<UOdysseyVectorObject*>& iSelectedObjectArray, ::ULIS::FRectD& iRoi, uint32 iSelectionFlags );
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
        void UseColorImage(); // TODO : rename UseDefaultImage
        void GetColorImagePixelValue( uint32 iX, uint32 iY, uint8 *oR, uint8* oG, uint8 *oB, uint8 *oA );
        ::ULIS::FColor GetColorImagePixelValue( uint32 iX, uint32 iY );
        void SetSelectionSpace( UOdysseyVectorGroup* iSelectionSpace );
        UOdysseyVectorGroup* GetSelectionSpace();
};

ODYSSEYVECTOR_API FArchive& operator<<(FArchive &Ar, FOdysseyVectorEngine* iVectorEngine );
