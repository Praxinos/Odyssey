#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorHandle.h"

class FOdysseyVectorBucket;

class ODYSSEYVECTOR_API FOdysseyVectorHandleBucket : public FOdysseyVectorHandle
{
    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() override { return mStaticClass; };

        static FOdysseyVectorHandleBucket* New( FOdysseyVectorBucket* iParentBucket );

        ~FOdysseyVectorHandleBucket();
        FOdysseyVectorHandleBucket( FOdysseyVectorBucket* iParentBucket );
        FOdysseyVectorBucket* GetParent();

        void Init( FOdysseyVectorBucket* iParentBucket );

    private:
        static const uint32 mStaticClass = 0xe5d595c7; // value is crc32 FOdysseyVectorHandleBucket

        FOdysseyVectorBucket* mParentBucket;
};
