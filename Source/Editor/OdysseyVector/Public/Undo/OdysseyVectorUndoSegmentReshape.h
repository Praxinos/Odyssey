#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorSegmentCubic.h"

struct FSegmentShape
{
    FOdysseyVectorSegment* segment;
    ::ULIS::FVec2D point[4];

    FSegmentShape( FOdysseyVectorSegment* iSegment )
    {
        segment = iSegment;

        if( iSegment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(iSegment);

            point[0] = cubicSegment->GetVertex(0)->GetCoords();
            point[1] = cubicSegment->GetVertex(1)->GetCoords();
            point[2] = cubicSegment->GetHandle(0)->GetCoords();
            point[3] = cubicSegment->GetHandle(1)->GetCoords();
        }
    }
};

class ODYSSEYVECTOR_API FOdysseyVectorUndoSegmentReshape : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoSegmentReshape();
        FOdysseyVectorUndoSegmentReshape( FOdysseyVectorScene* iScene );

        void RecordSegment( std::vector<FOdysseyVectorSegment*>& iSegmentArray );
        void RecordSegment( FOdysseyVectorSegment* iSegment );
        bool HasSegment( FOdysseyVectorSegment* iSegment );

        void SwapArray();

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FSegmentShape> mSegmentShapeArray;
};
