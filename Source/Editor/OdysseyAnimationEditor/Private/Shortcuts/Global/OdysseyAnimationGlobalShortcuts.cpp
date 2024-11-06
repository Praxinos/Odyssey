// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/Global/OdysseyAnimationGlobalShortcuts.h"
#include "Shortcuts/Global/OdysseyAnimationGlobalCellsShortcuts.h"
#include "Shortcuts/Global/OdysseyAnimationGlobalTimelineShortcuts.h"
#include "Shortcuts/Global/OdysseyAnimationGlobalTimelineToolsShortcuts.h"

FOdysseyAnimationGlobalShortcuts::FOdysseyAnimationGlobalShortcuts(TSharedPtr<FOdysseyAnimationEditorExtension> iExtension)
{
    Add(MakeShared<FOdysseyAnimationGlobalCellsShortcuts>(iExtension));
    Add(MakeShared<FOdysseyAnimationGlobalTimelineShortcuts>(iExtension));
    Add(MakeShared<FOdysseyAnimationGlobalTimelineToolsShortcuts>());
}
