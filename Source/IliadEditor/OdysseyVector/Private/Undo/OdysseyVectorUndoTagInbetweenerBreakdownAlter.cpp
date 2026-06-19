// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoTagInbetweenerBreakdownAlter.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoTagInbetweenerBreakdownAlter::~FOdysseyVectorUndoTagInbetweenerBreakdownAlter()
{
    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.Clean( mApplied ? eSnapshotState::Altered : eSnapshotState::Initial );
    }
}

FOdysseyVectorUndoTagInbetweenerBreakdownAlter::FOdysseyVectorUndoTagInbetweenerBreakdownAlter( FOdysseyVectorLayer* iSharedEnv
                                                                                              , FOdysseyVectorTagInbetweener* iInbetweenerTag )
    : FOdysseyVectorUndo( iSharedEnv )
{
    mInbetweenerTagSnapshotBuffer.emplace_back(  iInbetweenerTag
                                              , FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                                              , ( FSnapshotFlags::Breakdown::GRIDGEOMETRY
                                                | FSnapshotFlags::Breakdown::CHART )
                                              , ( FSnapshotFlags::Route::TRAJECTORIES
                                                | FSnapshotFlags::Route::STEPS )
                                              , ( FSnapshotFlags::Trajectory::BEZIER
                                                | FSnapshotFlags::Trajectory::WAYPOINTS ) )
                                              .RecordState(  eSnapshotState::Initial );
}

FOdysseyVectorUndoTagInbetweenerBreakdownAlter::FOdysseyVectorUndoTagInbetweenerBreakdownAlter( FOdysseyVectorLayer* iSharedEnv
                                                                                              , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray )
    : FOdysseyVectorUndo( iSharedEnv )
{
    mInbetweenerTagSnapshotBuffer.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotBuffer.emplace_back( inbetweenerTag
                                                 , FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                                                 , ( FSnapshotFlags::Breakdown::GRIDGEOMETRY
                                                   | FSnapshotFlags::Breakdown::CHART )
                                                 , ( FSnapshotFlags::Route::TRAJECTORIES
                                                   | FSnapshotFlags::Route::STEPS )
                                                 , ( FSnapshotFlags::Trajectory::BEZIER
                                                   | FSnapshotFlags::Trajectory::WAYPOINTS ) )
                                                 .RecordState(  eSnapshotState::Initial );
    }
}

FOdysseyVectorUndoTagInbetweenerBreakdownAlter::FOdysseyVectorUndoTagInbetweenerBreakdownAlter( FOdysseyVectorLayer* iSharedEnv
                                                                                              , const std::list<FOdysseyVectorTagInbetweener*>& iInbetweenerTagList )
    : FOdysseyVectorUndo( iSharedEnv )
{
    mInbetweenerTagSnapshotBuffer.reserve( iInbetweenerTagList.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagList )
    {
        mInbetweenerTagSnapshotBuffer.emplace_back( inbetweenerTag
                                                 , FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                                                 , ( FSnapshotFlags::Breakdown::GRIDGEOMETRY
                                                   | FSnapshotFlags::Breakdown::CHART )
                                                 , ( FSnapshotFlags::Route::TRAJECTORIES
                                                   | FSnapshotFlags::Route::STEPS )
                                                 , ( FSnapshotFlags::Trajectory::BEZIER
                                                   | FSnapshotFlags::Trajectory::WAYPOINTS ) )
                                                 .RecordState( eSnapshotState::Initial );
    }
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownAlter::Begin()
{
    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.RecordState( eSnapshotState::Initial );
    }
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownAlter::End()
{
    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.RecordState( eSnapshotState::Altered );
    }
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownAlter::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagsnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagsnapshot.LoadState( eSnapshotState::Altered );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownAlter::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagsnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagsnapshot.LoadState( eSnapshotState::Initial );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerBreakdownAlter::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerBreakdownAlter");
}
