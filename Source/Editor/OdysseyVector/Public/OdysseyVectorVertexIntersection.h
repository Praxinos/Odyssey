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
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() override { return mStaticClass; };

        /**
         * @brief Destructor.
         */
        ~FOdysseyVectorVertexIntersection();

        /**
         * @brief Constructor.
         */
        FOdysseyVectorVertexIntersection();

        virtual ::ULIS::FVec2D GetPosition( FOdysseyVectorSegment* iSegment ) override;

        virtual double GetT( FOdysseyVectorSegment* iSegment ) override;

        void AddSegment( FOdysseyVectorSegmentCubic* iSegment, double t );
        // overloaded
        FOdysseyVectorSegment* GetSegment( FOdysseyVectorVertex& iOtherVertex );

        virtual ::ULIS::FVec2D& GetCoords( FOdysseyVectorSegment* iSegment ) override;

    protected:
        uint64 mIntersectionID;
        // map for intersection positions
        std::map<FOdysseyVectorSegment*, FIntersection> mTMap;

    private:
        static const uint32 mStaticClass =  0x29459195; // value is crc32 FOdysseyVectorVertexIntersection
};
