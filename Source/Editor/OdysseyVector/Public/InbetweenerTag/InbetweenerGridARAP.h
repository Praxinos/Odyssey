#pragma once

#include "CoreMinimal.h"

#include <ULIS>

#include "InbetweenerGrid.h"


class ODYSSEYVECTOR_API FInbetweenerGridARAP : public FInbetweenerGrid
{
    public:
        virtual ~FInbetweenerGridARAP(){};
        FInbetweenerGridARAP( FOdysseyVectorTagInbetweener* iInbetweenerTag
                            , uint32 iNumQuadX
                            , uint32 iNumQuadY );

        virtual void Make(  uint32 iNumQuadX
                          , uint32 iNumQuadY
                          , const ::ULIS::FRectD& iBBox
                          , const std::vector<::ULIS::FVec2D>& iSourcePositionBuffer
                          , const std::vector<::ULIS::FVec2D>& iTargetPositionBuffer ) override;

        double RegularizeQuads( eInbetweenerPointPositionType iPositionType );
        void RegularizeQuad( FInbetweenerQuad* iQuad
                           , eInbetweenerPointPositionType iPositionType );
        uint32 Regularize( eInbetweenerPointPositionType iSourcePositionType
                         , eInbetweenerPointPositionType iDestPositionType
                         , int maxIterations
                         , bool allGrid
                         , bool convergenceStop );
        void Regularize();
        virtual void Update( uint32 iUpdateFlags
                           , uint64 iTagInvalidationFlags ) override;
        uint32 GetRigidity();
        void SetRigidity( uint32 iRigidity );
        virtual void MapInterpolatedPaths( std::vector<FInterpolatedPath>& iPathBuffer
                                         , const BLMatrix2D& iSpaceInverseMatrix ) override;
        void DiscardEmptyQuads( std::vector<FInterpolatedPath>& iPathBuffer );

        friend class FOdysseyVectorTagInbetweener;

    protected:
        void IntersectNeededQuads( const ::ULIS::FRectD& iSourceBBox
                                 , double iXMin
                                 , double iYMin
                                 , double iXMax
                                 , double iYMax );

    protected:
        bool k_cornersFixed;
        uint32 mRigidity;
};
