// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"

#include "OdysseyVectorTagInbetweener.h"

class FOdysseyVectorGroupPaint;
class FOdysseyVectorEngine;
class FOdysseyVectorLayer;

class ODYSSEYVECTOR_API FOdysseyVectorUndoTagInbetweenerBreakdownRemove : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoTagInbetweenerBreakdownRemove();
        FOdysseyVectorUndoTagInbetweenerBreakdownRemove( FOdysseyVectorLayer* iSharedEnv
                                                       , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                       , uint64 iReturnFlags );
        FOdysseyVectorUndoTagInbetweenerBreakdownRemove( FOdysseyVectorLayer* iSharedEnv
                                                       , const std::list<FOdysseyVectorTagInbetweener*>& iInbetweenerTagList
                                                       , uint64 iReturnFlags );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FSnapshotTagInbetweener> mInbetweenerTagSnapshotBuffer;
};
