// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Undo/OdysseyVectorUndoTagInbetweenerStepAlign.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoTagInbetweenerStepAlign::~FOdysseyVectorUndoTagInbetweenerStepAlign()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {

    }
}

FOdysseyVectorUndoTagInbetweenerStepAlign::FOdysseyVectorUndoTagInbetweenerStepAlign( FOdysseyVectorGroupPaint* iScene
                                                                                    , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                                                    , FInbetweenerRoute* iRoute
                                                                                    , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
    , mInbetweenerTag( iInbetweenerTag )
    , mRouteSnapshot( iRoute
                    , FSnapshotFlags::Route::TRAJECTORIES | FSnapshotFlags::Route::STEPS
                    , FSnapshotFlags::Trajectory::BEZIER )
{
}

void
FOdysseyVectorUndoTagInbetweenerStepAlign::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mRouteSnapshot.LoadAlteredState();

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoTagInbetweenerStepAlign::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mRouteSnapshot.RecordAlteredState();
    mRouteSnapshot.LoadInitialState();

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerStepAlign::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerStepAlign");
}
