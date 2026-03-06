// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeEditorTab.h"
// Unreal headers
#include "Framework/Docking/TabManager.h"
#include "Widgets/Docking/SDockTab.h"
#include "LevelEditor.h"

/////////////////////////////////////////////////////
// FArianeEditorTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FArianeEditorTab::~FArianeEditorTab()
{
}

FArianeEditorTab::FArianeEditorTab(FText InDisplayName, FSlateIcon InIcon)
    : DisplayName(InDisplayName)
    , Icon(InIcon)
    , Widget(nullptr)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FArianeEditorTab::Init()
{
    Widget = CreateWidget();
}

void
FArianeEditorTab::Open()
{
    TSharedPtr< FTabManager > tabManager = GetTabManager();

    if( CanOpen() )
    {
        tabManager->TryInvokeTab( FTabId( GetId() ) );
    }
}

void
FArianeEditorTab::Close()
{
    TSharedPtr< FTabManager > tabManager = GetTabManager();
    TSharedPtr< SDockTab > livetab = tabManager->FindExistingLiveTab(FTabId(GetId()));

    if (livetab)
    {
        livetab->RequestCloseTab();
    }
}

bool
FArianeEditorTab::IsOpen() const
{
    TSharedPtr< FTabManager > tabManager = GetTabManager();
    TSharedPtr< SDockTab > livetab = tabManager->FindExistingLiveTab( FTabId( GetId() ) );

    return livetab ? true : false;
}

bool
FArianeEditorTab::CanOpen() const
{
    return true;
}

const FText&
FArianeEditorTab::GetName() const
{
    return DisplayName;
}

const FSlateIcon&
FArianeEditorTab::GetIcon() const
{
    return Icon;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FArianeEditorTab::BindShortcuts(FBaseToolkit* iToolkit)
{
}

void
FArianeEditorTab::ExtendMenu( TSharedRef<FExtender> iExtender )
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Spawner callback

TSharedPtr< FTabManager >
FArianeEditorTab::GetTabManager()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));

    return LevelEditorModule.GetLevelEditorTabManager()->AsShared();
}

void
FArianeEditorTab::Register( TSharedRef<FWorkspaceItem>& iWorkspaceMenuCategoryRef )
{
    TSharedPtr< FTabManager > tabManager = GetTabManager();
    FOnSpawnTab onSpawnTab = FOnSpawnTab::CreateSP( AsShared(), &FArianeEditorTab::SpawnTab );

    TAttribute<ETabSpawnerMenuType::Type> menuType = MakeAttributeLambda(
        [this]() -> ETabSpawnerMenuType::Type
        {
            return CanOpen() ? ETabSpawnerMenuType::Enabled : ETabSpawnerMenuType::Hidden;
        }
    );

    tabManager->RegisterTabSpawner(GetId(), onSpawnTab )
        .SetDisplayName( DisplayName )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( GetIcon() )
        .SetReadOnlyBehavior( ETabReadOnlyBehavior::Hidden )
        .SetMenuType( menuType );
}

void
FArianeEditorTab::Unregister()
{
    TSharedPtr<FTabManager> tabManager = GetTabManager();

    tabManager->UnregisterTabSpawner( GetId() );
}

TSharedRef< SDockTab >
FArianeEditorTab::SpawnTab( const FSpawnTabArgs& iArgs )
{
    return SNew( SDockTab )
        .Label( DisplayName )
        [
            Widget.ToSharedRef()
        ];
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

const TSharedPtr<SWidget>&
FArianeEditorTab::GetWidget() const
{
    return Widget;
}
