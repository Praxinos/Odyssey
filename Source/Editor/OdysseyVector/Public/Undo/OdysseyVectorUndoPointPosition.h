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

struct FPointPosition
{
    FOdysseyVectorPoint* point;
    ::ULIS::FVec2D position;
    double radius;

    FPointPosition( FOdysseyVectorPoint* iPoint )
    {
        point = iPoint;
        position = iPoint->GetCoords();
        radius = iPoint->GetRadius();
    }

    FPointPosition( FOdysseyVectorPoint* iPoint, double iX, double iY, double iRadius )
    {
        point = iPoint;
        position.x = iX;
        position.y = iY;
        radius = iRadius;
    }
};

class ODYSSEYVECTOR_API FOdysseyVectorUndoPointPosition : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoPointPosition();
        FOdysseyVectorUndoPointPosition( FOdysseyVectorScene* iScene, std::vector<FOdysseyVectorPoint*>& iPointArray );
        FOdysseyVectorUndoPointPosition( FOdysseyVectorScene* iScene
                                       , FOdysseyVectorPoint* iPoint
                                       , double iX
                                       , double iY
                                       , double iRadius );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FPointPosition> mPointPositionArray;
};
