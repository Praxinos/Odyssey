#include "Undo/OdysseyVectorUndoTagInbetweenerRouteRemove.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorSharedEnv.h"

FOdysseyVectorUndoTagInbetweenerRouteRemove::~FOdysseyVectorUndoTagInbetweenerRouteRemove()
{
    if( mApplied )
    {
        delete mRoute;
    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoTagInbetweenerRouteRemove::FOdysseyVectorUndoTagInbetweenerRouteRemove( FOdysseyVectorGroupPaint* iScene
                                                                                        , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                                                        , FInbetweenerRoute* iRoute
                                                                                        , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
    , mInbetweenerTag( iInbetweenerTag )
    , mRoute( iRoute )
{
    GetEngineListFromObjectList( { iScene }, mEngineList );
}

void
FOdysseyVectorUndoTagInbetweenerRouteRemove::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mInbetweenerTag->RemoveRoute( mRoute );

    // update invalidated objects
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

void
FOdysseyVectorUndoTagInbetweenerRouteRemove::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mInbetweenerTag->AddRoute( mRoute );

    // update invalidated objects
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerRouteRemove::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerRouteRemove");
}
