#include "Undo/OdysseyVectorUndoTagInbetweenerBreakdownRemove.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"

FOdysseyVectorUndoTagInbetweenerBreakdownRemove::~FOdysseyVectorUndoTagInbetweenerBreakdownRemove()
{
    if( mApplied )
    {

    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoTagInbetweenerBreakdownRemove::FOdysseyVectorUndoTagInbetweenerBreakdownRemove( FOdysseyVectorGroupPaint* iScene
                                                                                                , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                                                                , uint64 iReturnFlags )
    : FOdysseyVectorUndoTagInbetweenerBreakdownRemove( iScene
                                                   , { iInbetweenerTag }
                                                   , { iInbetweenerTag->GetOwner()->GetEngine() }
                                                   , iReturnFlags )
{

}

FOdysseyVectorUndoTagInbetweenerBreakdownRemove::FOdysseyVectorUndoTagInbetweenerBreakdownRemove( FOdysseyVectorGroupPaint* iScene
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
                                                  | FSnapshotFlags::Route::STEPS );
    }

    mEngineArray.reserve( iEngineList.size() );

    for( FOdysseyVectorEngine* tagEngine : iEngineList )
    {
        mEngineArray.push_back( tagEngine );
    }
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownRemove::Apply( UObject* iIgnored )
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

    //mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::OnSignalDelegate().Broadcast( nullptr
                                                     , ( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW | mReturnFlags ) );
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownRemove::Revert( UObject* iIgnored )
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

    //mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::OnSignalDelegate().Broadcast( nullptr
                                                     , ( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW | mReturnFlags ) );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerBreakdownRemove::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerBreakdownRemove");
}
