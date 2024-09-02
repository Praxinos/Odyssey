#include "Undo/OdysseyVectorUndoTagInbetweenerBreakdownAdd.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"

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
                                                                                                , FOdysseyVectorTagInbetweener* iInbetweenerTag )
    : FOdysseyVectorUndoTagInbetweenerBreakdownAdd( iScene
                                                   , { iInbetweenerTag }
                                                   , { iInbetweenerTag->GetOwner()->GetEngine() } )
{

}

FOdysseyVectorUndoTagInbetweenerBreakdownAdd::FOdysseyVectorUndoTagInbetweenerBreakdownAdd( FOdysseyVectorGroupPaint* iScene
                                                                                          , const std::list<FOdysseyVectorTagInbetweener*>& iInbetweenerTagList
                                                                                          , const std::list<FOdysseyVectorEngine*>& iEngineList )
    : FOdysseyVectorUndo( iScene )
{
    mInbetweenerTagSnapshotBuffer.reserve( iInbetweenerTagList.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagList )
    {
        FOdysseyVectorEngine* inbetweenerTagEngine = inbetweenerTag->GetOwner()->GetEngine();

        mInbetweenerTagSnapshotBuffer.emplace_back( inbetweenerTag
                                                  , FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                                                  , 0
                                                  , FSnapshotFlags::Route::TRAJECTORIES
                                                  | FSnapshotFlags::Route::STEPS );
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
        inbetweenerTagSnapshot.Preswap();
        inbetweenerTagSnapshot.Restore();
    }

    for( FOdysseyVectorEngine* tagEngine : mEngineArray )
    {
        // update invalidated objects
        tagEngine->GetScene()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    }

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownAdd::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.Preswap();
        inbetweenerTagSnapshot.Restore();
    }

    // update invalidated objects
    for( FOdysseyVectorEngine* tagEngine : mEngineArray )
    {
        // update invalidated objects
        tagEngine->GetScene()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    }

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerBreakdownAdd::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerBreakdownAdd");
}
