#include "Undo/OdysseyVectorUndoPointPosition.h"

FOdysseyVectorUndoPointPosition::~FOdysseyVectorUndoPointPosition()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        // nothing to do
    }

    mPointPositionArray.clear();
}

FOdysseyVectorUndoPointPosition::FOdysseyVectorUndoPointPosition( FOdysseyVectorScene* iScene
                                                                , std::vector<FOdysseyVectorPoint*>& iPointArray )
    : FOdysseyVectorUndo( iScene )
{
    mPointPositionArray.reserve( iPointArray.size() );

    for( int i = 0; i < iPointArray.size(); i++ )
    {
        mPointPositionArray.push_back( FPointPosition( iPointArray[i] ) );
    }
}

FOdysseyVectorUndoPointPosition::FOdysseyVectorUndoPointPosition( FOdysseyVectorScene* iScene
                                                                , FOdysseyVectorPoint* iPoint
                                                                , double iX
                                                                , double iY
                                                                , double iRadius )
    : FOdysseyVectorUndo( iScene )
{
    mPointPositionArray.push_back( FPointPosition( iPoint, iX, iY, iRadius ) );
}

static void
LoadArray( std::vector<FPointPosition>& mPointPositionArray )
{
    for( int i = 0; i < mPointPositionArray.size(); i++ )
    {
        FPointPosition formerPosition = FPointPosition( mPointPositionArray[i].point );

        // Note: virtual function Set() will invalidate segments in needed
        mPointPositionArray[i].point->Set( mPointPositionArray[i].position.x
                                         , mPointPositionArray[i].position.y
                                         , mPointPositionArray[i].radius  );
        // replace with former value (prepare for the counterpart operation, either Apply or Revert)
        mPointPositionArray[i] = formerPosition;
    }
}

void
FOdysseyVectorUndoPointPosition::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    LoadArray( mPointPositionArray );

    // update invalidated objects
    mScene->Update(0);

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoPointPosition::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    LoadArray( mPointPositionArray );

    // update invalidated objects
    mScene->Update(0);

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoPointPosition::ToString() const
{
    return FString("FOdysseyVectorUndoPointPosition");
}
