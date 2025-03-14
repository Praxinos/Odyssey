// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegment.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoVertexLock : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoVertexLock();
        FOdysseyVectorUndoVertexLock( FOdysseyVectorGroupPaint* iScene
                                    , const std::vector<FOdysseyVectorVertex*>& iVertexArray
                                    , uint64 iReturnFlags );
        FOdysseyVectorUndoVertexLock( FOdysseyVectorGroupPaint* iScene
                                    , FOdysseyVectorVertex* iVertex
                                    , uint64 iReturnFlags );
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
