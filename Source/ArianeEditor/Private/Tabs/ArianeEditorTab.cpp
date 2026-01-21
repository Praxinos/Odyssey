// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeEditorTab.h"
// Unreal headers
#include "Framework/Docking/TabManager.h"
#include "Widgets/Docking/SDockTab.h"

/////////////////////////////////////////////////////
// FArianeEditorTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FArianeEditorTab::~FArianeEditorTab()
{
}

FArianeEditorTab::FArianeEditorTab(FText iDisplayName, FSlateIcon iIcon)
    : mDisplayName(iDisplayName)
    , mIcon(iIcon)
    , mWidget(nullptr)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FArianeEditorTab::Init()
{
    mWidget = CreateWidget();
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
    return mDisplayName;
}

const FSlateIcon&
FArianeEditorTab::GetIcon() const
{
    return mIcon;
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

void
FArianeEditorTab::SetTabManager( TSharedPtr<FTabManager> iTabManager)
{
    mTabManager = iTabManager;
}

TSharedPtr< FTabManager >
FArianeEditorTab::GetTabManager() const
{
    return mTabManager;
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
        .SetDisplayName( DisplayName() )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( Icon() )
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
        .Label( mDisplayName )
        [
            mWidget.ToSharedRef()
        ];
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

const FText&
FArianeEditorTab::DisplayName() const
{
    return mDisplayName;
}

const FSlateIcon&
FArianeEditorTab::Icon() const
{
    return mIcon;
}

const TSharedPtr<SWidget>&
FArianeEditorTab::Widget() const
{
    return mWidget;
}
