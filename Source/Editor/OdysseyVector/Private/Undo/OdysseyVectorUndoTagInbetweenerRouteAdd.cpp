#include "Undo/OdysseyVectorUndoTagInbetweenerRouteAdd.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"

FOdysseyVectorUndoTagInbetweenerRouteAdd::~FOdysseyVectorUndoTagInbetweenerRouteAdd()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        delete mRoute;
    }
}

FOdysseyVectorUndoTagInbetweenerRouteAdd::FOdysseyVectorUndoTagInbetweenerRouteAdd( FOdysseyVectorGroupPaint* iScene
                                                                                  , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                                                  , FInbetweenerRoute* iRoute
                                                                                  , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene, iReturnFlags )
    , mInbetweenerTag( iInbetweenerTag )
    , mRoute( iRoute )
{
}

void
FOdysseyVectorUndoTagInbetweenerRouteAdd::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mInbetweenerTag->AddRoute( mRoute );

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Invalidate( 0 );
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
}

void
FOdysseyVectorUndoTagInbetweenerRouteAdd::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mInbetweenerTag->RemoveRoute( mRoute );

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Invalidate( 0 );
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerRouteAdd::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerRouteAdd");
}
