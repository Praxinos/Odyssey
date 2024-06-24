#include "Undo/OdysseyVectorUndoTagInbetweenerTrajectoryRemove.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"

FOdysseyVectorUndoTagInbetweenerTrajectoryRemove::~FOdysseyVectorUndoTagInbetweenerTrajectoryRemove()
{
    if( mApplied )
    {
        for( FInbetweenerTrajectory* trajectory : mTrajectoryArray )
        {
            delete trajectory;
        }
    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoTagInbetweenerTrajectoryRemove::FOdysseyVectorUndoTagInbetweenerTrajectoryRemove( FOdysseyVectorGroupPaint* iScene
                                                                                                  , FInbetweenerTrajectory* iTrajectory )
    : FOdysseyVectorUndo( iScene )
{
    mTrajectoryArray.push_back( iTrajectory );
}

FOdysseyVectorUndoTagInbetweenerTrajectoryRemove::FOdysseyVectorUndoTagInbetweenerTrajectoryRemove( FOdysseyVectorGroupPaint* iScene
                                                                                                  , const std::list<FInbetweenerTrajectory*>& iTrajectoryList )
    : FOdysseyVectorUndo( iScene )
{
    mTrajectoryArray.reserve( iTrajectoryList.size() );

    for( FInbetweenerTrajectory* trajectory : iTrajectoryList )
    {
        mTrajectoryArray.push_back( trajectory );
    }
}

void
FOdysseyVectorUndoTagInbetweenerTrajectoryRemove::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FInbetweenerTrajectory* trajectory : mTrajectoryArray )
    {
        trajectory->GetGrid()->RemoveTrajectory( trajectory );
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoTagInbetweenerTrajectoryRemove::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FInbetweenerTrajectory* trajectory : mTrajectoryArray )
    {
        trajectory->GetGrid()->AddTrajectory( trajectory );
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
FOdysseyVectorUndoTagInbetweenerTrajectoryRemove::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerTrajectoryRemove");
}
