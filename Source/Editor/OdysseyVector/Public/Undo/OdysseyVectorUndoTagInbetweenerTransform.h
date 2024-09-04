#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"

class FOdysseyVectorGroupPaint;
class FOdysseyVectorTag;

class ODYSSEYVECTOR_API FOdysseyVectorUndoTagInbetweenerTransform : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoTagInbetweenerTransform();
        FOdysseyVectorUndoTagInbetweenerTransform( FOdysseyVectorGroupPaint* iScene
                                                 , const std::list<FOdysseyVectorTagInbetweener*>& iInbetweenerTagList
                                                 , uint64 iReturnFlags );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FSnapshotTagInbetweener> mInbetweenerTagArray;
};
