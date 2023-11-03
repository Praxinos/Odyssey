#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegment.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoVertexAlign : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoVertexAlign();
        FOdysseyVectorUndoVertexAlign( FOdysseyVectorScene* iScene
                                     , const std::vector<FOdysseyVectorVertex*>& iVertexArray );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    protected:
        bool IsSegmentRecorded( FOdysseyVectorSegment* iSegment );

    private:
        std::vector<FOdysseyVectorVertex*> mAlignedVertexArray;
        std::vector<FSnapshotSegmentCubic> mCubicSegmentSnapshotArray;
};
