#include "Undo/OdysseyVectorUndoChartAlter.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"

FOdysseyVectorUndoChartAlter::~FOdysseyVectorUndoChartAlter()
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

FOdysseyVectorUndoChartAlter::FOdysseyVectorUndoChartAlter( FOdysseyVectorGroupPaint* iScene
                                                          , FOdysseyVectorTagInbetweener* iInbetweenerTag )
    : FOdysseyVectorUndo( iScene )
    , mInbetweenerTag( iInbetweenerTag )
{
    mChart = iInbetweenerTag->GetChart();
    mCount = iInbetweenerTag->GetInbetweenCount();
}

void
FOdysseyVectorUndoChartAlter::Apply( UObject* iIgnored )
{
    FInbetweenerChart& currentChart = mInbetweenerTag->GetChart();
    uint32 currentCount = mInbetweenerTag->GetInbetweenCount();
    FInbetweenerChart swapChart = currentChart;
    uint32 swapCount = currentCount;

    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mInbetweenerTag->SetInbetweenCount( mCount );
    currentChart = mChart;

    mCount = swapCount;
    mChart = swapChart;

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoChartAlter::Revert( UObject* iIgnored )
{
    FInbetweenerChart& currentChart = mInbetweenerTag->GetChart();
    uint32 currentCount = mInbetweenerTag->GetInbetweenCount();
    FInbetweenerChart swapChart = currentChart;
    uint32 swapCount = currentCount;

    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mInbetweenerTag->SetInbetweenCount( mCount );
    currentChart = mChart;

    mCount = swapCount;
    mChart = swapChart;

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoChartAlter::ToString() const
{
    return FString("FOdysseyVectorUndoChartAlter");
}
