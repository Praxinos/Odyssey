#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorHandle.h"

class ODYSSEYVECTOR_API FOdysseyVectorHandlePoint : public FOdysseyVectorHandle
{
    public:
        static FOdysseyVectorHandlePoint* New( FOdysseyVectorPoint* iParentPoint );
        void Init( FOdysseyVectorPoint* iParentPoint );

    private:
        FOdysseyVectorPoint* mParentPoint;

    protected:

    public:
        ~FOdysseyVectorHandlePoint();
        FOdysseyVectorHandlePoint();
        FOdysseyVectorPoint* GetParent();
};
