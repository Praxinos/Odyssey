// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include <mutex>

class FOdysseyVectorObject;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorLayer;
class FOdysseyVectorEngine;
class FOdysseyVectorCell;

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
                         , FOdysseyVectorEngine* iEngine
                         , const ::ULIS::FRectD& iInvalidationArea
                         , double iAncestorsOpacity
                         , uint64 iDrawingFlags ) = 0;
        // when drawn as a shared tag
        virtual void Draw( FOdysseyVectorCell* iCurrentCell
                         , BLContext* iBLContext
                         , FOdysseyVectorEngine* iEngine
                         , const ::ULIS::FRectD& iInvalidationArea
                         , double iAncestorsOpacity
                         , uint64 iDrawingFlags ) = 0;
        virtual void Update( uint32 iUpdateFlags
                           , uint64 iOwnerInvalidationFlags ) = 0;
        virtual void UpdateMatrix() = 0;
        virtual void Share( FOdysseyVectorLayer* iSharedEnv );
        virtual void Unshare( FOdysseyVectorLayer* iSharedEnv );
        virtual void ObjectAdded();
        virtual void ObjectRemoved();
        virtual void Added();
        virtual void Removed();
        virtual FOdysseyVectorTag* Copy( FOdysseyVectorObject* iOwnerObject ) = 0;

        FOdysseyVectorObject* GetOwner( );

    protected:
        FOdysseyVectorObject* mOwner;
        uint32 mFlags;
        bool bShared;
};
