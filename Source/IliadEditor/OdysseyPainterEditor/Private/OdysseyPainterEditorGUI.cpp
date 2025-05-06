// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyPainterEditorGUI.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationLayerImageVector.h"
#include "OdysseyAnimationCellImageVector.h"
#include "OdysseyEditorTab.h"
#include "OdysseyLayerStack.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorAnimationTimelineTab.h"
#include "OdysseyPainterEditorAnimationLighttableTab.h"
#include "OdysseyPainterEditorAnimationDetailsTab.h"
#include "OdysseyPainterEditorColorSelectorTab.h"
#include "OdysseyPainterEditorCommands.h"
#include "OdysseyPainterEditorFlipbookTimelineTab.h"
#include "OdysseyPainterEditorLayerStackTab.h"
#include "OdysseyPainterEditorMeshSelectorTab.h"
#include "OdysseyPainterEditorRasterSelection.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyPainterEditorTextureDetailsTab.h"
#include "OdysseyPainterEditorToolsTab.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "OdysseyPainterEditorVectorSceneTreeViewTab.h"
#include "OdysseyTextureLayerImageVector.h"
#include "OdysseyTextureLayerStack.h"
#include "OdysseyVectorGroupPaint.h"

#include "SOdysseyTabletAPISwitcher.h"

#include "Framework/Docking/LayoutExtender.h"
#include "LevelEditor.h"
#include "ToolMenus.h"
#include "Toolkits/AssetEditorModeUILayer.h"

#include "RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorGUI::~FOdysseyPainterEditorGUI()
{
}

FOdysseyPainterEditorGUI::FOdysseyPainterEditorGUI(FOdysseyPainterEditor* iEditor)
    : mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------- Tabs

void
FOdysseyPainterEditorGUI::Initialize()
{
    CreateTabs();
}

void
FOdysseyPainterEditorGUI::Finalize()
{
}

void
FOdysseyPainterEditorGUI::ExtendLevelEditorLayout(FLayoutExtender& Extender)
{
    Extender.ExtendLayout(FTabId(TEXT("PlacementBrowser")), ELayoutExtensionPosition::Below, FTabManager::FTab(FOdysseyPainterEditorToolsTab::StaticId(), ETabState::ClosedTab));
    Extender.ExtendLayout(FTabId(TEXT("LevelEditorSceneOutliner")), ELayoutExtensionPosition::Below, FTabManager::FTab(FOdysseyPainterEditorColorSelectorTab::StaticId(), ETabState::ClosedTab));
    Extender.ExtendLayout(FTabId(TEXT("LevelEditorSelectionDetails")), ELayoutExtensionPosition::Above, FTabManager::FTab(FOdysseyPainterEditorLayerStackTab::StaticId(), ETabState::ClosedTab));
    Extender.ExtendLayout(FTabId(TEXT("Sequencer")), ELayoutExtensionPosition::Above, FTabManager::FTab(FOdysseyPainterEditorAnimationTimelineTab::StaticId(), ETabState::ClosedTab));
}

void
FOdysseyPainterEditorGUI::CreateTabs()
{
    TSharedRef<FOdysseyPainterEditorMeshSelectorTab> meshSelectorTab = MakeShared<FOdysseyPainterEditorMeshSelectorTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorViewportTab> viewportTab = MakeShared<FOdysseyPainterEditorViewportTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorColorSelectorTab> colorSelectorTab = MakeShared<FOdysseyPainterEditorColorSelectorTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorToolsTab> toolsTab = MakeShared<FOdysseyPainterEditorToolsTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorVectorSceneTreeViewTab> vectorSceneTreeViewTab = MakeShared<FOdysseyPainterEditorVectorSceneTreeViewTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorLayerStackTab> layerStackTab = MakeShared<FOdysseyPainterEditorLayerStackTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorTextureDetailsTab> textureDetailsTab = MakeShared<FOdysseyPainterEditorTextureDetailsTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorFlipbookTimelineTab> flipbookTimelineTab = MakeShared<FOdysseyPainterEditorFlipbookTimelineTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorAnimationTimelineTab> animationTimelineTab = MakeShared<FOdysseyPainterEditorAnimationTimelineTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorAnimationLighttableTab> animationLighttableTab = MakeShared<FOdysseyPainterEditorAnimationLighttableTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorAnimationDetailsTab> animationDetailsTab = MakeShared<FOdysseyPainterEditorAnimationDetailsTab>(mEditor);

    //Used for the viewport drawing editor to know which tab to open by default

    flipbookTimelineTab->ShouldOpenByDefault(true);
    colorSelectorTab->ShouldOpenByDefault(true);
    toolsTab->ShouldOpenByDefault(true);
    layerStackTab->ShouldOpenByDefault(true);
    animationTimelineTab->ShouldOpenByDefault(true);

    mEditor->AddTab(toolsTab);
    mEditor->AddTab(meshSelectorTab);
    mEditor->AddTab(viewportTab);
    mEditor->AddTab(colorSelectorTab);
    mEditor->AddTab(vectorSceneTreeViewTab);
    mEditor->AddTab(layerStackTab);
    mEditor->AddTab(textureDetailsTab);
    mEditor->AddTab(flipbookTimelineTab);
    mEditor->AddTab(animationTimelineTab);
    mEditor->AddTab(animationLighttableTab);
    mEditor->AddTab(animationDetailsTab);
}

void
FOdysseyPainterEditorGUI::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorGUI::__VA_ARGS__ ), FCanExecuteAction() );

    MAP_ACTION(painterEditorCommands.SwitchTabletAPI, SwitchTabletAPI )

    //Need to rethink the commands and shortcuts to put them in the right place and not in GUI
    MAP_ACTION(painterEditorCommands.ClearCurrentLayer, ClearCurrentLayer)
    MAP_ACTION(painterEditorCommands.SelectAll, SelectAll)
    MAP_ACTION(painterEditorCommands.ClearCurrentSelection, ClearCurrentSelection)
    MAP_ACTION(painterEditorCommands.InvertSelection, InvertSelection)

    MAP_ACTION(painterEditorCommands.ToggleEraserButton, ToggleEraserButton)

    #undef MAP_ACTION
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------- Layout

void
FOdysseyPainterEditorGUI::BuildLayout(FOdysseyEditorLayoutBuilder& iBuilder)
{
    TSharedRef<FTabManager::FLayout> layout = iBuilder.GetLayout();

    TSharedRef<FTabManager::FArea> mainArea = iBuilder.CreateArea("MainArea");
    mainArea->SetOrientation(Orient_Horizontal);
    layout->AddArea(mainArea);

    TSharedRef<FTabManager::FSplitter> mainVerticalSplitter = iBuilder.CreateSplitter("MainVerticalSplitter");
    mainVerticalSplitter->SetOrientation(Orient_Vertical);
    mainVerticalSplitter->SetSizeCoefficient(1.f);

    mainArea->Split
    (
        mainVerticalSplitter
    );

    TSharedRef<FTabManager::FSplitter> mainHorizontalSplitter = iBuilder.CreateSplitter("MainHorizontalSplitter");
    mainHorizontalSplitter->SetOrientation(Orient_Horizontal);
    mainHorizontalSplitter->SetSizeCoefficient(1.f);

    mainVerticalSplitter->Split
    (
        mainHorizontalSplitter
    );

    TSharedRef<FTabManager::FSplitter> leftSplitter = iBuilder.CreateSplitter("LeftSplitter");
    leftSplitter->SetOrientation(Orient_Vertical);
    leftSplitter->SetSizeCoefficient(0.15f);

    TSharedRef<FTabManager::FSplitter> centerSplitter = iBuilder.CreateSplitter("CenterSplitter");
    centerSplitter->SetOrientation(Orient_Vertical);
    centerSplitter->SetSizeCoefficient(0.7f);

    TSharedRef<FTabManager::FSplitter> rightSplitter = iBuilder.CreateSplitter("RightSplitter");
    rightSplitter->SetOrientation(Orient_Vertical);
    rightSplitter->SetSizeCoefficient(0.15f);

    mainHorizontalSplitter->Split
    (
        leftSplitter
    );

    mainHorizontalSplitter->Split
    (
        centerSplitter
    );

    mainHorizontalSplitter->Split
    (
        rightSplitter
    );

    CreateLeftSection(iBuilder);
    CreateCenterSection(iBuilder);
    CreateRightSection(iBuilder);
    CreateBottomSection(iBuilder);
}

void
FOdysseyPainterEditorGUI::CreateLeftSection(FOdysseyEditorLayoutBuilder& iBuilder)
{
    TSharedRef<FTabManager::FSplitter> leftSplitter = iBuilder.GetSplitter("LeftSplitter");

    TSharedRef<FTabManager::FStack> currentToolStack = iBuilder.CreateStack("CurrentToolStack");
    currentToolStack->SetHideTabWell(false);
    currentToolStack->SetSizeCoefficient(0.33f);
    currentToolStack->AddTab(FOdysseyPainterEditorVectorSceneTreeViewTab::StaticId(),ETabState::OpenedTab);

    TSharedRef<FTabManager::FStack> toolsStack = iBuilder.CreateStack("ToolsStack");
    toolsStack->SetHideTabWell(false);
    toolsStack->SetSizeCoefficient(0.33f);
    toolsStack->AddTab(FOdysseyPainterEditorToolsTab::StaticId(), ETabState::OpenedTab);

    leftSplitter->Split
    (
        currentToolStack
    );

    leftSplitter->Split
    (
        toolsStack
    );
}

void
FOdysseyPainterEditorGUI::CreateRightSection(FOdysseyEditorLayoutBuilder& iBuilder)
{
    TSharedRef<FTabManager::FSplitter> rightSplitter = iBuilder.GetSplitter("RightSplitter");

    TSharedRef<FTabManager::FStack> colorSelectorStack = iBuilder.CreateStack("ColorSelectorStack");
    colorSelectorStack->SetHideTabWell(false);
    colorSelectorStack->SetSizeCoefficient(0.3f);
    colorSelectorStack->AddTab(FOdysseyPainterEditorColorSelectorTab::StaticId(), ETabState::OpenedTab);

    TSharedRef<FTabManager::FStack> textureLayerStackStack = iBuilder.CreateStack("TextureLayerStackStack");
    textureLayerStackStack->SetHideTabWell(false);
    textureLayerStackStack->SetSizeCoefficient(0.35f);
    textureLayerStackStack->AddTab(FOdysseyPainterEditorLayerStackTab::StaticId(), ETabState::OpenedTab);

    TSharedRef<FTabManager::FStack> textureDetailsStack = iBuilder.CreateStack("TextureDetailsStack");
    textureDetailsStack->SetHideTabWell(false);
    textureDetailsStack->SetSizeCoefficient(0.15f);
    textureDetailsStack->AddTab(FOdysseyPainterEditorLayerStackTab::StaticId(), ETabState::OpenedTab);

    rightSplitter->Split
    (
        colorSelectorStack
    );

    rightSplitter->Split
    (
        textureLayerStackStack
    );

    rightSplitter->Split
    (
        textureDetailsStack
    );
}

void
FOdysseyPainterEditorGUI::CreateCenterSection(FOdysseyEditorLayoutBuilder& iBuilder)
{
    TSharedRef<FTabManager::FSplitter> centerSplitter = iBuilder.GetSplitter("CenterSplitter");

    TSharedRef<FTabManager::FStack> viewportStack = iBuilder.CreateStack("viewportStack");
    viewportStack->SetHideTabWell(false);
    viewportStack->SetSizeCoefficient(0.975f);
    viewportStack->AddTab(FOdysseyPainterEditorViewportTab::StaticId(), ETabState::OpenedTab);

    centerSplitter->Split
    (
        viewportStack
    );
}

void
FOdysseyPainterEditorGUI::CreateBottomSection(FOdysseyEditorLayoutBuilder& iBuilder)
{
    TSharedRef<FTabManager::FSplitter> mainVerticalSplitter = iBuilder.GetSplitter("MainVerticalSplitter");
    TSharedRef<FTabManager::FStack> flipbookTimelineStack = iBuilder.CreateStack("FlipbookTimelineStack");
    flipbookTimelineStack->SetHideTabWell(false);
    flipbookTimelineStack->SetSizeCoefficient(0.2f);
    flipbookTimelineStack->AddTab(FOdysseyPainterEditorFlipbookTimelineTab::StaticId(), ETabState::OpenedTab);

    TSharedRef<FTabManager::FStack> animationTimelineStack = iBuilder.CreateStack("AnimationTimelineStack");
    animationTimelineStack->SetHideTabWell(false);
    animationTimelineStack->SetSizeCoefficient(0.2f);
    animationTimelineStack->AddTab(FOdysseyPainterEditorAnimationTimelineTab::StaticId(), ETabState::OpenedTab);

    mainVerticalSplitter->Split
    (
        animationTimelineStack
    );

    mainVerticalSplitter->Split
    (
        flipbookTimelineStack
    );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Shortcuts

void
FOdysseyPainterEditorGUI::SwitchTabletAPI()
{
    SOdysseyTabletAPISwitcher::Open();
}

void FOdysseyPainterEditorGUI::ClearCurrentLayer()
{
    if( mEditor && mEditor->GetSource() )
        mEditor->GetSource()->Clear();
}

void FOdysseyPainterEditorGUI::ToggleEraserButton()
{
    if( mEditor )
    {
        if( mEditor->GetCurrentTool()->IsA(UOdysseyPainterEditorRasterDrawingTool::StaticClass()) )
        {
            Cast< UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetCurrentTool())->BlendParameters.bEraserMode = !Cast< UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetCurrentTool())->BlendParameters.bEraserMode;
        }
    }
}

void
FOdysseyPainterEditorGUI::SelectAll()
{
    TArray<FVector2D> polyPoints;
    polyPoints.Add(FVector2D(0, 0));
    polyPoints.Add(FVector2D(mEditor->RasterSelection()->GetBlock()->Width(), 0));
    polyPoints.Add(FVector2D(mEditor->RasterSelection()->GetBlock()->Width(), mEditor->RasterSelection()->GetBlock()->Height()));
    polyPoints.Add(FVector2D(0, mEditor->RasterSelection()->GetBlock()->Height()));

    mEditor->RasterSelection()->Add(polyPoints);
}

void
FOdysseyPainterEditorGUI::ClearCurrentSelection()
{
    mEditor->RasterSelection()->Clear();
}

void FOdysseyPainterEditorGUI::InvertSelection()
{
    mEditor->RasterSelection()->Invert();
}

#undef LOCTEXT_NAMESPACE
