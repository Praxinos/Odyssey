// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/Timeline/OdysseyAnimationTimelineShortcuts.h"

#include "Shortcuts/Timeline/OdysseyAnimationTimelineCellsShortcuts.h"
#include "Shortcuts/Timeline/OdysseyAnimationTimelineCellImageRasterShortcuts.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationTimelineShortcuts::FOdysseyAnimationTimelineShortcuts(UOdysseyLayerStack* iLayerStack, FOdysseyAnimationEditorExtension* iAnimationExtension)
    : mCommandList(MakeShared<FUICommandList>())
    , mCellsShortcuts(MakeShared<FOdysseyAnimationTimelineCellsShortcuts>(iLayerStack, iAnimationExtension))
    , mCellImageRasterShortcuts(MakeShared<FOdysseyAnimationTimelineCellImageRasterShortcuts>(iLayerStack, iAnimationExtension))
	, mCellImageStaggerShortcuts(MakeShared<FOdysseyAnimationTimelineCellImageStaggerShortcuts>(iLayerStack, iAnimationExtension))
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