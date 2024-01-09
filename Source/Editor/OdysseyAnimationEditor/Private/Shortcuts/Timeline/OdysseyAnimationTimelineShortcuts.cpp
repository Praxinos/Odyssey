// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/Timeline/OdysseyAnimationTimelineShortcuts.h"

#include "Shortcuts/Timeline/OdysseyAnimationTimelineCellsShortcuts.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationTimelineShortcuts::FOdysseyAnimationTimelineShortcuts(TSharedPtr<SOdysseyAnimationLayerStackTreeView> iTreeView)
    : mCommandList(MakeShared<FUICommandList>())
    , mTreeView(iTreeView)
    , mCellsShortcuts(MakeShared<FOdysseyAnimationTimelineCellsShortcuts>(iTreeView))
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
}

#undef LOCTEXT_NAMESPACE