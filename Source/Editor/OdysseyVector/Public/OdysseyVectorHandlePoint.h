#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorHandle.h"

class ODYSSEYVECTOR_API FOdysseyVectorHandlePoint : public FOdysseyVectorHandle
{
    private:
        FOdysseyVectorPoint& mParentPoint;

    protected:

    public:
        ~FOdysseyVectorHandlePoint();
        FOdysseyVectorHandlePoint( FOdysseyVectorPoint& iParentPoint );
        uint32 GetType();
        FOdysseyVectorPoint& GetParent();
};
