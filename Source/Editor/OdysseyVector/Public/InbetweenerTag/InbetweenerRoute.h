#pragma once

#include "CoreMinimal.h"
#include <ULIS>

#include "InbetweenerTag/InbetweenerTrajectory.h"
#include "InbetweenerTag/InbetweenerWaypoint.h"

class FOdysseyVectorTagInbetweener;

class ODYSSEYVECTOR_API FInbetweenerRoute
{
    public:
        virtual ~FInbetweenerRoute();
        FInbetweenerRoute( FOdysseyVectorTagInbetweener* iInbetweenerTag
                         , uint32 iQuadIndex
                         , double iQuadU
                         , double iQuadV );
        void Init( uint32 iQuadIndex
                 , double iQuadU
                 , double iQuadV );
        uint32 GetQuadIndex();
        double GetQuadU();
        double GetQuadV();
        void Update( uint32 iUpdateFlags
                   , uint64 iOwnerInvalidationFlags
                   , uint64 iTagInvalidationFlags );
        FOdysseyVectorTagInbetweener* GetInbetweenerTag();
        void ResetSpacing();
        std::vector<FInbetweenerTrajectory>& GetTrajectoryBuffer();

    private:
        std::vector<FInbetweenerTrajectory> mTrajectoryBuffer;
        FOdysseyVectorTagInbetweener* mInbetweenerTag;
        uint32 mQuadIndex;
        double mQuadU;
        double mQuadV;
};
