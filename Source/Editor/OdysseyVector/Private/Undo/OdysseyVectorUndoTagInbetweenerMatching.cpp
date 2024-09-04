#include "Undo/OdysseyVectorUndoTagInbetweenerMatching.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"

FOdysseyVectorUndoTagInbetweenerMatching::~FOdysseyVectorUndoTagInbetweenerMatching()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {

    }
}

FOdysseyVectorUndoTagInbetweenerMatching::FOdysseyVectorUndoTagInbetweenerMatching( FOdysseyVectorGroupPaint* iScene
                                                                                  , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                                                  , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene, iReturnFlags )
{
    mInbetweenerTagSnapshotBuffer.emplace_back( iInbetweenerTag
                                              , 0 
                                              , FSnapshotFlags::Breakdown::GRIDGEOMETRY
                                              , 0  );
}

FOdysseyVectorUndoTagInbetweenerMatching::FOdysseyVectorUndoTagInbetweenerMatching( FOdysseyVectorGroupPaint* iScene
                                                                                  , const std::list<FOdysseyVectorTagInbetweener*>& iInbetweenerTagList
                                                                                  , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene, iReturnFlags )
{
    mInbetweenerTagSnapshotBuffer.reserve( iInbetweenerTagList.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagList )
    {
        mInbetweenerTagSnapshotBuffer.emplace_back( inbetweenerTag
                                                  , 0 
                                                  , FSnapshotFlags::Breakdown::GRIDGEOMETRY
                                                  , 0  );
    }
}

void
FOdysseyVectorUndoTagInbetweenerMatching::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.Preswap();
        inbetweenerTagSnapshot.Restore();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW | mReturnFlags );
}

void
FOdysseyVectorUndoTagInbetweenerMatching::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.Preswap();
        inbetweenerTagSnapshot.Restore();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW | mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerMatching::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerMatching");
}
