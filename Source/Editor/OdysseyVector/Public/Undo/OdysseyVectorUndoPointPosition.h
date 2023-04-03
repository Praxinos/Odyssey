#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorHandle.h"
#include "OdysseyVectorHandleSegment.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorVertexCubic.h"

struct FPointPosition
{
    FOdysseyVectorPoint* point;
    ::ULIS::FVec2D position;

    FPointPosition( FOdysseyVectorPoint* iPoint )
    {
        point = iPoint;
        position = iPoint->GetCoords();

        /*if( iPoint->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {

        }*/
    }
};

class ODYSSEYVECTOR_API FOdysseyVectorUndoPointPosition : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoPointPosition();
        FOdysseyVectorUndoPointPosition( FOdysseyVectorScene* iScene );

        void RecordPositionBefore( std::vector<FOdysseyVectorPoint*>& iPointArray );
        void RecordPositionAfter( std::vector<FOdysseyVectorPoint*>& iPointArray );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FPointPosition> mPointPositionBeforeArray;
        std::vector<FPointPosition> mPointPositionAfterArray;
        FOdysseyVectorScene* mScene;
};
