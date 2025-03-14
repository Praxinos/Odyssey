// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorSegmentCubic.h"

class FOdysseyVectorGroupPaint;

class ODYSSEYVECTOR_API FOdysseyVectorSegmentCubicGap : public FOdysseyVectorSegmentCubic
{
    private:
        static const uint32 mStaticClass = 0xe7056bb1; // value is crc32 FOdysseyVectorSegmentCubicGap

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() override { return mStaticClass; };
        virtual bool HasBaseClass( uint32 iBaseClassID ) override;

    public:
        virtual ~FOdysseyVectorSegmentCubicGap();

       /**
         * @brief function to allocate a new cubic segment.
         * @param iPath the path this segment belongs to
         * @param iVertex0
         * @param iVertex1
         * @return a pointer to the newly created segment
         */
        FOdysseyVectorSegmentCubicGap ( FOdysseyVectorGroupPaint* iPaintgroup
                                      , FOdysseyVectorVertex* iVertex0
                                      , FOdysseyVectorVertex* iVertex1 );

       /**
         * @brief Update cached data for this segment.
         */
        virtual void Update( uint32 iUpdateFlags ) override;


    private:
        FOdysseyVectorGroupPaint* mPaintgroup;
};
