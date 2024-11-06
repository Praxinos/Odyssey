#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorPoint.h"

class FOdysseyVectorSegment;
class FOdysseyVectorVertex;

class ODYSSEYVECTOR_API FOdysseyVectorHandleSegment : public FOdysseyVectorPoint
{
    private:
        static const uint32 mStaticClass = 0x22364e08; // value is crc32 FOdysseyVectorHandleSegment

    public:
        static uint32 StaticClass() { return mStaticClass; };
        uint32 GetClass() { return mStaticClass; };

        static void ArrayToVertexArray( const std::vector<FOdysseyVectorHandleSegment*>& iSegmentHandleArray
                                      , std::vector<FOdysseyVectorVertex*>& oVertexArray );

        /**
         * @brief Destructor
         */
        ~FOdysseyVectorHandleSegment();

        /**
         * @brief Constructor
         * @param iOwnerSegment segment owning this handle
         * @param iX x coordinates.
         * @param iY y coordinates.
         */
        FOdysseyVectorHandleSegment( FOdysseyVectorSegment* iOwnerSegment
                                   , FOdysseyVectorVertex* iAttachedVertex
                                   , double iX
                                   , double iY );

        /**
         * @brief Get segment owning this handle
         * @return a pointer to the segment owning this handle
         */
        FOdysseyVectorSegment* GetOwner();

        uint32 GetID();
        void SetID( uint32 iHandleID );
        FOdysseyVectorVertex* GetAttachedVertex();

    protected:
        virtual void SetCoords( double iX, double iY ) override;

    private:
        FOdysseyVectorSegment* mOwnerSegment;
        FOdysseyVectorVertex* mAttachedVertex;
        uint32 mID;
};
