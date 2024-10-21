#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include <mutex>

class FOdysseyVectorObject;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorSharedEnv;

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
        virtual void Update( uint32 iUpdateFlags
                           , uint64 iOwnerInvalidationFlags ) = 0;
        virtual void UpdateMatrix() = 0;
        virtual void Share( FOdysseyVectorSharedEnv* iSharedEnv );
        virtual void Unshare( FOdysseyVectorSharedEnv* iSharedEnv );
        virtual void ObjectAdded();
        virtual void ObjectRemoved();
        virtual void Added();
        virtual void Removed();
        void LockDrawing();
        void UnlockDrawing();

        FOdysseyVectorObject* GetOwner();

    protected:
        FOdysseyVectorObject* mOwner;
        uint32 mFlags;
        bool bShared;

    public:
        // mutex to prevent drawing whil update isn't complete. this is necessary due to the Proxy renderer
        // which runs in a different thread
        std::mutex mDrawingMutex;
};
