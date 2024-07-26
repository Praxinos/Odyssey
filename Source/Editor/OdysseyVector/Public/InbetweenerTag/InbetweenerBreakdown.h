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
                             , int32 iSourceInbetweenIndex
                             , int32 iTargetInbetweenIndex );

        void SetSourceInbetweenIndex( int32 iSourceInbetweenIndex );
        void SetTargetInbetweenIndex( int32 iTargetInbetweenIndex );
        int32 GetSourceInbetweenIndex();
        int32 GetTargetInbetweenIndex();
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
        int32 mSourceInbetweenIndex;
        int32 mTargetInbetweenIndex;
        uint32 mIndex;
};
