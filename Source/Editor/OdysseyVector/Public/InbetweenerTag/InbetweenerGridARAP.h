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
                          , const ::ULIS::FRectD& iBBox ) override;
        double RegularizeQuads( eInbetweenerPointPositionType iPositionType );
        void RegularizeQuad( FInbetweenerQuad* iQuad
                           , eInbetweenerPointPositionType iPositionType );
        uint32 Regularize( eInbetweenerPointPositionType iSourcePositionType
                         , eInbetweenerPointPositionType iDestPositionType
                         , int maxIterations
                         , bool allGrid
                         , bool convergenceStop );
        virtual void Update() override;
        uint32 GetRigidity();
        void SetRigidity( uint32 iRigidity );

        friend class FOdysseyVectorTagInbetweener;

    protected:
        bool k_cornersFixed;
        uint32 mRigidity;
};
