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

UCLASS()
class UOdysseyVectorVertexIntersection : public UOdysseyVectorVertex
{
    struct FIntersection {
        ::ULIS::FVec2D position;
        double t;
    };

    public:
        GENERATED_BODY()

    protected:
        uint64 mIntersectionID;
        // map for intersection positions
        std::map<UOdysseyVectorSegment*, FIntersection> mTMap;

    public:
        ~UOdysseyVectorVertexIntersection();
        UOdysseyVectorVertexIntersection();
        ::ULIS::FVec2D GetPosition( UOdysseyVectorSegment& );
        ::ULIS::FVec2D& GetCoords();
        double GetT( UOdysseyVectorSegment& );
        void Draw( ::ULIS::FRectD &iRoi );
        void AddSegment( UOdysseyVectorSegmentCubic* iSegment, double t );
        // overloaded
        UOdysseyVectorSegment* GetSegment( UOdysseyVectorVertex& iOtherVertex );
};
