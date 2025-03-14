// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

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
        bool IsVisited();
        void SetVisited( bool iVisited );
        uint32 GetNeighbours( FInbetweenerQuad* oNeighbours[4] );
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
        static const uint32 LINKED  = 1L << 0;
        static const uint32 VISITED = 1L << 1;

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
