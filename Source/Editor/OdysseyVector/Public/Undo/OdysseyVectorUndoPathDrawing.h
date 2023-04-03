#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoPathDrawing : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoPathDrawing();
        FOdysseyVectorUndoPathDrawing( FOdysseyVectorPath* iPath
                                     , std::vector<FOdysseyVectorVertex*>& iVertexArray
                                     , std::vector<FOdysseyVectorSegment*>& iSegmentArray );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        FOdysseyVectorPath* mPath;
        std::vector<FOdysseyVectorVertex*> mVertexArray;
        std::vector<FOdysseyVectorSegment*> mSegmentArray;
};
