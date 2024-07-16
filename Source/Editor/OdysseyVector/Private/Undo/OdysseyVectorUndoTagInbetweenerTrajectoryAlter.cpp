#include "Undo/OdysseyVectorUndoTagInbetweenerTrajectoryAlter.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"

FOdysseyVectorUndoTagInbetweenerTrajectoryAlter::~FOdysseyVectorUndoTagInbetweenerTrajectoryAlter()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoTagInbetweenerTrajectoryAlter::FOdysseyVectorUndoTagInbetweenerTrajectoryAlter( FOdysseyVectorGroupPaint* iScene
                                                                                                , FInbetweenerTrajectory* iTrajectory )
    : FOdysseyVectorUndo( iScene )
{
    mTrajectorySnapshotBuffer.emplace_back( iTrajectory, FSnapshotFlags::Trajectory::BEZIER );
}

FOdysseyVectorUndoTagInbetweenerTrajectoryAlter::FOdysseyVectorUndoTagInbetweenerTrajectoryAlter( FOdysseyVectorGroupPaint* iScene
                                                                                                , const std::list<FInbetweenerTrajectory*>& iTrajectoryList )
    : FOdysseyVectorUndo( iScene )
{
    for( FInbetweenerTrajectory* trajectory : iTrajectoryList )
    {
        mTrajectorySnapshotBuffer.emplace_back( trajectory, FSnapshotFlags::Trajectory::BEZIER );
    }
}

FOdysseyVectorUndoTagInbetweenerTrajectoryAlter::FOdysseyVectorUndoTagInbetweenerTrajectoryAlter( FOdysseyVectorGroupPaint* iScene
                                                                                                , const std::list<FInbetweenerHandleTrajectory*>& iTrajectoryHandleList )
    : FOdysseyVectorUndo( iScene )
{
    for( FInbetweenerHandleTrajectory* trajectoryHandle : iTrajectoryHandleList )
    {
        if( std::find_if( mTrajectorySnapshotBuffer.begin()
                        , mTrajectorySnapshotBuffer.end()
                        , [trajectoryHandle]( FSnapshotTrajectory& trajectorySnapshot )
                          {
                              if( trajectorySnapshot.GetTrajectory() == trajectoryHandle->GetTrajectory() )
                              {
                                  return true;
                              }

                              return false;
                          } ) == mTrajectorySnapshotBuffer.end() )
        {
            mTrajectorySnapshotBuffer.emplace_back( trajectoryHandle->GetTrajectory(), FSnapshotFlags::Trajectory::BEZIER );
        }
    }
}

void
FOdysseyVectorUndoTagInbetweenerTrajectoryAlter::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotTrajectory& trajectorySnapshot : mTrajectorySnapshotBuffer )
    {
        trajectorySnapshot.Restore();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoTagInbetweenerTrajectoryAlter::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotTrajectory& trajectorySnapshot : mTrajectorySnapshotBuffer )
    {
        trajectorySnapshot.Restore();
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
FOdysseyVectorUndoTagInbetweenerTrajectoryAlter::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerTrajectoryAlter");
}
