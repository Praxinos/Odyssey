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
    mEngineList.push_back( iInbetweenerTag->GetOwner()->GetEngine() );

    mInbetweenerTagSnapshotArray.emplace_back(  iInbetweenerTag
                                              , FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                                              , FSnapshotFlags::Breakdown::GRIDGEOMETRY
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
    GetEngineListFromInbetweenerTagArray( iInbetweenerTagArray, mEngineList );

    mInbetweenerTagSnapshotArray.reserve( iInbetweenerTagArray.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagArray )
    {
        mInbetweenerTagSnapshotArray.emplace_back( inbetweenerTag
                                                 , FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                                                 , FSnapshotFlags::Breakdown::GRIDGEOMETRY
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
    GetEngineListFromInbetweenerTagList( iInbetweenerTagList, mEngineList );

    mInbetweenerTagSnapshotArray.reserve( iInbetweenerTagList.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagList )
    {
        mInbetweenerTagSnapshotArray.emplace_back( inbetweenerTag
                                                 , FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                                                 , FSnapshotFlags::Breakdown::GRIDGEOMETRY
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

    // update invalidated objects
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
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

    // update invalidated objects
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerBreakdownAlter::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerBreakdownAlter");
}
