#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorScene.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoErase : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoErase();
        FOdysseyVectorUndoErase( FOdysseyVectorScene* iScene
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
};
