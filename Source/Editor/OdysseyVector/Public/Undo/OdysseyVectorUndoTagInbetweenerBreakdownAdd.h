#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"

#include "OdysseyVectorTagInbetweener.h"

class FOdysseyVectorGroupPaint;

class ODYSSEYVECTOR_API FOdysseyVectorUndoTagInbetweenerBreakdownAdd : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoTagInbetweenerBreakdownAdd();
        FOdysseyVectorUndoTagInbetweenerBreakdownAdd( FOdysseyVectorGroupPaint* iScene
                                                    , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                    , FInbetweenerBreakdown* iBreakdown
                                                    , uint32 iDrawingIndex );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        FOdysseyVectorTagInbetweener* mInbetweenerTag;
        FInbetweenerBreakdown* mBreakdown;
        std::vector<FSnapshotRoute> mRouteSnapshotBuffer;
        uint32 mDrawingIndex;
};
