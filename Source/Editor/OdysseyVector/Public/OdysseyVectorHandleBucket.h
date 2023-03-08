#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorHandle.h"

class FOdysseyVectorBucket;

class ODYSSEYVECTOR_API FOdysseyVectorHandleBucket : public FOdysseyVectorHandle
{
    public:
        static FOdysseyVectorHandleBucket* New( FOdysseyVectorBucket* iParentBucket );
        void Init( FOdysseyVectorBucket* iParentBucket );

    private:
        FOdysseyVectorBucket* mParentBucket;

    protected:

    public:
        ~FOdysseyVectorHandleBucket();
        FOdysseyVectorHandleBucket();
        FOdysseyVectorBucket* GetParent();
};
