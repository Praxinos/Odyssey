#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndoPathAlter.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoKnot : public FOdysseyVectorUndoPathAlter
{
    public:
        ~FOdysseyVectorUndoKnot();
        FOdysseyVectorUndoKnot( FOdysseyVectorScene* iScene
                              , FOdysseyVectorPath* iPath
                              , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                              , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
                              , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                              , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                              , FOdysseyVectorPath* iMergedPath
                              , std::vector<FOdysseyVectorVertex*>& iMergedVertexArray
                              , std::vector<FOdysseyVectorSegment*>& iMergedSegmentArray );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        FOdysseyVectorPath* mPath;
        FOdysseyVectorPath* mMergedPath;
        std::vector<FOdysseyVectorVertex*> mMergedVertexArray;
        std::vector<FOdysseyVectorSegment*> mMergedSegmentArray;
};
