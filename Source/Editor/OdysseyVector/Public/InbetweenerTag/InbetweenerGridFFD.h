#pragma once

#include "CoreMinimal.h"
#include <ULIS>
#include "InbetweenerGrid.h"


class ODYSSEYVECTOR_API FInbetweenerGridFFD : public FInbetweenerGrid
{
    public:
        virtual ~FInbetweenerGridFFD(){};
        FInbetweenerGridFFD( FOdysseyVectorTagInbetweener* iInbetweenerTag );

        virtual ::ULIS::FVec2D DeformPoint( FInterpolatedPoint* iInterpolatedPoint
                                          , const ::ULIS::FRectD& iSourceBBox ) override;
        void MapInterpolatedPaths( std::vector<FInterpolatedPath>& iPathBuffer
                                 , const BLMatrix2D& iSpaceInverseMatrix );

        friend class FOdysseyVectorTagInbetweener;

    protected:
        std::vector<double> mUBinomialCoefficientBuffer;
        std::vector<double> mVBinomialCoefficientBuffer;
};
