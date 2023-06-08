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
        virtual void SetX( double iX ) override;
        virtual void SetY( double iY ) override;
        virtual void Set( double iX, double iY ) override;
        virtual void Set( double iX, double iY, double iRadius ) override;
        void Set( const ::ULIS::FVec2D& iCoords );

    private:
        static const uint32 mStaticClass = 0x22364e08; // value is crc32 FOdysseyVectorHandleSegment 

        FOdysseyVectorSegment* mParentSegment;
};
