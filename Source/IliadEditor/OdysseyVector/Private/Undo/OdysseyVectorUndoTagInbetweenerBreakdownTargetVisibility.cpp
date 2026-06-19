// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility::~FOdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility()
{
    for( FSnapshotInbetweenerBreakdown& breakdownSnapshot : mBreakdownSnapshotBuffer )
    {
        breakdownSnapshot.Clean( mApplied ? eSnapshotState::Altered : eSnapshotState::Initial );
    }
}

FOdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility::FOdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility( FOdysseyVectorLayer* iSharedEnv
                                                                                                                    , FInbetweenerBreakdown* iBreakdown )
    : FOdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility( iSharedEnv
                                                               , std::list<FInbetweenerBreakdown*> ({ iBreakdown }) )
{

}

FOdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility::FOdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility( FOdysseyVectorLayer* iSharedEnv
                                                                                                                    , const std::list<FInbetweenerBreakdown*>& iBreakdownList )
    : FOdysseyVectorUndo( iSharedEnv )
{
    //mEngineList = iEngineList;

    for( FInbetweenerBreakdown* breakdown : iBreakdownList )
    {
        mBreakdownSnapshotBuffer.emplace_back( breakdown
                                             , FSnapshotFlags::Breakdown::TARGETVISIBILITY )
                                             .RecordState(  eSnapshotState::Initial );
    }
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotInbetweenerBreakdown& breakdownSnapshot : mBreakdownSnapshotBuffer )
    {
        breakdownSnapshot.LoadState( eSnapshotState::Altered );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotInbetweenerBreakdown& breakdownSnapshot : mBreakdownSnapshotBuffer )
    {
        breakdownSnapshot.RecordState( eSnapshotState::Altered ); // will run once
    }


    for( FSnapshotInbetweenerBreakdown& breakdownSnapshot : mBreakdownSnapshotBuffer )
    {
        breakdownSnapshot.LoadState( eSnapshotState::Initial );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility");
}
