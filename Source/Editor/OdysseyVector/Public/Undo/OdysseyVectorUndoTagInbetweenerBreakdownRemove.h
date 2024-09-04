#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"

#include "OdysseyVectorTagInbetweener.h"

class FOdysseyVectorGroupPaint;
class FOdysseyVectorEngine;

class ODYSSEYVECTOR_API FOdysseyVectorUndoTagInbetweenerBreakdownRemove : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoTagInbetweenerBreakdownRemove();
        FOdysseyVectorUndoTagInbetweenerBreakdownRemove( FOdysseyVectorGroupPaint* iScene
                                                       , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                       , uint64 iReturnFlags );
        FOdysseyVectorUndoTagInbetweenerBreakdownRemove( FOdysseyVectorGroupPaint* iScene
                                                       , const std::list<FOdysseyVectorTagInbetweener*>& iInbetweenerTagList
                                                       , const std::list<FOdysseyVectorEngine*>& iEngineList
                                                       , uint64 iReturnFlags );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FSnapshotTagInbetweener> mInbetweenerTagSnapshotBuffer;
        std::vector<FOdysseyVectorEngine*> mEngineArray;
};
