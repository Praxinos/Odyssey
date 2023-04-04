#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorScene.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoPathAlter : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoPathAlter();
        FOdysseyVectorUndoPathAlter( FOdysseyVectorScene* iScene
                                   , FOdysseyVectorVertex* iRemovedVertex
                                   , FOdysseyVectorSegment* iRemovedSegment
                                   , FOdysseyVectorVertex* iAddedVertex
                                   , FOdysseyVectorSegment* iAddedSegment );
        FOdysseyVectorUndoPathAlter( FOdysseyVectorScene* iScene
                                   , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                                   , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray );
        FOdysseyVectorUndoPathAlter( FOdysseyVectorScene* iScene
                                   , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                                   , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
                                   , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                                   , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        FOdysseyVectorScene* mScene;
        std::vector<FOdysseyVectorVertex*> mRemovedVertexArray;
        std::vector<FOdysseyVectorSegment*> mRemovedSegmentArray;
        std::vector<FOdysseyVectorVertex*> mAddedVertexArray;
        std::vector<FOdysseyVectorSegment*> mAddedSegmentArray;
};
