#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"

class FOdysseyVectorGroupPaint;
class FOdysseyVectorTag;
class FOdysseyVectorTagInbetweener;

class ODYSSEYVECTOR_API FOdysseyVectorUndoTagInbetweenerMatching : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoTagInbetweenerMatching();
        FOdysseyVectorUndoTagInbetweenerMatching( FOdysseyVectorGroupPaint* iScene
                                                , FOdysseyVectorTagInbetweener* iInbetweenerTag );
        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        FSnapshotTagInbetweener mInbetweenerTagSnapshot;
};
