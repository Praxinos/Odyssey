#pragma once

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorSegment.h"
#include "OdysseyVectorScene.h"
#include "HUD/OdysseyVectorHUD.h"

class ODYSSEYVECTOR_API FOdysseyVectorEngine
{

    public:
        BLContext* GetBLContext();
        BLImage* GetBLImage();
        BLImage* GetBLMask();
        ~FOdysseyVectorEngine();
        FOdysseyVectorEngine( double iWidth, double iHeight );
        void RenderSelected( UOdysseyVectorScene& iScene );

        void Render( UOdysseyVectorScene& iScene );
        void Pick( UOdysseyVectorScene& iScene, std::vector<::ULIS::FVec2D>& iPointArray, uint32 iSelectionFlags );
        void PickSegments( UOdysseyVectorScene* iScene
                         , double iX
                         , double iY
                         , double iRadius
                         , std::vector<FOdysseyVectorSegment*>& oPickedSegmentArray
                         , std::vector<double>& oDistance );
        void PickPoints( UOdysseyVectorScene* iScene
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
        void Erase( UOdysseyVectorScene& iScene, ::ULIS::FRectD &iRoi, bool iSelectedOnly );

        /*void Init( double iWidth, double iHeight );*/
        void InvalidateRegion( double x, double y, double w, double h );
        void InvalidateRegion( ::ULIS::FRectD& iRegion );
        void InvalidateRegion( ::ULIS::FRectI& iRegion );
        ::ULIS::FRectD& GetInvalidateRegion();
        void SetDrawingFlags( uint64 iDrawingFlags );
        void AddHUD( FOdysseyVectorHUD* iHUDObject );
        void RemoveHUD( FOdysseyVectorHUD* iHUDObject );
        void RenderHUD( UOdysseyVectorScene& iScene );
        void ClearHUD();
        ::ULIS::FRectD GenerateMask( std::vector<::ULIS::FVec2D>& iPointArray );
        void UseMaskImage();
        void UseColorImage(); // TODO : rename UseDefaultImage
        void GetColorImagePixelValue( uint32 iX, uint32 iY, uint8 *oR, uint8* oG, uint8 *oB, uint8 *oA );
        ::ULIS::FColor GetColorImagePixelValue( uint32 iX, uint32 iY );
        void SetSelectionSpace( UOdysseyVectorGroup* iSelectionSpace );
        UOdysseyVectorGroup* GetSelectionSpace();

    private:
        ::ULIS::FRectD mRoi;
        BLContext* mBLContext;
        BLImage* mBLImage;
        BLImage* mBLMask;
        uint64 mDrawingFlags;
        std::list<FOdysseyVectorHUD*> mHUDList;
        UOdysseyVectorGroup* mSelectionSpace;

};

ODYSSEYVECTOR_API FArchive& operator<<(FArchive &Ar, FOdysseyVectorEngine* iVectorEngine );
