// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
#include "OdysseyPainterEditorToolCollectionTab.h"
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
FOdysseyPainterEditorGUI::CreateTabs()
{
    TSharedRef<FOdysseyPainterEditorMeshSelectorTab> meshSelectorTab = MakeShared<FOdysseyPainterEditorMeshSelectorTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorViewportTab> viewportTab = MakeShared<FOdysseyPainterEditorViewportTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorColorSelectorTab> colorSelectorTab = MakeShared<FOdysseyPainterEditorColorSelectorTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorToolsTab> toolsTab = MakeShared<FOdysseyPainterEditorToolsTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorToolCollectionTab> toolCollectionTab = MakeShared<FOdysseyPainterEditorToolCollectionTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorVectorSceneTreeViewTab> vectorSceneTreeViewTab = MakeShared<FOdysseyPainterEditorVectorSceneTreeViewTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorLayerStackTab> layerStackTab = MakeShared<FOdysseyPainterEditorLayerStackTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorTextureDetailsTab> textureDetailsTab = MakeShared<FOdysseyPainterEditorTextureDetailsTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorFlipbookTimelineTab> flipbookTimelineTab = MakeShared<FOdysseyPainterEditorFlipbookTimelineTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorAnimationTimelineTab> animationTimelineTab = MakeShared<FOdysseyPainterEditorAnimationTimelineTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorAnimationLighttableTab> animationLighttableTab = MakeShared<FOdysseyPainterEditorAnimationLighttableTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorAnimationDetailsTab> animationDetailsTab = MakeShared<FOdysseyPainterEditorAnimationDetailsTab>(mEditor);

    //Used for the viewport drawing editor to know which tab to open by default

    mEditor->AddTab(toolsTab);
    mEditor->AddTab(toolCollectionTab);
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

    MAP_ACTION(painterEditorCommands.ToggleEraserButton, ToggleEraserButton)

    #undef MAP_ACTION
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

#undef LOCTEXT_NAMESPACE
