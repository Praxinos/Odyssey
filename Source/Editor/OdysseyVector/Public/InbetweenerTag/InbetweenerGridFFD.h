#pragma once

#include "CoreMinimal.h"
#include <ULIS>
#include "InbetweenerGrid.h"


class ODYSSEYVECTOR_API FInbetweenerGridFFD : public FInbetweenerGrid
{
    public:
        virtual ~FInbetweenerGridFFD(){};
        FInbetweenerGridFFD( FInbetweenerBreakdown* iBreakdown );

        /**
         * @brief Map paths to the grid according to the needs of the grid
         * @param iPathBuffer the paths to map
         */
        void MapInterpolatedPaths( std::vector<FInterpolatedPath>& iPathBuffer );

    protected:
        friend class FOdysseyVectorTagInbetweener;

    protected:
};
