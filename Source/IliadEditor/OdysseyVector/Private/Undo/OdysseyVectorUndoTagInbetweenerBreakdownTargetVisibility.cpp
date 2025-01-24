// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

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
        mBreakdownSnapshotBuffer.emplace_back( breakdown, FSnapshotFlags::Breakdown::TARGETVISIBILITY );
    }
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownTargetVisibility::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotInbetweenerBreakdown& breakdownSnapshot : mBreakdownSnapshotBuffer )
    {
        breakdownSnapshot.LoadAlteredState();
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
        breakdownSnapshot.RecordAlteredState(); // will run once
        breakdownSnapshot.LoadInitialState();
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
