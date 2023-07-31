#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorScene.h"
#include "OdysseyVectorGroupPaint.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoBucketParam : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoBucketParam();
         FOdysseyVectorUndoBucketParam( FOdysseyVectorScene* iScene
                                      , FOdysseyVectorBucket* iBucket );
         FOdysseyVectorUndoBucketParam( FOdysseyVectorScene* iScene
                                      , std::vector<FOdysseyVectorBucket*>& iBucketArray );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    protected:
        void Swap();

    private:
        std::vector<FOdysseyVectorBucket> mBucketSaveArray;
        std::vector<FOdysseyVectorBucket*> mBucketArray;
};
