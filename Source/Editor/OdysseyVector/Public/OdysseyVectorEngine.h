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
        void RenderSelected( FOdysseyVectorScene& iScene );

        void Render( FOdysseyVectorScene& iScene );
        void Pick( FOdysseyVectorScene& iScene, std::vector<::ULIS::FVec2D>& iPointArray, uint32 iSelectionFlags );
        void PickSegments( FOdysseyVectorScene* iScene
                         , double iX
                         , double iY
                         , double iRadius
                         , std::vector<FOdysseyVectorSegment*>& oPickedSegmentArray
                         , std::vector<double>* oDistance );
        void PickPoints( FOdysseyVectorScene* iScene
                       , double iX
                       , double iY
                       , double iRadius
                       , std::vector<FOdysseyVectorPoint*>& oPickedPointArray
                       , uint64 iPickingFlags );
        bool Knot( FOdysseyVectorVertex* iVertexA
                 , FOdysseyVectorVertex* iVertexB
                 , FOdysseyVectorSegment** oCreatedSegment
                 , FOdysseyVectorSegment** oRemovedSegment
                 , bool iSmooth );
        void UseImage( BLImage* iImage );
        static void RecursivePick( FOdysseyVectorGroup* iSelectionSpace, FOdysseyVectorObject& iObj, std::vector<FOdysseyVectorObject*>& iSelectedObjectArray, ::ULIS::FRectD& iRoi, uint32 iSelectionFlags );
        static void RecursiveErase( FOdysseyVectorObject& iObj
                                  , std::vector<FOdysseyVectorObject*>& iErasedObjectArray
                                  ,::ULIS::FRectD &iRoi
                                  , bool iSelectedOnly );
        void Erase( FOdysseyVectorScene& iScene, ::ULIS::FRectD &iRoi, bool iSelectedOnly );

        /*void Init( double iWidth, double iHeight );*/
        void InvalidateRegion( double x, double y, double w, double h );
        void InvalidateRegion( ::ULIS::FRectD& iRegion );
        void InvalidateRegion( ::ULIS::FRectI& iRegion );
        ::ULIS::FRectD& GetInvalidateRegion();
        void SetDrawingFlags( uint64 iDrawingFlags );
        void AddHUD( FOdysseyVectorHUD* iHUDObject );
        void RemoveHUD( FOdysseyVectorHUD* iHUDObject );
        void RenderHUD( FOdysseyVectorScene& iScene );
        void ClearHUD();
        ::ULIS::FRectD GenerateMask( std::vector<::ULIS::FVec2D>& iPointArray );
        void UseMaskImage();
        void UseColorImage(); // TODO : rename UseDefaultImage
        void GetColorImagePixelValue( uint32 iX, uint32 iY, uint8 *oR, uint8* oG, uint8 *oB, uint8 *oA );
        ::ULIS::FColor GetColorImagePixelValue( uint32 iX, uint32 iY );
        void SetSelectionSpace( FOdysseyVectorGroup* iSelectionSpace );
        FOdysseyVectorGroup* GetSelectionSpace();

    private:
        ::ULIS::FRectD mRoi;
        BLContext* mBLContext;
        BLImage* mBLImage;
        BLImage* mBLMask;
        uint64 mDrawingFlags;
        std::list<FOdysseyVectorHUD*> mHUDList;
        FOdysseyVectorGroup* mSelectionSpace;

};

ODYSSEYVECTOR_API FArchive& operator<<(FArchive &Ar, FOdysseyVectorEngine* iVectorEngine );
