// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineShortcuts.h"

#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineCellsShortcuts.h"
#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineCellImageRasterShortcuts.h"
#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineCellImageStaggerShortcuts.h"
#include "OdysseyAnimationLayerStack.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationTimelineShortcuts::FOdysseyAnimationTimelineShortcuts(
    const TAttribute<UOdysseyAnimation*>& iAnimation,
    const TAttribute<int>& iCurrentFrame,
    const FOnTransactCurrentFrame& iOnTransactCurrentFrame
)
    : mCommandList(MakeShared<FUICommandList>())
    , mCellsShortcuts(MakeShared<FOdysseyAnimationTimelineCellsShortcuts>(iAnimation, iCurrentFrame, iOnTransactCurrentFrame))
    , mCellImageRasterShortcuts(MakeShared<FOdysseyAnimationTimelineCellImageRasterShortcuts>(iAnimation))
    , mCellImageStaggerShortcuts(MakeShared<FOdysseyAnimationTimelineCellImageStaggerShortcuts>(iAnimation))
{
    MapActionsToCommandList();
}

TSharedRef<FUICommandList>
FOdysseyAnimationTimelineShortcuts::GetCommandList() const
{
    return mCommandList;
}

void
FOdysseyAnimationTimelineShortcuts::MapActionsToCommandList()
{
    mCellsShortcuts->MapActionsToCommandList(mCommandList);
    mCellImageRasterShortcuts->MapActionsToCommandList(mCommandList);
    mCellImageStaggerShortcuts->MapActionsToCommandList(mCommandList);
}

#undef LOCTEXT_NAMESPACE
