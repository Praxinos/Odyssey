#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"

class FOdysseyVectorGroupPaint;
class FOdysseyVectorTag;

class ODYSSEYVECTOR_API FOdysseyVectorUndoTagInbetweenerCommit : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoTagInbetweenerCommit();
        FOdysseyVectorUndoTagInbetweenerCommit( FOdysseyVectorGroupPaint* iScene
                                              , const std::list<FOdysseyVectorTag*>& iRemovedTagList 
                                              , const std::list<FOdysseyVectorObject*>& iAddedObjectList);

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::list<FOdysseyVectorTag*> mRemovedTagList;
        std::list<FOdysseyVectorObject*> mAddedObjectList;
};
