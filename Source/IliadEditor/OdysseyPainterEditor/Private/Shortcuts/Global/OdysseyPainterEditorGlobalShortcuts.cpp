// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Shortcuts/Global/OdysseyPainterEditorGlobalShortcuts.h"

#include "OdysseyPainterEditorCommands.h"
#include "OdysseyPainterEditor.h"
#include "Shortcuts/Global/OdysseyPainterEditorGlobalToolsShortcuts.h"
#include "Shortcuts/Global/OdysseyPainterEditorGlobalLayersShortcuts.h"
#include "Shortcuts/Global/OdysseyPainterEditorGlobalTimelineShortcuts.h"
#include "OdysseyPainterEditorSource.h"
#include "ULISLoaderModule.h"

FOdysseyPainterEditorGlobalShortcuts::FOdysseyPainterEditorGlobalShortcuts(FOdysseyPainterEditor* iEditor)
    : mEditor(iEditor)
{
    Add(MakeShared<FOdysseyPainterEditorGlobalToolsShortcuts>(iEditor));
    Add(MakeShared<FOdysseyPainterEditorGlobalLayersShortcuts>(iEditor));
    Add(MakeShared<FOdysseyPainterEditorGlobalTimelineShortcuts>(iEditor));
}

void FOdysseyPainterEditorGlobalShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    FOdysseyEditorShortcuts::MapActionsToCommandList(iCommandList);
}
