#pragma once

#include "CoreMinimal.h"
#include <ULIS>
#include "InbetweenerGrid.h"


class ODYSSEYVECTOR_API FInbetweenerGridFFD : public FInbetweenerGrid
{
    public:
        virtual ~FInbetweenerGridFFD(){};
        FInbetweenerGridFFD( FOdysseyVectorTagInbetweener* iInbetweenerTag
                           , uint32 iNumQuadX
                           , uint32 iNumQuadY );

        virtual void Make( uint32 iNumQuadX
                         , uint32 iNumQuadY
                         , const ::ULIS::FRectD& iBBox  ) override;
        virtual void DeformPaths( std::vector<FInterpolatedPath>& iInterpolatedPathBuffer
                                , uint32 iInbetweenIndex ) override;
        void ComputeBinomialCoefficients();
        ::ULIS::FVec2D DeformPoint( FInterpolatedPoint* iInterpolatedPoint );

        friend class FOdysseyVectorTagInbetweener;

    protected:
        std::vector<double> mUBinomialCoefficientBuffer;
        std::vector<double> mVBinomialCoefficientBuffer;
};
