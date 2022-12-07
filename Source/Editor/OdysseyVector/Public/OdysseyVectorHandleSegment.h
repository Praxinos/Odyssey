#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorHandle.h"

class FOdysseyVectorHandleSegment : public FOdysseyVectorHandle
{
    private:
        FOdysseyVectorSegment& mParentSegment;

    protected:

    public:
        ~FOdysseyVectorHandleSegment();
        FOdysseyVectorHandleSegment( FOdysseyVectorSegment& iParentSegment, double iX, double iY );
        uint32 GetType();
        FOdysseyVectorSegment& GetParent();
        void SetX( double iX );
        void SetY( double iY );
        void Set( double iX,double iY );
};
