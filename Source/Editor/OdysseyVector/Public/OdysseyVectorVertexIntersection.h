#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorVertexIntersection.generated.h"

class FOdysseyVectorPath;
class FOdysseyVectorLoop;
class UOdysseyVectorSegment;
class UOdysseyVectorSegmentCubic;

struct FIntersection {
    ::ULIS::FVec2D position;
    double t;
};

UCLASS()
class UOdysseyVectorVertexIntersection : public UOdysseyVectorVertex
{
    public:
        GENERATED_BODY()

    protected:
        uint64 mIntersectionID;
        // map for intersection positions
        std::map<UOdysseyVectorSegment*, FIntersection> mTMap;
        bool mVisited;

    public:
        ~UOdysseyVectorVertexIntersection();
        UOdysseyVectorVertexIntersection();
        ::ULIS::FVec2D GetPosition( UOdysseyVectorSegment& iSegment );
        ::ULIS::FVec2D& GetCoords();
        double GetT( UOdysseyVectorSegment& );
        void Draw( UOdysseyVectorPath* iPath, ::ULIS::FRectD &iRoi );
        void AddSegment( UOdysseyVectorSegmentCubic* iSegment, double t );
        // overloaded
        UOdysseyVectorSegment* GetSegment( UOdysseyVectorVertex& iOtherVertex );
        void SetVisited( bool iVisited );
        bool IsVisited();
};
