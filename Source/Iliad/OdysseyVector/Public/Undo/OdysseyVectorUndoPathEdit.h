#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroupPaint.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoPathEdit : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoPathEdit();
        FOdysseyVectorUndoPathEdit( FOdysseyVectorGroupPaint* iScene
                                  , const std::vector<FOdysseyVectorVertex*>& iEditedVertexArray
                                  , const std::vector<FOdysseyVectorSegment*>& iEditedSegmentArray
                                  , uint64 iReturnFlags );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

        bool HasRecordedVertex( FOdysseyVectorVertex* iVertex );
        bool HasRecordedSegment( FOdysseyVectorSegment* iSegment );

    protected:
        std::vector<FSnapshotVertex> mVertexSnapshotArray;
        std::vector<FSnapshotSegmentCubic> mCubicSegmentSnapshotArray;
};
