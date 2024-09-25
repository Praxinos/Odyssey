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
    : FOdysseyVectorUndo( iScene, iReturnFlags )
    , mInbetweenerTag( iInbetweenerTag )
    , mRoute( iRoute )
{
}

void
FOdysseyVectorUndoTagInbetweenerRouteRemove::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mInbetweenerTag->RemoveRoute( mRoute );

    // update invalidated objects
    mScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // request radraw
    mScene->GetEngine()->Invalidate( 0 );
    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
}

void
FOdysseyVectorUndoTagInbetweenerRouteRemove::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mInbetweenerTag->AddRoute( mRoute );

    // update invalidated objects
    mScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // request radraw
    mScene->GetEngine()->Invalidate( 0 );
    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerRouteRemove::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerRouteRemove");
}
