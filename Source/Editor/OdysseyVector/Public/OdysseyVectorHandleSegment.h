#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorHandle.h"

class ODYSSEYVECTOR_API FOdysseyVectorHandleSegment : public FOdysseyVectorHandle
{
    public:
        static FOdysseyVectorHandleSegment* New( FOdysseyVectorSegment* iParentSegment, double iX, double iY );
        ~FOdysseyVectorHandleSegment();
        FOdysseyVectorHandleSegment( );

        void Init( FOdysseyVectorSegment* iParentSegment, double iX, double iY );
        FOdysseyVectorSegment* GetParent();
        void SetX( double iX );
        void SetY( double iY );
        void Set( double iX,double iY );

    private:
        FOdysseyVectorSegment* mParentSegment;
};
