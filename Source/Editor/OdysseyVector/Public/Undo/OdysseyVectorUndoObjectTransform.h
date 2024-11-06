#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"

class FOdysseyVectorObject;
class FOdysseyVectorGroupPaint;

class ODYSSEYVECTOR_API FOdysseyVectorUndoObjectTransform : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoObjectTransform();
        FOdysseyVectorUndoObjectTransform( FOdysseyVectorGroupPaint* iScene
                                         , FOdysseyVectorObject* iObject
                                         , uint64 iReturnFlags );
        FOdysseyVectorUndoObjectTransform( FOdysseyVectorGroupPaint* iScene
                                         , std::list<FOdysseyVectorObject*>& iObjectList
                                         , uint64 iReturnFlags );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FSnapshotObject> mObjectSnapshotArray;
};
