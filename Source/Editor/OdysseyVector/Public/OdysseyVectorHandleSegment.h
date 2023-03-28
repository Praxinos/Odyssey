#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorHandle.h"

class FOdysseyVectorSegment;

class ODYSSEYVECTOR_API FOdysseyVectorHandleSegment : public FOdysseyVectorHandle
{
    public:
        static uint32 StaticClass() { return mStaticClass; };
        uint32 GetClass() { return mStaticClass; };

        static FOdysseyVectorHandleSegment* New( FOdysseyVectorSegment* iParentSegment, double iX, double iY );
        ~FOdysseyVectorHandleSegment();
        FOdysseyVectorHandleSegment( );

        void Init( FOdysseyVectorSegment* iParentSegment, double iX, double iY );
        FOdysseyVectorSegment* GetParent();
        void SetX( double iX );
        void SetY( double iY );
        void Set( double iX,double iY );

    private:
        static const uint32 mStaticClass = 0x22364e08; // value is crc32 FOdysseyVectorHandleSegment 

        FOdysseyVectorSegment* mParentSegment;
};
