#include "Undo/OdysseyVectorUndoTagInbetweenerBreakdownAdd.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorSharedEnv.h"

FOdysseyVectorUndoTagInbetweenerBreakdownAdd::~FOdysseyVectorUndoTagInbetweenerBreakdownAdd()
{
    if( mApplied )
    {

    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoTagInbetweenerBreakdownAdd::FOdysseyVectorUndoTagInbetweenerBreakdownAdd( FOdysseyVectorGroupPaint* iScene
                                                                                          , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                                                          , uint64 iReturnFlags )
    : FOdysseyVectorUndoTagInbetweenerBreakdownAdd( iScene
                                                   , { iInbetweenerTag }
                                                   , { iInbetweenerTag->GetOwner()->GetEngine() }
                                                   , iReturnFlags )
{

}

FOdysseyVectorUndoTagInbetweenerBreakdownAdd::FOdysseyVectorUndoTagInbetweenerBreakdownAdd( FOdysseyVectorGroupPaint* iScene
                                                                                          , const std::list<FOdysseyVectorTagInbetweener*>& iInbetweenerTagList
                                                                                          , const std::list<FOdysseyVectorEngine*>& iEngineList
                                                                                          , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
{
    mEngineList = iEngineList;

    mInbetweenerTagSnapshotBuffer.reserve( iInbetweenerTagList.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagList )
    {
        FOdysseyVectorEngine* inbetweenerTagEngine = inbetweenerTag->GetOwner()->GetEngine();

        mInbetweenerTagSnapshotBuffer.emplace_back( inbetweenerTag
                                                  , FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                                                  , 0
                                                  , FSnapshotFlags::Route::TRAJECTORIES
                                                  | FSnapshotFlags::Route::STEPS
                                                  , FSnapshotFlags::Trajectory::BEZIER
                                                  | FSnapshotFlags::Trajectory::WAYPOINTS );
    }
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownAdd::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.LoadAlteredState();
    }

    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownAdd::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.RecordAlteredState(); // will run once
        inbetweenerTagSnapshot.LoadInitialState();
    }

    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerBreakdownAdd::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerBreakdownAdd");
}
