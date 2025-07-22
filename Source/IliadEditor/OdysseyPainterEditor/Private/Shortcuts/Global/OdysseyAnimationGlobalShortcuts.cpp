// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Shortcuts/Global/OdysseyAnimationGlobalShortcuts.h"

#include "OdysseyPainterEditor.h"
#include "Shortcuts/Global/OdysseyAnimationGlobalCellsShortcuts.h"
#include "Shortcuts/Global/OdysseyAnimationGlobalTimelineToolsShortcuts.h"
#include "Shortcuts/Global/OdysseyAnimationGlobalTimelineHeaderShortcuts.h"

FOdysseyAnimationGlobalShortcuts::FOdysseyAnimationGlobalShortcuts(
    const TAttribute<UOdysseyAnimation*>& iAnimation,
    const TAttribute<int>& iCurrentFrame,
    const FOnTransactCurrentFrame& iOnTransactCurrentFrame
)
    : mAnimation(iAnimation)
{
    Add(MakeShared<FOdysseyAnimationGlobalCellsShortcuts>(iAnimation, iCurrentFrame, iOnTransactCurrentFrame));
    Add(MakeShared<FOdysseyAnimationGlobalTimelineToolsShortcuts>());
}

void
FOdysseyAnimationGlobalShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    FOdysseyEditorShortcuts::MapActionsToCommandList(iCommandList);

    OdysseyAnimationGlobalTimelineHeaderShortcuts::MapActions_SetAnimationBounds(
        iCommandList,
        mAnimation
    );
}
