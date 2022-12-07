#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

class FOdysseyVectorPath;
class FOdysseyVectorLoop;
class FOdysseyVectorSegment;
class FOdysseyVectorSegmentCubic;

class FOdysseyVectorPointIntersection : public FOdysseyVectorPoint
{
    struct FIntersection {
        ::ULIS::FVec2D position;
        double t;
    };

    protected:
        uint64 mIntersectionID;
        // map for intersection positions
        std::map<FOdysseyVectorSegment*, FIntersection> mTMap;

    public:
        ~FOdysseyVectorPointIntersection();
        FOdysseyVectorPointIntersection();
        uint32 GetType();
        ::ULIS::FVec2D GetPosition( FOdysseyVectorSegment& );
        ::ULIS::FVec2D& GetCoords();
        double GetT( FOdysseyVectorSegment& );
        void Draw( ::ULIS::FRectD &iRoi );
        void AddSegment( FOdysseyVectorSegmentCubic* iSegment, double t );
        // overloaded
        FOdysseyVectorSegment* GetSegment( FOdysseyVectorPoint& iOtherPoint );
};
