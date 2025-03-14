// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Undo/OdysseyVectorUndoTagInbetweenerRouteRemove.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

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
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
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

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoTagInbetweenerRouteRemove::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mInbetweenerTag->AddRoute( mRoute );

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerRouteRemove::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerRouteRemove");
}
