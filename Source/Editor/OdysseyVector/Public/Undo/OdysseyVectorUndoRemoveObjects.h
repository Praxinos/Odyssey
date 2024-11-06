#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroupPaint.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoRemoveObjects : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoRemoveObjects();
        FOdysseyVectorUndoRemoveObjects( FOdysseyVectorGroupPaint* iScene
                                       , const std::vector<FOdysseyVectorObject*>& iRemovedObjectArray
                                       , uint64 iReturnFlags );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FOdysseyVectorObject*> mRemovedObjectArray;
};
