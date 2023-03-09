#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorHandle.h"

class ODYSSEYVECTOR_API FOdysseyVectorHandlePoint : public FOdysseyVectorHandle
{
    public:
        static FOdysseyVectorHandlePoint* New( FOdysseyVectorPoint* iParentPoint );
        ~FOdysseyVectorHandlePoint();
        FOdysseyVectorHandlePoint();
        FOdysseyVectorPoint* GetParent();
        void Init( FOdysseyVectorPoint* iParentPoint );

    private:
        FOdysseyVectorPoint* mParentPoint;

};
