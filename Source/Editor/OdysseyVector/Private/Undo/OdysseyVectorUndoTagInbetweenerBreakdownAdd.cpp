#include "Undo/OdysseyVectorUndoTagInbetweenerBreakdownAdd.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"

static void
BuildRouteSnapshotBuffer( FOdysseyVectorTagInbetweener* iInbetweenerTag
                        , std::vector<FSnapshotRoute>& oRouteSnapshotBuffer )
{
    // Adding or Removing a breakdown will affect routes. Backup them.
    oRouteSnapshotBuffer.clear();
    oRouteSnapshotBuffer.reserve( iInbetweenerTag->GetRouteList().size() );

    for( FInbetweenerRoute* route : iInbetweenerTag->GetRouteList() )
    {
        oRouteSnapshotBuffer.emplace_back( route, FSnapshotFlags::ALL, FSnapshotFlags::ALL );
    }
}

FOdysseyVectorUndoTagInbetweenerBreakdownAdd::~FOdysseyVectorUndoTagInbetweenerBreakdownAdd()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        delete mBreakdown;
    }
}

FOdysseyVectorUndoTagInbetweenerBreakdownAdd::FOdysseyVectorUndoTagInbetweenerBreakdownAdd( FOdysseyVectorGroupPaint* iScene
                                                                                          , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                                                          , FInbetweenerBreakdown* iBreakdown
                                                                                          , uint32 iDrawingIndex )
    : FOdysseyVectorUndo( iScene )
    , mInbetweenerTag( iInbetweenerTag )
    , mBreakdown( iBreakdown )
    , mDrawingIndex( iDrawingIndex )
{
    // Adding or Removing a breakdown will affect routes. Backup them.
    BuildRouteSnapshotBuffer( mInbetweenerTag, mRouteSnapshotBuffer );
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownAdd::Apply( UObject* iIgnored )
{
    std::vector<FSnapshotRoute> swapRouteSnapshotBuffer;

    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );
    // save current routes for swapping
    BuildRouteSnapshotBuffer( mInbetweenerTag, swapRouteSnapshotBuffer );

    mInbetweenerTag->AddBreakdown( mBreakdown, mDrawingIndex, false );

    // Adding or Removing a breakdown will affect routes. Restore them.
    for( FSnapshotRoute& routeSnapshot : mRouteSnapshotBuffer )
    {
        routeSnapshot.Restore();
    }
    // swap
    mRouteSnapshotBuffer = swapRouteSnapshotBuffer;

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownAdd::Revert( UObject* iIgnored )
{
    std::vector<FSnapshotRoute> swapRouteSnapshotBuffer;

    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );
    // save current routes for swapping
    BuildRouteSnapshotBuffer( mInbetweenerTag, swapRouteSnapshotBuffer );

    mInbetweenerTag->RemoveBreakdown( mBreakdown, false );

    // Adding or Removing a breakdown will affect routes. Restore them.
    for( FSnapshotRoute& routeSnapshot : mRouteSnapshotBuffer )
    {
        routeSnapshot.Restore();
    }
    // swap
    mRouteSnapshotBuffer = swapRouteSnapshotBuffer;

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

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
