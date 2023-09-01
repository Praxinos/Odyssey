#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorVertex.h"

struct FVertexRadius
{
    FOdysseyVectorVertex* vertex;
    double radius;

    FVertexRadius( FOdysseyVectorVertex* iVertex )
    {
        vertex = iVertex;
        radius = iVertex->GetRadius();
    }
};

class ODYSSEYVECTOR_API FOdysseyVectorUndoVertexRadius : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoVertexRadius();
        FOdysseyVectorUndoVertexRadius( FOdysseyVectorScene* iScene
                                      , std::vector<FOdysseyVectorPoint*>& iPointArray
                                      , bool iAlterAllAlong );
        FOdysseyVectorUndoVertexRadius( FOdysseyVectorScene* iScene
                                      , FOdysseyVectorVertex* iVertex
                                      , bool iAlterAllAlong );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FVertexRadius> mVertexRadiusArray;
        bool mAlterAllAlong;
};
