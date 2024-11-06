#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoDeleteVertex : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoDeleteVertex();
         FOdysseyVectorUndoDeleteVertex( FOdysseyVectorGroupPaint* iScene
                                       , const std::list<FOdysseyVectorObject*>& iObjectList
                                       , uint64 iReturnFlags );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FSnapshotPath> mPathSnapshotArray;
};
