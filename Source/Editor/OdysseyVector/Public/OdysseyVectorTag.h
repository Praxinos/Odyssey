#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

class FOdysseyVectorObject;
class FOdysseyVectorGroupPaint;

class ODYSSEYVECTOR_API FOdysseyVectorTag
{
    private:
        static const uint32 mStaticClass = 0xcd382614; // value is crc32 FOdysseyVectorTag

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };

        virtual ~FOdysseyVectorTag();
        FOdysseyVectorTag( FOdysseyVectorObject* iOwnerObject );
        virtual void Draw( BLContext* iBLContext
                         , const ::ULIS::FRectD& iInvalidationArea
                         , double iAncestorsOpacity
                         , uint64 iDrawingFlags ) = 0;
        // when drawn as a shared tag
        virtual void Draw( FOdysseyVectorGroupPaint* iCurrentScene
                         , BLContext* iBLContext
                         , const ::ULIS::FRectD& iInvalidationArea
                         , double iAncestorsOpacity
                         , uint64 iDrawingFlags ) = 0;
        virtual void Update( uint32 iUpdateFlags ) = 0;
        virtual void Added() = 0;
        virtual void Removed() = 0;
        virtual void UpdateMatrix() = 0;

        FOdysseyVectorObject* GetOwner();

    protected:
        FOdysseyVectorObject* mOwner;
};
