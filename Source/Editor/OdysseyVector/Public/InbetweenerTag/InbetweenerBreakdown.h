#pragma once

#include "CoreMinimal.h"
#include <ULIS>

#include "blend2d.h"


class FInbetweenerGrid;
class FOdysseyVectorTagInbetweener;
enum class eInbetweenerGridType : uint8;

class ODYSSEYVECTOR_API FInbetweenerBreakdown
{
    public:
        virtual ~FInbetweenerBreakdown();
        FInbetweenerBreakdown( FOdysseyVectorTagInbetweener* iInbetweenerTag
                             , FInbetweenerBreakdown* iMasterBreakdown
                             , uint32 iSourceDrawingIndex
                             , uint32 iTargetDrawingIndex );

        void SetSourceDrawingIndex( uint32 iSourceDrawingIndex );
        void SetTargetDrawingIndex( uint32 iTargetDrawingIndex );
        uint32 GetSourceDrawingIndex();
        uint32 GetTargetDrawingIndex();
        FInbetweenerGrid* GetGrid();
        void SetIndex( uint32 iIndex );
        uint32 GetIndex();
        void SetGrid( eInbetweenerGridType iGridType );
        BLMatrix2D& GetSourceLocalMatrix();
        BLMatrix2D& GetTargetLocalMatrix();
        FOdysseyVectorTagInbetweener* GetInbetweenerTag();
        FInbetweenerBreakdown* GetMasterBreakdown();
        void DrawPathsAtTarget( BLContext* iBLContext );

        void SetPrevBreakdown( FInbetweenerBreakdown* iPrevBreakdown );
        void SetNextBreakdown( FInbetweenerBreakdown* iNextBreakdown );

        FInbetweenerBreakdown* GetPrevBreakdown( );
        FInbetweenerBreakdown* GetNextBreakdown( );

    private:
        FInbetweenerBreakdown* mMasterBreakdown;
        FInbetweenerBreakdown* mPrevBreakdown;
        FInbetweenerBreakdown* mNextBreakdown;
        FOdysseyVectorTagInbetweener* mInbetweenerTag;
        FInbetweenerGrid* mGrid;
        uint32 mSourceDrawingIndex;
        uint32 mTargetDrawingIndex;
        uint32 mIndex;
};
