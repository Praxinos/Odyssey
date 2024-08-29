#include "Undo/OdysseyVectorUndoTagInbetweenerTransform.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"

FOdysseyVectorUndoTagInbetweenerTransform::~FOdysseyVectorUndoTagInbetweenerTransform()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {

    }
}

FOdysseyVectorUndoTagInbetweenerTransform::FOdysseyVectorUndoTagInbetweenerTransform( FOdysseyVectorGroupPaint* iScene
                                                                                    , const std::list<FOdysseyVectorTagInbetweener*>& iInbetweenerTagList )
    : FOdysseyVectorUndo( iScene )
{
    mInbetweenerTagArray.reserve( iInbetweenerTagList.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagList )
    {
        mInbetweenerTagArray.emplace_back( inbetweenerTag
                                         , FSnapshotFlags::Tag::Inbetweener::TRANSFORMATIONS
                                         , 0 );
    }
}

void
FOdysseyVectorUndoTagInbetweenerTransform::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotTagInbetweener& snapshotInbetweenerTag : mInbetweenerTagArray )
    {
        snapshotInbetweenerTag.Restore();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoTagInbetweenerTransform::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotTagInbetweener& snapshotInbetweenerTag : mInbetweenerTagArray )
    {
        snapshotInbetweenerTag.Restore();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerTransform::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerTransform");
}
