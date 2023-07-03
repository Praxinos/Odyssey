#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorPathCubic.h"

class ODYSSEYVECTOR_API FOdysseyVectorPrimitive : public FOdysseyVectorPathCubic
{
    private:
        static const uint32 mStaticClass = 0xf3a57735; // value is crc32 FOdysseyVectorPrimitive

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };

        bool HasBaseClass( uint32 iBaseClassID );

    public:
        virtual ~FOdysseyVectorPrimitive();
        FOdysseyVectorPrimitive( const FString& iName );
        virtual FOdysseyVectorPathCubic* Convert() = 0;
};
