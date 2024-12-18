// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndoPathAlter.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoPathStitch : public FOdysseyVectorUndoPathAlter
{
    public:
        ~FOdysseyVectorUndoPathStitch();
        FOdysseyVectorUndoPathStitch( FOdysseyVectorGroupPaint* iScene
                                    , std::vector<FOdysseyVectorPath*>& iRemovedPathArray
                                    , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                                    , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
                                    , std::vector<FOdysseyVectorPath*>& iAddedPathArray
                                    , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                                    , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                                    , std::vector<FOdysseyVectorVertex*>& iMergedVertexArray
                                    , std::vector<FOdysseyVectorSegment*>& iMergedSegmentArray
                                    , uint64 iReturnFlags );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        //FOdysseyVectorPath* mPath;
        //FOdysseyVectorPath* mMergedPath;
        std::vector<FOdysseyVectorVertex*> mMergedVertexArray;
        std::vector<FOdysseyVectorSegment*> mMergedSegmentArray;
};
