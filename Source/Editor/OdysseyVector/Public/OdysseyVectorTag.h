#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

class FOdysseyVectorObject;

class ODYSSEYVECTOR_API FOdysseyVectorTag
{
    public:
        virtual ~FOdysseyVectorTag();
        FOdysseyVectorTag( FOdysseyVectorObject* iOwnerObject );
        virtual void Reset() = 0;
        virtual void Draw( BLContext* iBLContext
                         , const ::ULIS::FRectD& iInvalidationArea
                         , double iAncestorsOpacity
                         , uint64 iDrawingFlags ) = 0;

    protected:
        FOdysseyVectorObject* mOwnerObject;
};
