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
    : FOdysseyVectorUndo()
    , mScene( iScene )
{
    mPointPositionArray.reserve( iPointArray.size() );

    for( int i = 0; i < iPointArray.size(); i++ )
    {
        mPointPositionArray.push_back( FPointPosition( iPointArray[i] ) );
    }
}

static void
LoadArray( std::vector<FPointPosition>& mPointPositionArray )
{
    for( int i = 0; i < mPointPositionArray.size(); i++ )
    {
        FPointPosition formerPosition = FPointPosition( mPointPositionArray[i].point );

        // Note: virtual function Set() will invalidate segments in needed
        mPointPositionArray[i].point->Set( mPointPositionArray[i].position.x, mPointPositionArray[i].position.y );
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

    mScene->Update( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

void
FOdysseyVectorUndoPointPosition::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    LoadArray( mPointPositionArray );

    mScene->Update( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoPointPosition::ToString() const
{
    return FString("FOdysseyVectorUndoPointPosition");
}
