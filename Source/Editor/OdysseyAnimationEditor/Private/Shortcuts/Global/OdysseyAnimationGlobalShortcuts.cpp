// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/Global/OdysseyAnimationGlobalCellsShortcuts.h"
#include "Shortcuts/Global/OdysseyAnimationGlobalTimelineShortcuts.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationGlobalShortcuts::FOdysseyAnimationGlobalShortcuts(TSharedPtr<FOdysseyAnimationEditorExtension> iExtension)
    : mCellsShortcuts(MakeShared<FOdysseyAnimationGlobalCellsShortcuts>(iExtension))
    , mTimelineShortcuts(MakeShared<FOdysseyAnimationGlobalTimelineShortcuts>(iExtension))
{
}

void
FOdysseyAnimationGlobalShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    mCellsShortcuts->MapActionsToCommandList(iCommandList);
    mTimelineShortcuts->MapActionsToCommandList(iCommandList);
}

#undef LOCTEXT_NAMESPACE