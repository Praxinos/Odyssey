#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

class FOdysseyVectorPath;
class FOdysseyVectorLoop;
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

        void AttachLoop( FOdysseyVectorLoop* iLoop );
        FOdysseyVectorLoop* GetLoop();


        ::ULIS::FVec2D& GetCoords();
        ::ULIS::FVec2D& GetCoordsOnSegment( FOdysseyVectorSegment* iSegment );

    protected:
        uint64 mIntersectionID;
        // map for intersection positions
        std::map<FOdysseyVectorSegment*, FIntersection> mTMap;
        FOdysseyVectorLoop* mLoop;

};
