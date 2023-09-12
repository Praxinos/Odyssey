#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorPath.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoPathExtend : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoPathExtend();
        FOdysseyVectorUndoPathExtend( FOdysseyVectorScene* iScene
                                    , FOdysseyVectorPath* iPath );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

        void RecordSegment( FOdysseyVectorSegment* iNewSegment, FOdysseyVectorVertex* iNewVertex );

    protected:
        FOdysseyVectorPath* mPath;
        std::vector<FOdysseyVectorVertex*> mAddedVertexArray;
        std::vector<FOdysseyVectorSegment*> mAddedSegmentArray;
};
