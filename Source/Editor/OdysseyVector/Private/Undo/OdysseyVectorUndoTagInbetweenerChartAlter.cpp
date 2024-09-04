#include "Undo/OdysseyVectorUndoTagInbetweenerChartAlter.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"

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
    : FOdysseyVectorUndo( iScene, iReturnFlags )
{
    mInbetweenerTagSnapshotBuffer.emplace_back( iInbetweenerTag
                                              , FSnapshotFlags::Tag::Inbetweener::CHART
                                              , 0
                                              , 0 );
}

FOdysseyVectorUndoTagInbetweenerChartAlter::FOdysseyVectorUndoTagInbetweenerChartAlter( FOdysseyVectorGroupPaint* iScene
                                                                                      , const std::list<FOdysseyVectorTagInbetweener*>& iInbetweenerTagList
                                                                                      , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene, iReturnFlags )
{
    mInbetweenerTagSnapshotBuffer.reserve( iInbetweenerTagList.size() );

    for( FOdysseyVectorTagInbetweener* inbetweenerTag : iInbetweenerTagList )
    {
        mInbetweenerTagSnapshotBuffer.emplace_back( inbetweenerTag
                                                  , FSnapshotFlags::Tag::Inbetweener::CHART
                                                  , 0
                                                  , 0 );
    }
}

void
FOdysseyVectorUndoTagInbetweenerChartAlter::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.Preswap();
        inbetweenerTagSnapshot.Restore();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW | mReturnFlags );
}

void
FOdysseyVectorUndoTagInbetweenerChartAlter::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotTagInbetweener& inbetweenerTagSnapshot : mInbetweenerTagSnapshotBuffer )
    {
        inbetweenerTagSnapshot.Preswap();
        inbetweenerTagSnapshot.Restore();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW | mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerChartAlter::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerChartAlter");
}
