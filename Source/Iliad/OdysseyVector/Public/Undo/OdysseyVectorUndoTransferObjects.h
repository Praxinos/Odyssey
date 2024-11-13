#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroupPaint.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoTransferObjects : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoTransferObjects();
        FOdysseyVectorUndoTransferObjects( FOdysseyVectorGroupPaint* iScene
                                         , FOdysseyVectorObject* iTransferredObject
                                         , uint64 iReturnFlags );
        FOdysseyVectorUndoTransferObjects( FOdysseyVectorGroupPaint* iScene
                                         , const std::list<FOdysseyVectorObject*>& iObjectList
                                         , uint64 iReturnFlags );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FSnapshotObject> mTransferredObjectSnapshotArray;
};
