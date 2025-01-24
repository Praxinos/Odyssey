// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Undo/OdysseyVectorUndoTagInbetweenerRouteAdd.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

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
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
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

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoTagInbetweenerRouteAdd::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mInbetweenerTag->RemoveRoute( mRoute );

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerRouteAdd::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerRouteAdd");
}
