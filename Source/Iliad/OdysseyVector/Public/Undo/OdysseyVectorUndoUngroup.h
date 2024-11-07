#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoUngroup : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoUngroup();
        FOdysseyVectorUndoUngroup( FOdysseyVectorGroupPaint* iScene, FOdysseyVectorGroup* iGroup );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::list<FOdysseyVectorObject*> mUngroupedObjectList;
        FOdysseyVectorGroup* mGroup;
};
