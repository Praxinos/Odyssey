// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include <ULIS>

#include "InbetweenerTag/InbetweenerTrajectory.h"
#include "InbetweenerTag/InbetweenerStep.h"

class FOdysseyVectorTagInbetweener;
struct FOdysseyVectorObjectInvalidationFlags;

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
                   , const FOdysseyVectorObjectInvalidationFlags& iOwnerInvalidationFlags
                   , uint64 iTagInvalidationFlags );
        FOdysseyVectorTagInbetweener* GetInbetweenerTag();
        void ResizeWaypoints();
        std::vector<FInbetweenerTrajectory>& GetTrajectoryBuffer();
        std::vector<FInbetweenerStep>& GetStepBuffer();
        void Resize();
        void SetInbetweenerTag( FOdysseyVectorTagInbetweener* iInbetweenerTag );
        void Fit( uint32 iFitFrom );
        void Reset();
        bool IsEnabled();
        void Smooth();

    protected:
        void Disable();
        void Enable();

    private:
        std::vector<FInbetweenerStep> mStepBuffer;
        std::vector<FInbetweenerTrajectory> mTrajectoryBuffer;
        FOdysseyVectorTagInbetweener* mInbetweenerTag;
        uint32 mQuadIndex;
        double mQuadU;
        double mQuadV;
        bool bEnabled;
};
