// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoTagInbetweenerChartAlter.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoTagInbetweenerChartAlter::~FOdysseyVectorUndoTagInbetweenerChartAlter()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoTagInbetweenerChartAlter::FOdysseyVectorUndoTagInbetweenerChartAlter( FOdysseyVectorGroupPaint* iScene
                                                                                      , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                                                      , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    mInbetweenerTagSnapshotBuffer.emplace_back( iInbetweenerTag
                                              , 0
                                              , FSnapshotFlags::Breakdown::CHART
                                              , 0
                                              , 0
                                              , eSnapshotState::Initial );
}

FOdysseyVectorUndoTagInbetweenerChartAlter::FOdysseyVectorUndoTagInbetweenerChartAlter( FOdysseyVectorGroupPaint* iScene
                                                                                      , const std::list<FOdysseyVectorTagInbetweener*>& iInbetweenerTagList
                                                                                      , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
{
    mInbetweenerTagSnapshotBuffer.reserve( iInbetweenerTagList.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagList )
    {
        mInbetweenerTagSnapshotBuffer.emplace_back( inbetweenerTag
                                                  , 0
                                                  , FSnapshotFlags::Breakdown::CHART
                                                  , 0
                                                  , 0
                                                  , eSnapshotState::Initial );
    }
}

FOdysseyVectorUndoTagInbetweenerChartAlter::FOdysseyVectorUndoTagInbetweenerChartAlter( FOdysseyVectorLayer* iSharedEnv
                                                                                      , const std::list<FOdysseyVectorTag*>& iTagList
                                                                                      , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iSharedEnv, iReturnFlags )
{
    mInbetweenerTagSnapshotBuffer.reserve( iTagList.size() );

    for( FOdysseyVectorTag* tag : iTagList )
    {
        FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

        mInbetweenerTagSnapshotBuffer.emplace_back( inbetweenerTag
                                                  , 0
                                                  , FSnapshotFlags::Breakdown::CHART
                                                  , 0
                                                  , 0
                                                  , eSnapshotState::Initial );
    }
}

void
FOdysseyVectorUndoTagInbetweenerChartAlter::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.LoadState( eSnapshotState::Altered );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoTagInbetweenerChartAlter::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.RecordState( eSnapshotState::Altered );
        inbetweenerTagSnapshot.LoadState( eSnapshotState::Initial );
    }

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerChartAlter::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerChartAlter");
}
