// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroupPaint.h"

class FOdysseyVectorCell;
class FOdysseyVectorTagInbetweener;

class ODYSSEYVECTOR_API FOdysseyVectorUndoRemoveObjects : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoRemoveObjects();
        FOdysseyVectorUndoRemoveObjects( FOdysseyVectorGroupPaint* iScene
                                       , const std::vector<FOdysseyVectorObject*>& iRemovedObjectArray );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FSnapshotTagInbetweener> mInbetweenerTagSnapshotBuffer;
        std::vector<FOdysseyVectorObject*> mRemovedObjectArray;
        FOdysseyVectorCell* mCell;
};
