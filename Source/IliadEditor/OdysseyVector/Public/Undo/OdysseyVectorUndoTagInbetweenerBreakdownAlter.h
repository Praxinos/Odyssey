// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
        FOdysseyVectorUndoTagInbetweenerBreakdownAlter( FOdysseyVectorLayer* iSharedEnv
                                                      , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                      , uint64 iReturnFlags );
        FOdysseyVectorUndoTagInbetweenerBreakdownAlter( FOdysseyVectorLayer* iSharedEnv
                                                      , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                                      , uint64 iReturnFlags );
        FOdysseyVectorUndoTagInbetweenerBreakdownAlter( FOdysseyVectorLayer* iSharedEnv
                                                      , const std::list<FOdysseyVectorTagInbetweener*>& iInbetweenerTagList
                                                      , uint64 iReturnFlags );
        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FSnapshotTagInbetweener> mInbetweenerTagSnapshotArray;
};
