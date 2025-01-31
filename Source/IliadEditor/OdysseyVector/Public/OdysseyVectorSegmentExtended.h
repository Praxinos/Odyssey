// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorSegmentCubic.h"

class FOdysseyVectorGroupPaint;

class ODYSSEYVECTOR_API FOdysseyVectorSegmentExtended : public FOdysseyVectorSegment
{
    private:
        static const uint32 mStaticClass = 0xe6153e6c; // value is crc32 FOdysseyVectorSegmentExtended

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() override { return mStaticClass; };
        virtual bool HasBaseClass( uint32 iBaseClassID ) override;

    public:
        virtual ~FOdysseyVectorSegmentExtended();

       /**
         * @brief function to allocate a new cubic segment.
         * @param iPath the path this segment belongs to
         * @param iVertex0
         * @param iVertex1
         * @return a pointer to the newly created segment
         */
        FOdysseyVectorSegmentExtended ( FOdysseyVectorObject* iOwner
                                      , FOdysseyVectorVertex* iVertex0
                                      , FOdysseyVectorVertex* iVertex1
                                      , bool iNeedsWidth );

        virtual void DrawStructure( BLContext* iBLContext
                                  , FOdysseyVectorObject* iParentObject
                                  , bool iWorld );

    private:

};
