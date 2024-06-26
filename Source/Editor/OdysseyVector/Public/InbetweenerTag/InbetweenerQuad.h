#pragma once

#include "CoreMinimal.h"
#include <ULIS>

#include "InbetweenerTag/InbetweenerPoint.h"

class ODYSSEYVECTOR_API FInbetweenerQuad
{
    public:
        virtual ~FInbetweenerQuad(){};
        FInbetweenerQuad( );

        FInbetweenerPoint** GetPoints();
        void Init( FInbetweenerGrid* iGrid );
        void Link();
        void Unlink();
        bool IsLinked();
        void ComputeCentroids();
        void ComputeCentroid( eInbetweenerPointPositionType iPositionType );
        ::ULIS::FVec2D BiasedCentroid( eInbetweenerPointPositionType iPositionType );
        ::ULIS::FVec2D GetPoint( eInbetweenerPointPositionType iPositionType
                               , double iU
                               , double iV );
        FInbetweenerGrid* GetGrid();
        ::ULIS::FVec2D GetPinPosition();
        double GetPinU();
        double GetPinV();
        bool IsPinned();
        ::ULIS::FRectD GetBBox( eInbetweenerPointPositionType iPositionType );
        double GetSourceArea();
        bool HitTest( double iLocalX, double iLocalY );

        friend class FOdysseyVectorTagInbetweener;

    public:
        static const uint32 LINKED = 1L << 0;

    protected:
        uint32 mFlags;
        FInbetweenerGrid* mGrid;
        FInbetweenerPoint* mPoint[4];
        ::ULIS::FVec2D mCentroid[4];
        ::ULIS::FVec2D mPinPosition;
        bool bPinned;
        double mPinU;
        double mPinV;
};
