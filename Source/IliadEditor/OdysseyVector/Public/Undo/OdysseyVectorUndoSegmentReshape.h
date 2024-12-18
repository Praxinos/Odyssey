// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
        FOdysseyVectorUndoSegmentReshape( FOdysseyVectorGroupPaint* iScene, uint64 iReturnFlags );
        FOdysseyVectorUndoSegmentReshape( FOdysseyVectorGroupPaint* iScene
                                        , const std::vector<FOdysseyVectorVertex*>& iVertexArray
                                        , uint64 iReturnFlags );
        FOdysseyVectorUndoSegmentReshape( FOdysseyVectorGroupPaint* iScene
                                        , const std::vector<FOdysseyVectorSegment*>& iSegmentArray
                                        , uint64 iReturnFlags );

        void RecordVertex( FOdysseyVectorVertex* iVertex );
        void RecordSegment( const std::vector<FOdysseyVectorSegment*>& iSegmentArray );
        void RecordSegment( FOdysseyVectorSegment* iSegment );
        bool HasSegment( FOdysseyVectorSegment* iSegment );
        bool HasVertex( FOdysseyVectorVertex* iVertex );

        void SwapArray();

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FSnapshotVertex> mVertexSnapshotArray;
        std::vector<FSnapshotSegmentCubic> mCubicSegmentSnapshotArray;
};
