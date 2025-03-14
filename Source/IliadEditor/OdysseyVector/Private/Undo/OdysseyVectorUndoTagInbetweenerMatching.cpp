// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Undo/OdysseyVectorUndoTagInbetweenerMatching.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoTagInbetweenerMatching::~FOdysseyVectorUndoTagInbetweenerMatching()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {

    }
}

FOdysseyVectorUndoTagInbetweenerMatching::FOdysseyVectorUndoTagInbetweenerMatching( FOdysseyVectorGroupPaint* iScene
                                                                                  , const std::list<FInbetweenerBreakdown*>& iBreakdownList
                                                                                  , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    mBreakdownSnapshotBuffer.reserve( iBreakdownList.size() );

    for( FInbetweenerBreakdown* breakdown : iBreakdownList )
    {
        mBreakdownSnapshotBuffer.emplace_back( breakdown, FSnapshotFlags::Breakdown::GRIDGEOMETRY );
    }
}

FOdysseyVectorUndoTagInbetweenerMatching::FOdysseyVectorUndoTagInbetweenerMatching( FOdysseyVectorGroupPaint* iScene
                                                                                  , const std::list<FOdysseyVectorTagInbetweener*>& iInbetweenerTagList
                                                                                  , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    uint32 breakdownCount = 0;

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagList )
    {
        breakdownCount += inbetweenerTag->GetBreakdownCount();
    }

    mBreakdownSnapshotBuffer.reserve( breakdownCount );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagList )
    {
        for( FInbetweenerBreakdown* breakdown : inbetweenerTag->GetBreakdownList() )
        {
            mBreakdownSnapshotBuffer.emplace_back( breakdown, FSnapshotFlags::Breakdown::GRIDGEOMETRY );
        }
    }
}

void
FOdysseyVectorUndoTagInbetweenerMatching::Apply( UObject* iIgnored )
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
FOdysseyVectorUndoTagInbetweenerMatching::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotInbetweenerBreakdown& breakdownSnapshot : mBreakdownSnapshotBuffer )
    {
        breakdownSnapshot.RecordAlteredState();
        breakdownSnapshot.LoadInitialState();
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerMatching::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerMatching");
}
