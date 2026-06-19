// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"

class FOdysseyVectorGroupPaint;
class FOdysseyVectorTag;
class FOdysseyVectorTagInbetweener;

class ODYSSEYVECTOR_API FOdysseyVectorUndoTagInbetweenerReset : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoTagInbetweenerReset();
        FOdysseyVectorUndoTagInbetweenerReset( FOdysseyVectorGroupPaint* iScene
                                             , const std::list<FInbetweenerBreakdown*>& iBreakdownList
                                             , bool iResetGridGeometry
                                             , bool iResetGridTransformations );

        FOdysseyVectorUndoTagInbetweenerReset( FOdysseyVectorGroupPaint* iScene
                                             , const std::list<FOdysseyVectorTagInbetweener*>& iInbetweenerTagList
                                             , bool iResetGridGeometry
                                             , bool iResetGridTransformations );
        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        virtual void Begin() override;
        virtual void End() override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FSnapshotInbetweenerBreakdown> mBreakdownSnapshotBuffer;
};
