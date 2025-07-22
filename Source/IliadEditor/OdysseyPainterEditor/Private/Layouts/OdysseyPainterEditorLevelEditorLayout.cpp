// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorLevelEditorLayout.h"

#include "OdysseyPainterEditorAnimationDetailsTab.h"
#include "OdysseyPainterEditorAnimationTimelineTab.h"
#include "OdysseyPainterEditorColorSelectorTab.h"
#include "OdysseyPainterEditorLayerStackTab.h"
#include "OdysseyPainterEditorMeshSelectorTab.h"
#include "OdysseyPainterEditorTextureDetailsTab.h"
#include "OdysseyPainterEditorToolsTab.h"
#include "OdysseyPainterEditorVectorSceneTreeViewTab.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "Framework/Docking/LayoutExtender.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------- Layout

//static
void
FOdysseyPainterEditorLevelEditorLayout::ExtendLevelEditorLayout(FLayoutExtender& Extender)
{
    Extender.ExtendLayout(FTabId(TEXT("PlacementBrowser")), ELayoutExtensionPosition::Below, FTabManager::FTab(FOdysseyPainterEditorLayerStackTab::StaticId(), ETabState::ClosedTab));
    Extender.ExtendLayout(FTabId(TEXT("LevelEditorSceneOutliner")), ELayoutExtensionPosition::After, FTabManager::FTab(FOdysseyPainterEditorVectorSceneTreeViewTab::StaticId(), ETabState::ClosedTab));
    Extender.ExtendLayout(FTabId(TEXT("LevelEditorSceneOutliner")), ELayoutExtensionPosition::Below, FTabManager::FTab(FOdysseyPainterEditorColorSelectorTab::StaticId(), ETabState::ClosedTab));
    Extender.ExtendLayout(FTabId(TEXT("LevelEditorSelectionDetails")), ELayoutExtensionPosition::After, FTabManager::FTab(FOdysseyPainterEditorTextureDetailsTab::StaticId(), ETabState::ClosedTab));
    Extender.ExtendLayout(FTabId(TEXT("LevelEditorSelectionDetails")), ELayoutExtensionPosition::After, FTabManager::FTab(FOdysseyPainterEditorAnimationDetailsTab::StaticId(), ETabState::ClosedTab));
    Extender.ExtendLayout(FTabId(TEXT("Sequencer")), ELayoutExtensionPosition::After, FTabManager::FTab(FOdysseyPainterEditorAnimationTimelineTab::StaticId(), ETabState::ClosedTab));
    Extender.ExtendLayout(FTabId(TEXT("LevelEditorViewport")), ELayoutExtensionPosition::Below, FTabManager::FTab(FOdysseyPainterEditorViewportTab::StaticId(), ETabState::ClosedTab));
}

#undef LOCTEXT_NAMESPACE
