// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
                                       , std::vector<FOdysseyVectorHandleSegment*>& iHandleArray
                                       , uint64 iReturnFlags );
        FOdysseyVectorUndoPointPosition( FOdysseyVectorGroupPaint* iScene
                                       , std::vector<FOdysseyVectorPoint*>& iPointArray
                                       , uint64 iReturnFlags );
        FOdysseyVectorUndoPointPosition( FOdysseyVectorGroupPaint* iScene
                                       , FOdysseyVectorPoint* iPoint
                                       , uint64 iReturnFlags );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FSnapshotPoint> mPointSnapshotArray;
};
