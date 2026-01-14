// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoTagInbetweenerChartAlter.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoTagInbetweenerChartAlter::~FOdysseyVectorUndoTagInbetweenerChartAlter()
{
    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.Clean( mApplied ? eSnapshotState::Altered : eSnapshotState::Initial );
    }
}

FOdysseyVectorUndoTagInbetweenerChartAlter::FOdysseyVectorUndoTagInbetweenerChartAlter( FOdysseyVectorGroupPaint* iScene
                                                                                      , FOdysseyVectorTagInbetweener* iInbetweenerTag )
    : FOdysseyVectorUndo( iScene->GetLayer() )
{
    mInbetweenerTagSnapshotBuffer.emplace_back( iInbetweenerTag
                                              , FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                                              , FSnapshotFlags::Breakdown::CHART
                                              , 0
                                              , 0 )
                                              .RecordState(  eSnapshotState::Initial );
}

FOdysseyVectorUndoTagInbetweenerChartAlter::FOdysseyVectorUndoTagInbetweenerChartAlter( FOdysseyVectorGroupPaint* iScene
                                                                                      , const std::list<FOdysseyVectorTagInbetweener*>& iInbetweenerTagList )
    : FOdysseyVectorUndo( iScene->GetLayer() )
{
    mInbetweenerTagSnapshotBuffer.reserve( iInbetweenerTagList.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagList )
    {
        mInbetweenerTagSnapshotBuffer.emplace_back( inbetweenerTag
                                                  , FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                                                  , FSnapshotFlags::Breakdown::CHART
                                                  , 0
                                                  , 0 );
    }
}

FOdysseyVectorUndoTagInbetweenerChartAlter::FOdysseyVectorUndoTagInbetweenerChartAlter( FOdysseyVectorLayer* iSharedEnv
                                                                                      , const std::list<FOdysseyVectorTag*>& iTagList )
    : FOdysseyVectorUndo( iSharedEnv )
{
    mInbetweenerTagSnapshotBuffer.reserve( iTagList.size() );

    for( FOdysseyVectorTag* tag : iTagList )
    {
        FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

        mInbetweenerTagSnapshotBuffer.emplace_back( inbetweenerTag
                                                  , FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                                                  , FSnapshotFlags::Breakdown::CHART
                                                  , 0
                                                  , 0 );
    }
}

void
FOdysseyVectorUndoTagInbetweenerChartAlter::Begin()
{
    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.RecordState( eSnapshotState::Initial );
    }
}

void
FOdysseyVectorUndoTagInbetweenerChartAlter::End()
{
    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.RecordState( eSnapshotState::Altered );
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
