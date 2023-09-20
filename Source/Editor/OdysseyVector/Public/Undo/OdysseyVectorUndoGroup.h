#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoGroup : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoGroup();
        FOdysseyVectorUndoGroup( FOdysseyVectorScene* iScene
                               , FOdysseyVectorGroup* iAddedGroup
                               , std::vector<FOdysseyVectorObject*>& iAddedObjectArray
                               , std::vector<FOdysseyVectorObject*>& iAddedObjectOldParentArray
                               , std::vector<FOdysseyVectorBucket*>& iRemovedBucketArray );

        FOdysseyVectorUndoGroup( FOdysseyVectorScene* iScene
                               , FOdysseyVectorGroup* iAddedGroup
                               , std::vector<FOdysseyVectorObject*>& iAddedObjectArray
                               , std::vector<FOdysseyVectorObject*>& iAddedObjectOldParentArray );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FOdysseyVectorBucket*> mRemovedBucketArray;
        std::vector<FOdysseyVectorObject*> mAddedObjectOldParentArray;
        std::vector<FOdysseyVectorObject*> mAddedObjectArray;
        FOdysseyVectorGroup* mAddedGroup;
};
