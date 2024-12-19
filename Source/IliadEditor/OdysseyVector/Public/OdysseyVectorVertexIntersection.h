// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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

        void SetIntersection( FOdysseyVectorIntersection* iIntersection0
                            , FOdysseyVectorIntersection* iIntersection1 );

        FOdysseyVectorIntersection* GetIntersection( uint32 iIndex );
        double GetT( FOdysseyVectorSegment* iSegment );
        void Attach();
        void Detach();
        bool SelfIntersects();

    protected:
        FOdysseyVectorIntersection mIntersection[2];
        FOdysseyVectorSegment* mSegment[2];
};
