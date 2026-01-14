// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoTagInbetweenerTransform.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoTagInbetweenerTransform::~FOdysseyVectorUndoTagInbetweenerTransform()
{
    for( FSnapshotInbetweenerBreakdown& breakdownSnapshot : mBreakdownSnapshotBuffer )
    {
        breakdownSnapshot.Clean( mApplied ? eSnapshotState::Altered : eSnapshotState::Initial );
    }
}

FOdysseyVectorUndoTagInbetweenerTransform::FOdysseyVectorUndoTagInbetweenerTransform( FOdysseyVectorGroupPaint* iScene
                                                                                    , const std::list<FInbetweenerBreakdown*>& iBreakdownList )
    : FOdysseyVectorUndo( iScene->GetLayer() )
{
    mBreakdownSnapshotBuffer.reserve( iBreakdownList.size() );

    for( FInbetweenerBreakdown* breakdown : iBreakdownList )
    {
        mBreakdownSnapshotBuffer.emplace_back( breakdown
                                             , FSnapshotFlags::Breakdown::TRANSFORMATIONS );
    }
}

void
FOdysseyVectorUndoTagInbetweenerTransform::Begin()
{
    for( FSnapshotInbetweenerBreakdown& breakdownSnapshot : mBreakdownSnapshotBuffer )
    {
        breakdownSnapshot.RecordState( eSnapshotState::Initial );
    }
}

void
FOdysseyVectorUndoTagInbetweenerTransform::End()
{
    for( FSnapshotInbetweenerBreakdown& breakdownSnapshot : mBreakdownSnapshotBuffer )
    {
        breakdownSnapshot.RecordState( eSnapshotState::Altered );
    }
}

void
FOdysseyVectorUndoTagInbetweenerTransform::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotInbetweenerBreakdown& breakdownSnapshot : mBreakdownSnapshotBuffer )
    {
        breakdownSnapshot.LoadState( eSnapshotState::Altered );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoTagInbetweenerTransform::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotInbetweenerBreakdown& breakdownSnapshot : mBreakdownSnapshotBuffer )
    {
        breakdownSnapshot.LoadState( eSnapshotState::Initial );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerTransform::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerTransform");
}
