// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility::~FOdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility()
{
    if( mApplied )
    {

    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility::FOdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility( FOdysseyVectorLayer* iSharedEnv
                                                                                                                    , FInbetweenerBreakdown* iBreakdown
                                                                                                                    , uint64 iReturnFlags )
    : FOdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility( iSharedEnv
                                                               , std::list<FInbetweenerBreakdown*> ({ iBreakdown })
                                                               , iReturnFlags )
{

}

FOdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility::FOdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility( FOdysseyVectorLayer* iSharedEnv
                                                                                                                    , const std::list<FInbetweenerBreakdown*>& iBreakdownList
                                                                                                                    , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iSharedEnv, iReturnFlags )
{
    //mEngineList = iEngineList;

    for( FInbetweenerBreakdown* breakdown : iBreakdownList )
    {
        mBreakdownSnapshotBuffer.emplace_back( breakdown
                                             , FSnapshotFlags::Breakdown::TARGETVISIBILITY
                                             , eSnapshotState::Initial );
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
