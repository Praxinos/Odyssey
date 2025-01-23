// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Undo/OdysseyVectorUndoTagInbetweenerBreakdownAlter.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorSharedEnv.h"

FOdysseyVectorUndoTagInbetweenerBreakdownAlter::~FOdysseyVectorUndoTagInbetweenerBreakdownAlter()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {

    }
}

FOdysseyVectorUndoTagInbetweenerBreakdownAlter::FOdysseyVectorUndoTagInbetweenerBreakdownAlter( FOdysseyVectorSharedEnv* iSharedEnv
                                                                                              , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                                                              , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iSharedEnv, iReturnFlags )
{
    mInbetweenerTagSnapshotArray.emplace_back(  iInbetweenerTag
                                              , FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                                              , ( FSnapshotFlags::Breakdown::GRIDGEOMETRY
                                                | FSnapshotFlags::Breakdown::CHART )
                                              , ( FSnapshotFlags::Route::TRAJECTORIES
                                                | FSnapshotFlags::Route::STEPS )
                                              , ( FSnapshotFlags::Trajectory::BEZIER
                                                | FSnapshotFlags::Trajectory::WAYPOINTS ) );
}

FOdysseyVectorUndoTagInbetweenerBreakdownAlter::FOdysseyVectorUndoTagInbetweenerBreakdownAlter( FOdysseyVectorSharedEnv* iSharedEnv
                                                                                              , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                                                                              , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iSharedEnv, iReturnFlags )
{
    mInbetweenerTagSnapshotArray.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotArray.emplace_back( inbetweenerTag
                                                 , FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                                                 , ( FSnapshotFlags::Breakdown::GRIDGEOMETRY
                                                   | FSnapshotFlags::Breakdown::CHART )
                                                 , ( FSnapshotFlags::Route::TRAJECTORIES
                                                   | FSnapshotFlags::Route::STEPS )
                                                 , ( FSnapshotFlags::Trajectory::BEZIER
                                                   | FSnapshotFlags::Trajectory::WAYPOINTS ) );
    }
}

FOdysseyVectorUndoTagInbetweenerBreakdownAlter::FOdysseyVectorUndoTagInbetweenerBreakdownAlter( FOdysseyVectorSharedEnv* iSharedEnv
                                                                                              , const std::list<FOdysseyVectorTagInbetweener*>& iInbetweenerTagList
                                                                                              , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iSharedEnv, iReturnFlags )
{
    mInbetweenerTagSnapshotArray.reserve( iInbetweenerTagList.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagList )
    {
        mInbetweenerTagSnapshotArray.emplace_back( inbetweenerTag
                                                 , FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                                                 , ( FSnapshotFlags::Breakdown::GRIDGEOMETRY
                                                   | FSnapshotFlags::Breakdown::CHART )
                                                 , ( FSnapshotFlags::Route::TRAJECTORIES
                                                   | FSnapshotFlags::Route::STEPS )
                                                 , ( FSnapshotFlags::Trajectory::BEZIER
                                                   | FSnapshotFlags::Trajectory::WAYPOINTS ) );
    }
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownAlter::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagsnapshot : mInbetweenerTagSnapshotArray )
    {
        inbetweenerTagsnapshot.LoadAlteredState();
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownAlter::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagsnapshot : mInbetweenerTagSnapshotArray )
    {
        inbetweenerTagsnapshot.RecordAlteredState();
        inbetweenerTagsnapshot.LoadInitialState();
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
