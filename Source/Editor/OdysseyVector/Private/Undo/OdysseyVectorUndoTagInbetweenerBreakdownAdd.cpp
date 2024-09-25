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
    : FOdysseyVectorUndo( iScene, iReturnFlags )
{
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

    mEngineArray.reserve( iEngineList.size() );

    for( FOdysseyVectorEngine* tagEngine : iEngineList )
    {
        mEngineArray.push_back( tagEngine );
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

    for( FOdysseyVectorEngine* tagEngine : mEngineArray )
    {
        // request redraw attached cells
        tagEngine->Invalidate( 0 );
    }

    mScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
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

    // update invalidated objects
    for( FOdysseyVectorEngine* tagEngine : mEngineArray )
    {
        // request redraw attached cells
        tagEngine->Invalidate( 0 );
    }

    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerBreakdownAdd::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerBreakdownAdd");
}
