// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorPoint.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoPointPosition : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoPointPosition();
        FOdysseyVectorUndoPointPosition( FOdysseyVectorGroupPaint* iScene
                                       , std::vector<FOdysseyVectorVertex*>& iVertexArray
                                       , std::vector<FOdysseyVectorHandleSegment*>& iHandleArray );
        FOdysseyVectorUndoPointPosition( FOdysseyVectorGroupPaint* iScene, std::vector<FOdysseyVectorPoint*>& iPointArray );
        FOdysseyVectorUndoPointPosition( FOdysseyVectorGroupPaint* iScene, FOdysseyVectorPoint* iPoint );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FSnapshotPoint> mPointSnapshotArray;
};
