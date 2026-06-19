// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include <ULIS>

#include "InbetweenerGrid.h"


class ODYSSEYVECTOR_API FInbetweenerGridARAP : public FInbetweenerGrid
{
    public:
        virtual ~FInbetweenerGridARAP(){};
        FInbetweenerGridARAP( FInbetweenerBreakdown* iBreakdown );

        void Regularize( uint32 iRigidity );

        /**
         * @brief Map paths to the grid according to the needs of the grid
         * @param iPathBuffer the paths to map
         */
        virtual void MapInterpolatedObjects() override;

        void MapInterpolatedPoints( FInterpolatedObject* iInterplatedObject );

        friend class FOdysseyVectorTagInbetweener;

    // ARAP deformation (do not confuse with ARAP interpolation)
    protected:
        bool IsContiguous( uint32 iUsedQuadCount );
        uint32 Expand( uint32 iUsedQuadCount );
        void DiscardEmptyQuads( std::vector<FInterpolatedPath>& iPathBuffer );
        uint32 Regularize( eInbetweenerPointPositionType iSourcePositionType
                         , eInbetweenerPointPositionType iDestPositionType
                         , int maxIterations
                         , bool allGrid
                         , bool convergenceStop );
        double RegularizeQuads( eInbetweenerPointPositionType iPositionType );
        void RegularizeQuad( FInbetweenerQuad* iQuad
                           , eInbetweenerPointPositionType iPositionType );
        void IntersectNeededQuads( const ::ULIS::FRectD& iSourceBBox
                                 , double iXMin
                                 , double iYMin
                                 , double iXMax
                                 , double iYMax );

    protected:

};
