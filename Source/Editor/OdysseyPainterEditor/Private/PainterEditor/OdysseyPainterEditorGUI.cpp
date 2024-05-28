// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorGUI.h"
#include "OdysseyEditorTab.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyAssetEditorToolkit.h"
#include "SOdysseyAboutWindow.h"
#include "SOdysseyTabletAPISwitcher.h"
#include "Models/OdysseyPainterEditorCommands.h"

#include "OdysseyPainterEditorBrushSelectorTab.h"
#include "OdysseyPainterEditorColorPaletteTab.h"
#include "OdysseyPainterEditorColorSlidersTab.h"
#include "OdysseyPainterEditorColorWheelTab.h"
#include "OdysseyPainterEditorHUDTab.h"
#include "OdysseyPainterEditorMeshSelectorTab.h"
#include "OdysseyPainterEditorTopTab.h"
#include "OdysseyPainterEditorToolsTab.h"
#include "OdysseyPainterEditorToolOptionsTab.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "OdysseyPainterEditorVectorSceneTreeViewTab.h"

#include "Framework/Docking/LayoutExtender.h"
#include "LevelEditor.h"
#include "ToolMenus.h"
#include "Toolkits/AssetEditorModeUILayer.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"

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
    Extender.ExtendLayout(FTabId(TEXT("PlacementBrowser")), ELayoutExtensionPosition::Below, FTabManager::FTab(FOdysseyPainterEditorToolOptionsTab::StaticId(), ETabState::ClosedTab));
    Extender.ExtendLayout(FTabId(TEXT("PlacementBrowser")), ELayoutExtensionPosition::Below, FTabManager::FTab(FOdysseyPainterEditorBrushSelectorTab::StaticId(), ETabState::ClosedTab));
    
    
    //Extender.ExtendLayout(FTabId(TEXT("LevelEditorSceneOutliner")), ELayoutExtensionPosition::Below, FTabManager::FTab(FOdysseyPainterEditorPaletteTab::StaticId(), ETabState::ClosedTab));
    //Extender.ExtendLayout(FTabId(TEXT("LevelEditorSceneOutliner")), ELayoutExtensionPosition::Below, FTabManager::FTab(FOdysseyPainterEditorColorSlidersTab::StaticId(), ETabState::ClosedTab));
    Extender.ExtendLayout(FTabId(TEXT("LevelEditorSceneOutliner")), ELayoutExtensionPosition::Below, FTabManager::FTab(FOdysseyPainterEditorColorWheelTab::StaticId(), ETabState::ClosedTab));
}

void
FOdysseyPainterEditorGUI::CreateTabs()
{
    TSharedRef<FOdysseyPainterEditorMeshSelectorTab> meshSelectorTab = MakeShared<FOdysseyPainterEditorMeshSelectorTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorPaletteTab> paletteTab = MakeShared<FOdysseyPainterEditorPaletteTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorViewportTab> viewportTab = MakeShared<FOdysseyPainterEditorViewportTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorHUDTab> HUDTab = MakeShared<FOdysseyPainterEditorHUDTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorBrushSelectorTab> brushSelectorTab = MakeShared<FOdysseyPainterEditorBrushSelectorTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorColorWheelTab> colorWheelTab = MakeShared<FOdysseyPainterEditorColorWheelTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorColorSlidersTab> colorSlidersTab = MakeShared<FOdysseyPainterEditorColorSlidersTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorToolsTab> toolsTab = MakeShared<FOdysseyPainterEditorToolsTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorTopTab> topTab = MakeShared<FOdysseyPainterEditorTopTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorToolOptionsTab> toolOptionsTab = MakeShared<FOdysseyPainterEditorToolOptionsTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorVectorSceneTreeViewTab> vectorSceneTreeViewTab = MakeShared<FOdysseyPainterEditorVectorSceneTreeViewTab>(mEditor);

    //Used for the viewport drawing editor to know which tab to open by default
    //paletteTab->ShouldOpenByDefault(true);
    brushSelectorTab->ShouldOpenByDefault(true);
    colorWheelTab->ShouldOpenByDefault(true);
    //colorSlidersTab->ShouldOpenByDefault(true);
    toolsTab->ShouldOpenByDefault(true);
    //selectedVectorObjectTab->ShouldOpenByDefault(true);
    toolOptionsTab->ShouldOpenByDefault(true);


    mEditor->AddTab(brushSelectorTab);
    mEditor->AddTab(toolOptionsTab);
    mEditor->AddTab(toolsTab);

    mEditor->AddTab(meshSelectorTab);
    mEditor->AddTab(paletteTab);
    mEditor->AddTab(viewportTab);
    mEditor->AddTab(HUDTab);
    mEditor->AddTab(colorWheelTab);
    mEditor->AddTab(colorSlidersTab);
    mEditor->AddTab(topTab);
    mEditor->AddTab(vectorSceneTreeViewTab);
}

void
FOdysseyPainterEditorGUI::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorGUI::__VA_ARGS__ ), FCanExecuteAction() );

    MAP_ACTION(painterEditorCommands.AboutIliad, AboutIliad )
    MAP_ACTION(painterEditorCommands.VisitPraxinosWebsite, VisitPraxinosWebsite )
    MAP_ACTION(painterEditorCommands.ManualAndReleaseNotes, ManualAndReleaseNotes )
    MAP_ACTION(painterEditorCommands.GetBrushPack, GetBrushPack )
    MAP_ACTION(painterEditorCommands.Discord, Discord )
    MAP_ACTION(painterEditorCommands.SwitchTabletAPI, SwitchTabletAPI )

    //Need to rethink the commands and shortcuts to put them in the right place and not in GUI
    MAP_ACTION(painterEditorCommands.ClearCurrentLayer, ClearCurrentLayer)
    /* MAP_ACTION(painterEditorCommands.ClearCurrentSelection, ClearCurrentSelection)
    MAP_ACTION(painterEditorCommands.CopyCurrentSelection, CopyCurrentSelection)
    MAP_ACTION(painterEditorCommands.PasteCurrentSelection, PasteCurrentSelection)
    MAP_ACTION(painterEditorCommands.PasteCurrentSelectionInNewLayer, PasteCurrentSelectionInNewLayer) */

    MAP_ACTION(painterEditorCommands.ToggleEraserButton, ToggleEraserButton)

    #undef MAP_ACTION
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Menu and Toolbar

void
FOdysseyPainterEditorGUI::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
    ExtendMenuAbout( iOwner, iMenuName );
}

void
FOdysseyPainterEditorGUI::ExtendMenuAbout( FToolMenuOwner iOwner, FName iMenuName )
{
    if (!UToolMenus::Get()->IsMenuRegistered( *(iMenuName.ToString() + FString(".Iliad") ) ) )
    {
        UToolMenu* menu = UToolMenus::Get()->FindMenu(iMenuName); // We register our menu under the parenthood of MainFrame.MainMenu

        //Adding the Odyssey sub menu to our menu
        menu->AddSubMenu(
            iOwner,
            NAME_None,
            "Iliad",
            LOCTEXT("about-menu.iliad-submenu.name", "Iliad"),
            LOCTEXT("about-menu.iliad-submenu.tooltip", "Iliad Actions")
        );
    }
    UToolMenu* menu = UToolMenus::Get()->FindMenu( *(iMenuName.ToString() + FString(".Iliad") ) );

    //Adding entries in our menu
    FToolMenuSection& aboutSection = menu->AddSection("About ILIAD", LOCTEXT("about-menu.iliad.iliad-section.name", "ILIAD"));
    {
        aboutSection.AddMenuEntry(
            FOdysseyPainterEditorCommands::Get().AboutIliad
            , LOCTEXT("about-menu.iliad.about-iliad.name", "About Iliad")
            , LOCTEXT("about-menu.iliad.about-iliad.tooltip", "to get more information about the plugin, the team that created it, etc.")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , NAME_None);
        aboutSection.AddMenuEntry(
            FOdysseyPainterEditorCommands::Get().VisitPraxinosWebsite
            , LOCTEXT("about-menu.iliad.about-iliad.visit-praxinos-website.name", "About Praxinos ...")
            , LOCTEXT("about-menu.iliad.about-iliad.visit-praxinos-website.tooltip", "to get more information about the company Praxinos, its projects, etc.")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.PraxinosLogo16")
            , NAME_None );
        aboutSection.AddMenuEntry(
            FOdysseyPainterEditorCommands::Get().ManualAndReleaseNotes
            , LOCTEXT("about-menu.iliad.about-iliad.manual-and-release-notes.name", "Manual and Release Notes ...")
            , LOCTEXT("about-menu.iliad.about-iliad.manual-and-release-notes.tooltip", "to get a full changelog of each Iliad version.")
            , FSlateIcon("OdysseyStyle", "About.Manual16")
            , NAME_None );
        aboutSection.AddMenuEntry(
            FOdysseyPainterEditorCommands::Get().GetBrushPack
            , LOCTEXT("about-menu.iliad.about-iliad.get-more-brushes.name", "Get more brushes ...")
            , LOCTEXT("about-menu.iliad.about-iliad.get-more-brushes.tooltip", "Want more brushes ? Just follow this link !")
            , FSlateIcon("OdysseyStyle", "About.MorePencils16")
            , NAME_None );
        aboutSection.AddMenuEntry(
            FOdysseyPainterEditorCommands::Get().Discord
            , LOCTEXT("about-menu.iliad.about-iliad.talk-with-the-developpers.name", "Talk with the developpers ...")
            , LOCTEXT("about-menu.iliad.about-iliad.talk-with-the-developpers.tooltip", "For those who want to discuss with us about the next improvements")
            , FSlateIcon("OdysseyStyle", "About.Discord2_16")
            , NAME_None );
    }
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
}

void
FOdysseyPainterEditorGUI::CreateLeftSection(FOdysseyEditorLayoutBuilder& iBuilder)
{
    TSharedRef<FTabManager::FSplitter> leftSplitter = iBuilder.GetSplitter("LeftSplitter");

    TSharedRef<FTabManager::FStack> brushStack = iBuilder.CreateStack("BrushStack");
    brushStack->SetHideTabWell(false);
    brushStack->SetSizeCoefficient(0.33f);
    brushStack->AddTab(FOdysseyPainterEditorBrushSelectorTab::StaticId(), ETabState::OpenedTab);

    TSharedRef<FTabManager::FStack> currentToolStack = iBuilder.CreateStack("CurrentToolStack");
    currentToolStack->SetHideTabWell(false);
    currentToolStack->SetSizeCoefficient(0.33f);
    currentToolStack->AddTab(FOdysseyPainterEditorToolOptionsTab::StaticId(), ETabState::OpenedTab);
    currentToolStack->AddTab(FOdysseyPainterEditorVectorSceneTreeViewTab::StaticId(),ETabState::OpenedTab);
    currentToolStack->SetForegroundTab(FOdysseyPainterEditorToolOptionsTab::StaticId());

    TSharedRef<FTabManager::FStack> toolsStack = iBuilder.CreateStack("ToolsStack");
    toolsStack->SetHideTabWell(false);
    toolsStack->SetSizeCoefficient(0.33f);
    toolsStack->AddTab(FOdysseyPainterEditorToolsTab::StaticId(), ETabState::OpenedTab);

    leftSplitter->Split
    (
        brushStack
    );

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

    TSharedRef<FTabManager::FStack> colorWheelStack = iBuilder.CreateStack("ColorWheelStack");
    colorWheelStack->SetHideTabWell(false);
    colorWheelStack->SetSizeCoefficient(0.3f);
    colorWheelStack->AddTab(FOdysseyPainterEditorColorWheelTab::StaticId(), ETabState::OpenedTab);

    TSharedRef<FTabManager::FStack> colorSliderStack = iBuilder.CreateStack("ColorSliderStack");
    colorSliderStack->SetHideTabWell(false);
    colorSliderStack->SetSizeCoefficient(0.3f);
    colorSliderStack->AddTab(FOdysseyPainterEditorColorSlidersTab::StaticId(), ETabState::OpenedTab);

    TSharedRef<FTabManager::FStack> colorPaletteStack = iBuilder.CreateStack("ColorPaletteStack");
    colorPaletteStack->SetHideTabWell(false);
    colorPaletteStack->SetSizeCoefficient(0.33f);
    colorPaletteStack->AddTab(FOdysseyPainterEditorPaletteTab::StaticId(), ETabState::OpenedTab);

    rightSplitter->Split
    (
        colorWheelStack
    );

    rightSplitter->Split
    (
        colorSliderStack
    );

    rightSplitter->Split
    (
        colorPaletteStack
    );
}

void
FOdysseyPainterEditorGUI::CreateCenterSection(FOdysseyEditorLayoutBuilder& iBuilder)
{
    TSharedRef<FTabManager::FSplitter> centerSplitter = iBuilder.GetSplitter("CenterSplitter");

    TSharedRef<FTabManager::FStack> topTabStack = iBuilder.CreateStack("TopTabStack");
    topTabStack->SetHideTabWell(true);
    topTabStack->SetSizeCoefficient(0.025f);
    topTabStack->AddTab(FOdysseyPainterEditorTopTab::StaticId(), ETabState::OpenedTab);

    TSharedRef<FTabManager::FStack> viewportStack = iBuilder.CreateStack("viewportStack");
    viewportStack->SetHideTabWell(false);
    viewportStack->SetSizeCoefficient(0.975f);
    viewportStack->AddTab(FOdysseyPainterEditorViewportTab::StaticId(), ETabState::OpenedTab);

    centerSplitter->Split
    (
        topTabStack
    );

    centerSplitter->Split
    (
        viewportStack
    );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Shortcuts

void
FOdysseyPainterEditorGUI::AboutIliad()
{
    TSharedPtr<SWindow> parentWindow = FGlobalTabmanager::Get()->GetRootWindow();
    SOdysseyAboutWindow::Open(parentWindow);
}

void
FOdysseyPainterEditorGUI::VisitPraxinosWebsite()
{
    FString URL = "https://praxinos.coop/";
    FPlatformProcess::LaunchURL( *URL, NULL, NULL );
}

void
FOdysseyPainterEditorGUI::ManualAndReleaseNotes()
{
    FString URL = "https://praxinos.coop/Documentation/Iliad/User/html/";
    FPlatformProcess::LaunchURL( *URL, NULL, NULL );
}

void
FOdysseyPainterEditorGUI::GetBrushPack()
{
    FString URL = "https://www.unrealengine.com/marketplace/en-US/profile/PraxinosCoop";
    FPlatformProcess::LaunchURL( *URL, NULL, NULL );
}

void
FOdysseyPainterEditorGUI::Discord()
{
    FString URL = "https://discord.gg/gEd6pj7";
    FPlatformProcess::LaunchURL( *URL, NULL, NULL );
}

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