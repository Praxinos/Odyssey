#pragma once

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorHandle.h"

class ODYSSEYVECTOR_API FOdysseyVectorHandlePoint : public FOdysseyVectorHandle
{
    public:
        static uint32 StaticClass() { return mStaticClass; };
        uint32 GetClass() { return mStaticClass; };

        static FOdysseyVectorHandlePoint* New( FOdysseyVectorPoint* iParentPoint );
        ~FOdysseyVectorHandlePoint();
        FOdysseyVectorHandlePoint();
        FOdysseyVectorPoint* GetParent();
        void Init( FOdysseyVectorPoint* iParentPoint );


    private:
        static const uint32 mStaticClass = 0x309598e7; // value is crc32 FOdysseyVectorHandlePoint 

        FOdysseyVectorPoint* mParentPoint;

};
