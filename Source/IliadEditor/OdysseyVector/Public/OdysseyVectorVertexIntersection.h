// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorVertex.h"
#include "OdysseyVectorIntersection.h"

class FOdysseyVectorObject;
class FOdysseyVectorSegment;

class ODYSSEYVECTOR_API FOdysseyVectorVertexIntersection : public FOdysseyVectorVertex
{
    private:
        static const uint32 mStaticClass =  0x29459195; // value is crc32 FOdysseyVectorVertexIntersection
    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() override { return mStaticClass; };

        // small temporary structure will allow us to alloc the intersection vertices in one go.
        // for X-Junction
        struct XRecord
        {
            FOdysseyVectorSegment* segment0;
            FOdysseyVectorSegment* segment1;
            double segment0T;
            double segment1T;
            double x;
            double y;

            XRecord( double iX
                   , double iY
                   , FOdysseyVectorSegment* iSegment0
                   , double iSegment0T
                   , FOdysseyVectorSegment* iSegment1
                   , double iSegment1T );
        };

        // small temporary structure will allow us to alloc the intersection vertices in one go.
        // for T-Junction
        struct TRecord
        {
            FOdysseyVectorSegment* segment;
            double segmentT;
            double x;
            double y;
            FOdysseyVectorVertex* vertex;

            // for T-Junction Gaps
            TRecord( double iX
                   , double iY
                   , FOdysseyVectorSegment* iSegment
                   , double iSegmentT
                   , FOdysseyVectorVertex* iVertex );
        };

        /**
         * @brief Destructor.
         */
        ~FOdysseyVectorVertexIntersection();

        /**
         * @brief Constructor for X-Junction
         */
        FOdysseyVectorVertexIntersection( FOdysseyVectorObject* iOwner
                                        , double iX
                                        , double iY
                                        , FOdysseyVectorSegment* iSegment0
                                        , double iSegment0T
                                        , FOdysseyVectorSegment* iSegment1
                                        , double iSegment1T );

        /**
         * @brief Constructor for T-Junction
         */
         FOdysseyVectorVertexIntersection( FOdysseyVectorObject* iOwner
                                         , double iX
                                         , double iY
                                         , FOdysseyVectorSegment* iSegment
                                         , double iSegmentT
                                         , FOdysseyVectorVertex* iVertex );

        double GetT( FOdysseyVectorSegment* iSegment );
        void Attach();
        void Detach();
        bool SelfIntersects();

    protected:
        FOdysseyVectorIntersection mIntersection[2];
        FOdysseyVectorSegment* mSegment[2];
};
