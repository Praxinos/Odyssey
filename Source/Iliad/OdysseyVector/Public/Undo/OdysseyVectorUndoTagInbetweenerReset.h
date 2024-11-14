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
                                             , bool iResetGridTransformations
                                             , uint64 iReturnFlags );

        FOdysseyVectorUndoTagInbetweenerReset( FOdysseyVectorGroupPaint* iScene
                                             , const std::list<FOdysseyVectorTagInbetweener*>& iInbetweenerTagList
                                             , bool iResetGridGeometry
                                             , bool iResetGridTransformations
                                             , uint64 iReturnFlags );
        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FSnapshotInbetweenerBreakdown> mBreakdownSnapshotBuffer;
};
