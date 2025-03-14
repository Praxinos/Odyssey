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

class ODYSSEYVECTOR_API FOdysseyVectorUndoTagInbetweenerRouteAlter : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoTagInbetweenerRouteAlter();
        FOdysseyVectorUndoTagInbetweenerRouteAlter( FOdysseyVectorGroupPaint* iScene
                                                  , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                  , FInbetweenerRoute* iRoute
                                                  , uint64 iReturnFlags );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        FOdysseyVectorTagInbetweener* mInbetweenerTag;
        FSnapshotRoute mRouteSnapshot;
};
