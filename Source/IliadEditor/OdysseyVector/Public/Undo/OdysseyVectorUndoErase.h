// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroupPaint.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoErase : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoErase();
        FOdysseyVectorUndoErase( FOdysseyVectorGroupPaint* iScene
                               , std::vector<FOdysseyVectorObject*>& iAddedObjectArray
                               , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                               , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                               , std::vector<FOdysseyVectorObject*>& iRemovedObjectArray
                               , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                               , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FOdysseyVectorObject*> mAddedObjectArray;
        std::vector<FOdysseyVectorVertex*> mAddedVertexArray;
        std::vector<FOdysseyVectorSegment*> mAddedSegmentArray;
        std::vector<FOdysseyVectorObject*> mRemovedObjectArray;
        std::vector<FOdysseyVectorVertex*> mRemovedVertexArray;
        std::vector<FOdysseyVectorSegment*> mRemovedSegmentArray;
        FSnapshotCell mCellSnapshot;
};
