#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorScene.h"
#include "OdysseyVectorGroupPaint.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoBucketAdd : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoBucketAdd();
        FOdysseyVectorUndoBucketAdd( FOdysseyVectorScene* iScene
                                   , FOdysseyVectorGroupPaint* iPaintGroup
                                   , FOdysseyVectorBucket* iBucket );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        FOdysseyVectorGroupPaint* mPaintGroup;
        FOdysseyVectorBucket* mBucket;
};
