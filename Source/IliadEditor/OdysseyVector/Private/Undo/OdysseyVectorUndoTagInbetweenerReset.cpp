// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoTagInbetweenerReset.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoTagInbetweenerReset::~FOdysseyVectorUndoTagInbetweenerReset()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {

    }
}

FOdysseyVectorUndoTagInbetweenerReset::FOdysseyVectorUndoTagInbetweenerReset( FOdysseyVectorGroupPaint* iScene
                                                                            , const std::list<FInbetweenerBreakdown*>& iBreakdownList
                                                                            , bool iResetGridGeometry
                                                                            , bool iResetTransformations
                                                                            , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    mBreakdownSnapshotBuffer.reserve( iBreakdownList.size() );

    for( FInbetweenerBreakdown* breakdown : iBreakdownList )
    {
        mBreakdownSnapshotBuffer.emplace_back( breakdown
                                             , ( iResetGridGeometry    ? FSnapshotFlags::Breakdown::GRIDGEOMETRY    : 0 )
                                             | ( iResetTransformations ? FSnapshotFlags::Breakdown::TRANSFORMATIONS : 0 )
                                             , eSnapshotState::Initial );
    }
}

FOdysseyVectorUndoTagInbetweenerReset::FOdysseyVectorUndoTagInbetweenerReset( FOdysseyVectorGroupPaint* iScene
                                                                            , const std::list<FOdysseyVectorTagInbetweener*>& iInbetweenerTagList
                                                                            , bool iResetGridGeometry
                                                                            , bool iResetTransformations
                                                                            , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    uint32 breakdownCount = 0;

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagList )
    {
        breakdownCount += inbetweenerTag->GetBreakdownCount();
    }

    mBreakdownSnapshotBuffer.reserve( breakdownCount );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagList )
    {
        for( FInbetweenerBreakdown* breakdown : inbetweenerTag->GetBreakdownList() )
        {
            mBreakdownSnapshotBuffer.emplace_back( breakdown
                                                 , ( iResetGridGeometry    ? FSnapshotFlags::Breakdown::GRIDGEOMETRY    : 0 )
                                                 | ( iResetTransformations ? FSnapshotFlags::Breakdown::TRANSFORMATIONS : 0 )
                                                 , eSnapshotState::Initial );
        }
    }
}

void
FOdysseyVectorUndoTagInbetweenerReset::Apply( UObject* iIgnored )
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
FOdysseyVectorUndoTagInbetweenerReset::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotInbetweenerBreakdown& breakdownSnapshot : mBreakdownSnapshotBuffer )
    {
        breakdownSnapshot.RecordState( eSnapshotState::Altered );
        breakdownSnapshot.LoadState( eSnapshotState::Initial );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerReset::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerReset");
}
