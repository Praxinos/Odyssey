// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoTagInbetweenerBreakdownRemove.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoTagInbetweenerBreakdownRemove::~FOdysseyVectorUndoTagInbetweenerBreakdownRemove()
{
    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.Clean( mApplied ? eSnapshotState::Altered : eSnapshotState::Initial );
    }
}

FOdysseyVectorUndoTagInbetweenerBreakdownRemove::FOdysseyVectorUndoTagInbetweenerBreakdownRemove( FOdysseyVectorLayer* iSharedEnv
                                                                                                , FOdysseyVectorTagInbetweener* iInbetweenerTag )
    : FOdysseyVectorUndoTagInbetweenerBreakdownRemove( iSharedEnv
                                                     , std::list<FOdysseyVectorTagInbetweener*> ({ iInbetweenerTag }) )
{

}

FOdysseyVectorUndoTagInbetweenerBreakdownRemove::FOdysseyVectorUndoTagInbetweenerBreakdownRemove( FOdysseyVectorLayer* iSharedEnv
                                                                                                , const std::list<FOdysseyVectorTagInbetweener*>& iInbetweenerTagList )
    : FOdysseyVectorUndo( iSharedEnv )
{
    mInbetweenerTagSnapshotBuffer.reserve( iInbetweenerTagList.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagList )
    {
        mInbetweenerTagSnapshotBuffer.emplace_back( inbetweenerTag
                                                  , FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                                                  , ( FSnapshotFlags::Breakdown::TRANSFORMATIONS
                                                    | FSnapshotFlags::Breakdown::CHART )
                                                  , ( FSnapshotFlags::Route::TRAJECTORIES
                                                    | FSnapshotFlags::Route::STEPS )
                                                  , ( FSnapshotFlags::Trajectory::BEZIER
                                                    | FSnapshotFlags::Trajectory::WAYPOINTS ) );
    }
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownRemove::Begin()
{
    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.RecordState( eSnapshotState::Initial );
    }
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownRemove::End()
{
    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.RecordState( eSnapshotState::Altered );
    }
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownRemove::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.LoadState( eSnapshotState::Altered );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownRemove::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.LoadState( eSnapshotState::Initial );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerBreakdownRemove::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerBreakdownRemove");
}
