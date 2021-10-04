// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorGUI.h"
#include "OdysseyEditorTab.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorBrushSelectorTab.h"
#include "OdysseyPainterEditorColorSlidersTab.h"
#include "OdysseyPainterEditorColorWheelTab.h"
#include "OdysseyPainterEditorMeshSelectorTab.h"
#include "OdysseyPainterEditorStrokeOptionsTab.h"
#include "OdysseyPainterEditorTopTab.h"
#include "OdysseyPainterEditorToolsTab.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "SOdysseyAboutScreen.h"
#include "SOdysseyTabletAPISwitcher.h"
#include "Models/OdysseyPainterEditorCommands.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorGUI"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorGUI::~FOdysseyPainterEditorGUI()
{
}

FOdysseyPainterEditorGUI::FOdysseyPainterEditorGUI(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorGUI(iEditor)
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------- Tabs

void
FOdysseyPainterEditorGUI::CreateTabs()
{
    FOdysseyEditorGUI::CreateTabs();

    ODYSSEY_ADD_TAB(mMeshSelectorTab, FOdysseyPainterEditorMeshSelectorTab, mEditor)
    ODYSSEY_ADD_TAB(mViewportTab, FOdysseyPainterEditorViewportTab, mEditor);
    ODYSSEY_ADD_TAB(mBrushSelectorTab, FOdysseyPainterEditorBrushSelectorTab, mEditor);
    ODYSSEY_ADD_TAB(mColorWheelTab, FOdysseyPainterEditorColorWheelTab, mEditor);
    ODYSSEY_ADD_TAB(mColorSlidersTab, FOdysseyPainterEditorColorSlidersTab, mEditor);
    ODYSSEY_ADD_TAB(mToolsTab, FOdysseyPainterEditorToolsTab, mEditor);
    ODYSSEY_ADD_TAB(mTopTab, FOdysseyPainterEditorTopTab, mEditor);
    ODYSSEY_ADD_TAB(mStrokeOptionsTab, FOdysseyPainterEditorStrokeOptionsTab, mEditor);
}

void
FOdysseyPainterEditorGUI::BindShortcuts(FBaseToolkit* iToolkit)
{
    FOdysseyEditorGUI::BindShortcuts(iToolkit);

    //---

    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorGUI::__VA_ARGS__ ), FCanExecuteAction() );

    MAP_ACTION(painterEditorCommands.AboutIliad, AboutIliad )
    MAP_ACTION(painterEditorCommands.VisitPraxinosWebsite, VisitPraxinosWebsite )
    MAP_ACTION(painterEditorCommands.ManualAndReleaseNotes, ManualAndReleaseNotes )
    MAP_ACTION(painterEditorCommands.GetBrushPack, GetBrushPack )
    MAP_ACTION(painterEditorCommands.Discord, Discord )
    MAP_ACTION(painterEditorCommands.SwitchTabletAPI, SwitchTabletAPI )

    #undef MAP_ACTION
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Menu and Toolbar

void
FOdysseyPainterEditorGUI::FillExtender(FBaseToolkit* iToolkit, TSharedPtr<FExtender>& ioExtender)
{
    FOdysseyEditorGUI::FillExtender(iToolkit, ioExtender);

    //---

    ioExtender->AddMenuExtension(
        "HelpApplication",
        EExtensionHook::After,
        iToolkit->GetToolkitCommands(),
        FMenuExtensionDelegate::CreateRaw( this, &FOdysseyPainterEditorGUI::ExtendMenuAbout ) );
}

//static
void
FOdysseyPainterEditorGUI::ExtendMenuAbout( FMenuBuilder& ioMenuBuilder )
{
    ioMenuBuilder.BeginSection( "About", LOCTEXT( "OdysseyPainter", "ILIAD" ) );
    {
        ioMenuBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().AboutIliad
        , NAME_None
        , LOCTEXT( "AboutIliad", "About Iliad" )
        , LOCTEXT( "AboutIliad_Tooltip", "to get more information about the plugin, the team that created it, etc." )
        , FSlateIcon( "OdysseyStyle", "OdysseyLogo.Iliad16" ) );
        ioMenuBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VisitPraxinosWebsite
        , NAME_None
        , LOCTEXT( "VisitPraxinosWebsite", "About Praxinos ..." )
        , LOCTEXT( "VisitPraxinosWebsite_Tooltip", "to get more information about the company Praxinos, its projects, etc." )
        , FSlateIcon( "OdysseyStyle", "OdysseyLogo.PraxinosLogo16" ) );
        ioMenuBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().ManualAndReleaseNotes
        , NAME_None
        , LOCTEXT( "ManualAndReleaseNotes", "Manual and Release Notes ..." )
        , LOCTEXT( "ReleaseNotes_Tooltip", "to get a full changelog of each Iliad version." )
        , FSlateIcon( "OdysseyStyle", "About.Manual16" ) );
        ioMenuBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().GetBrushPack
        , NAME_None
        , LOCTEXT( "GetMoreBrushes", "Get more brushes ..." )
        , LOCTEXT( "ReleaseNotes_Tooltip", "Want more brushes ? Just follow this link !" )
        , FSlateIcon( "OdysseyStyle", "About.MorePencils16" ) );
        ioMenuBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().Discord
        , NAME_None
        , LOCTEXT( "TalkWithTheDeveloppers", "Talk with the developpers ..." )
        , LOCTEXT( "ReleaseNotes_Tooltip", "For those who want to discuss with us about the next improvements" )
        , FSlateIcon( "OdysseyStyle", "About.Discord2_16" ) );
    }
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------- Layout

TSharedPtr<FTabManager::FLayout>
FOdysseyPainterEditorGUI::CreateLayout()
{
    return FOdysseyEditorGUI::CreateLayout()
        ->AddArea
        (
            FTabManager::NewPrimaryArea()
            ->SetOrientation(Orient_Horizontal)
            ->Split
            (
                CreateMainSection()
            )
        );
}

TSharedRef<FTabManager::FSplitter>
FOdysseyPainterEditorGUI::CreateLeftSection()
{
    return FTabManager::NewSplitter()
        ->SetOrientation(Orient_Vertical)
        ->SetSizeCoefficient(0.15f)
        // Brush Selector
        ->Split
        (
            FTabManager::NewStack()
            ->AddTab(mBrushSelectorTab->ID(), ETabState::OpenedTab)
            ->SetHideTabWell(false)
            ->SetSizeCoefficient(0.33f)
        )
        // Mesh Selector + Stroke Options
        ->Split
        (
            FTabManager::NewStack()
            ->SetHideTabWell(false)
            ->SetSizeCoefficient(0.33f)
            // Mesh Selector
            ->AddTab(mMeshSelectorTab->ID(), ETabState::OpenedTab)
            ->SetHideTabWell(false)
            ->SetSizeCoefficient(0.33f)
            // Stroke Options
            ->AddTab(mStrokeOptionsTab->ID(), ETabState::OpenedTab)
            ->SetHideTabWell(false)
            ->SetSizeCoefficient(0.33f)

        )
        // Tools
        ->Split
        (
            FTabManager::NewStack()
            ->AddTab(mToolsTab->ID(), ETabState::OpenedTab)
            ->SetHideTabWell(false)
            ->SetSizeCoefficient(0.33f)
        );
}

TSharedRef<FTabManager::FSplitter>
FOdysseyPainterEditorGUI::CreateRightSection()
{
    return FTabManager::NewSplitter()
        ->SetSizeCoefficient(0.15f)
        ->SetOrientation(Orient_Vertical)
        // ColorSelector
        ->Split
        (
            FTabManager::NewStack()
            ->AddTab(mColorWheelTab->ID(), ETabState::OpenedTab)
            ->SetHideTabWell(false)
            ->SetSizeCoefficient(0.3f)
        )
        // ColorSliders
        ->Split
        (
            FTabManager::NewStack()
            ->AddTab(mColorSlidersTab->ID(), ETabState::OpenedTab)
            ->SetHideTabWell(false)
            ->SetSizeCoefficient(0.3f)
        );
}

TSharedRef<FTabManager::FSplitter>
FOdysseyPainterEditorGUI::CreateMiddleSection()
{

    return FTabManager::NewSplitter()
        ->SetOrientation(Orient_Vertical)
        ->SetSizeCoefficient(0.7f)
        // Top Bar
        ->Split
        (
            FTabManager::NewStack()
            ->AddTab(mTopTab->ID(), ETabState::OpenedTab)
            ->SetHideTabWell(true)
            ->SetSizeCoefficient(0.025f)
        )
        // Viewport
        ->Split
        (
            FTabManager::NewStack()
            ->AddTab(mViewportTab->ID(), ETabState::OpenedTab)
            ->SetHideTabWell(false)
            ->SetSizeCoefficient(0.975f)
        );
}



TSharedRef<FTabManager::FSplitter>
FOdysseyPainterEditorGUI::CreateMainSection()
{
    return FTabManager::NewSplitter()
        ->SetOrientation(Orient_Vertical)
        ->SetSizeCoefficient(1.f)
        // TopMost Part
        ->Split
        (
            FTabManager::NewSplitter()
            ->SetSizeCoefficient(1.f)
            ->SetOrientation(Orient_Horizontal)
            // Left Bar
            ->Split
            (
                CreateLeftSection()
            )
            // Middle bar
            ->Split
            (
                CreateMiddleSection()
            )
            // Right bar
            ->Split
            (
                CreateRightSection()
            )
        );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

TSharedPtr<FOdysseyPainterEditorViewportTab>&
FOdysseyPainterEditorGUI::GetViewportTab()
{
    return mViewportTab;
}

TSharedPtr<FOdysseyPainterEditorBrushSelectorTab>&
FOdysseyPainterEditorGUI::GetBrushSelectorTab()
{
    return mBrushSelectorTab;
}

TSharedPtr<FOdysseyPainterEditorMeshSelectorTab>&
FOdysseyPainterEditorGUI::GetMeshSelectorTab()
{
    return mMeshSelectorTab;
}

TSharedPtr<FOdysseyPainterEditorColorWheelTab>&
FOdysseyPainterEditorGUI::GetColorWheelTab()
{
    return mColorWheelTab;
}

TSharedPtr<FOdysseyPainterEditorColorSlidersTab>&
FOdysseyPainterEditorGUI::GetColorSlidersTab()
{
    return mColorSlidersTab;
}

TSharedPtr<FOdysseyPainterEditorStrokeOptionsTab>&
FOdysseyPainterEditorGUI::GetStrokeOptionsTab()
{
    return mStrokeOptionsTab;
}

TSharedPtr<FOdysseyPainterEditorTopTab>&
FOdysseyPainterEditorGUI::GetTopTab()
{
    return mTopTab;
}

TSharedPtr<FOdysseyPainterEditorToolsTab>&
FOdysseyPainterEditorGUI::GetToolsTab()
{
    return mToolsTab;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Shortcuts

void
FOdysseyPainterEditorGUI::AboutIliad()
{
    TSharedPtr<SWindow> parentWindow = FGlobalTabmanager::Get()->GetRootWindow();
    SOdysseyAboutScreen::Open(parentWindow);
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
    FString URL = "https://www.unrealengine.com/marketplace/en-US/product/brush-pack-for-iliad";
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

#undef LOCTEXT_NAMESPACE