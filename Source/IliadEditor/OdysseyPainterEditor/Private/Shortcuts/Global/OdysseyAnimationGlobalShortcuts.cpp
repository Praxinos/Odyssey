// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/Global/OdysseyAnimationGlobalShortcuts.h"

#include "OdysseyPainterEditor.h"
#include "Shortcuts/Global/OdysseyAnimationGlobalCellsShortcuts.h"
#include "Shortcuts/Global/OdysseyAnimationGlobalTimelineShortcuts.h"
#include "Shortcuts/Global/OdysseyAnimationGlobalTimelineToolsShortcuts.h"
#include "Shortcuts/Global/OdysseyAnimationGlobalTimelineHeaderShortcuts.h"

FOdysseyAnimationGlobalShortcuts::FOdysseyAnimationGlobalShortcuts(FOdysseyPainterEditor* iEditor)
    : mEditor(iEditor)
{
    Add(MakeShared<FOdysseyAnimationGlobalCellsShortcuts>(iEditor));
    Add(MakeShared<FOdysseyAnimationGlobalTimelineShortcuts>(iEditor));
    Add(MakeShared<FOdysseyAnimationGlobalTimelineToolsShortcuts>());
}

void
FOdysseyAnimationGlobalShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    FOdysseyEditorShortcuts::MapActionsToCommandList(iCommandList);

    OdysseyAnimationGlobalTimelineHeaderShortcuts::MapActions_SetAnimationBounds(
        iCommandList,
        MakeAttributeLambda(
            [this]() -> UOdysseyAnimation*
            {
                return mEditor->GetAnimation();
            }
        )
    );
}
