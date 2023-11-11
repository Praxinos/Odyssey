#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegment.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoVertexAlignment : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoVertexAlignment();
        FOdysseyVectorUndoVertexAlignment( FOdysseyVectorScene* iScene
                                         , const std::vector<FOdysseyVectorVertex*>& iVertexArray );
        FOdysseyVectorUndoVertexAlignment( FOdysseyVectorScene* iScene, FOdysseyVectorVertex* iVertex );
        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FSnapshotVertex> mVertexSnapshotArray;
        std::vector<FSnapshotSegmentCubic> mCubicSegmentSnapshotArray;
};
