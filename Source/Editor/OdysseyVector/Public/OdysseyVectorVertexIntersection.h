#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

class FOdysseyVectorPath;
class FOdysseyVectorCycle;
class FOdysseyVectorSegment;
class FOdysseyVectorSegmentCubic;

struct FIntersection {
    ::ULIS::FVec2D position;
    double t;
};

class FOdysseyVectorVertexIntersection : public FOdysseyVectorVertex
{
    public:
        /**
         * @brief Destructor.
         */
        ~FOdysseyVectorVertexIntersection();

        /**
         * @brief Constructor.
         */
        FOdysseyVectorVertexIntersection();

        ::ULIS::FVec2D GetPosition( FOdysseyVectorSegment& iSegment );

        double GetT( FOdysseyVectorSegment& );

        void AddSegment( FOdysseyVectorSegmentCubic* iSegment, double t );
        // overloaded
        FOdysseyVectorSegment* GetSegment( FOdysseyVectorVertex& iOtherVertex );

        void AttachLoop( FOdysseyVectorCycle* iLoop );
        FOdysseyVectorCycle* GetLoop();

        virtual ::ULIS::FVec2D& GetCoords( FOdysseyVectorSegment* iSegment ) override;

    protected:
        uint64 mIntersectionID;
        // map for intersection positions
        std::map<FOdysseyVectorSegment*, FIntersection> mTMap;
        FOdysseyVectorCycle* mLoop;

};
