// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
        FOdysseyVectorUndoGroup( FOdysseyVectorGroupPaint* iScene
                               , FOdysseyVectorGroup* iAddedGroup
                               , std::vector<FOdysseyVectorObject*>& iAddedObjectArray
                               , std::vector<FOdysseyVectorObject*>& iAddedObjectOldParentArray
                               , std::vector<FOdysseyVectorBucket*>& iRemovedBucketArray );

        FOdysseyVectorUndoGroup( FOdysseyVectorGroupPaint* iScene
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
