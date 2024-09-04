#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"

#include "OdysseyVectorTagInbetweener.h"

class FOdysseyVectorGroupPaint;

class ODYSSEYVECTOR_API FOdysseyVectorUndoTagInbetweenerBreakdownAlter : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoTagInbetweenerBreakdownAlter();
        FOdysseyVectorUndoTagInbetweenerBreakdownAlter( FOdysseyVectorGroupPaint* iScene
                                                      , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                      , uint64 iReturnFlags );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        FSnapshotTagInbetweener mInbetweenerTagSnapshot;
};
