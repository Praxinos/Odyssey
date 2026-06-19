// IDDN.FR.001.060015.015.S.X.2019.000.00000
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
                                                      , FOdysseyVectorTagInbetweener* iInbetweenerTag );
        FOdysseyVectorUndoTagInbetweenerBreakdownAlter( FOdysseyVectorLayer* iSharedEnv
                                                      , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray );
        FOdysseyVectorUndoTagInbetweenerBreakdownAlter( FOdysseyVectorLayer* iSharedEnv
                                                      , const std::list<FOdysseyVectorTagInbetweener*>& iInbetweenerTagList );
        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        virtual void Begin() override;
        virtual void End() override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FSnapshotTagInbetweener> mInbetweenerTagSnapshotBuffer;
};
