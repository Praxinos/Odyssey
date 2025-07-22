// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyEditorTab.h"

#include "Widgets/Docking/SDockTab.h"

/////////////////////////////////////////////////////
// FOdysseyEditorTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyEditorTab::~FOdysseyEditorTab()
{
}

FOdysseyEditorTab::FOdysseyEditorTab(FText iDisplayName, FSlateIcon iIcon)
    : mDisplayName(iDisplayName)
    , mIcon(iIcon)
    , mWidget(nullptr)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyEditorTab::Init()
{
    mWidget = CreateWidget();
}

void
FOdysseyEditorTab::Open()
{
    if (!CanOpen())
        return;

    TSharedPtr< FTabManager > tabManager = GetTabManager();
    if (!tabManager)
        return;

    tabManager->TryInvokeTab(FTabId(GetId()));
}

void
FOdysseyEditorTab::Close()
{
    TSharedPtr< FTabManager > tabManager = GetTabManager();
    if (!tabManager)
        return;

    TSharedPtr< SDockTab > livetab = tabManager->FindExistingLiveTab(FTabId(GetId()));
    if (livetab)
        livetab->RequestCloseTab();
}

bool
FOdysseyEditorTab::IsOpened() const
{
    TSharedPtr< FTabManager > tabManager = GetTabManager();
    if (!tabManager)
        return false;

    TSharedPtr< SDockTab > livetab = tabManager->FindExistingLiveTab(FTabId(GetId()));
    return !!livetab;
}

bool
FOdysseyEditorTab::CanOpen() const
{
    return true;
}

const FText&
FOdysseyEditorTab::GetName() const
{
    return mDisplayName;
}

const FSlateIcon&
FOdysseyEditorTab::GetIcon() const
{
    return mIcon;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FOdysseyEditorTab::BindShortcuts(FBaseToolkit* iToolkit)
{
}

void
FOdysseyEditorTab::ExtendMenu( TSharedRef<FExtender> iExtender )
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Spawner callback

void
FOdysseyEditorTab::SetTabManager(TSharedPtr< FTabManager > iTabManager)
{
    mTabManager = iTabManager;
}

TSharedPtr< FTabManager >
FOdysseyEditorTab::GetTabManager() const
{
    return mTabManager.Pin();
}

void
FOdysseyEditorTab::Register( TSharedRef<FWorkspaceItem>& iWorkspaceMenuCategoryRef)
{
    TSharedPtr< FTabManager > tabManager = GetTabManager();
    if (!tabManager)
        return;

    FOnSpawnTab onSpawnTab = FOnSpawnTab::CreateSP( AsShared(), &FOdysseyEditorTab::SpawnTab );
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
FOdysseyEditorTab::Unregister()
{
    TSharedPtr< FTabManager > tabManager = GetTabManager();
    if (!tabManager)
        return;

    tabManager->UnregisterTabSpawner( GetId() );
}

TSharedRef< SDockTab >
FOdysseyEditorTab::SpawnTab( const FSpawnTabArgs& iArgs )
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
FOdysseyEditorTab::DisplayName() const
{
    return mDisplayName;
}

const FSlateIcon&
FOdysseyEditorTab::Icon() const
{
    return mIcon;
}

const TSharedPtr<SWidget>&
FOdysseyEditorTab::Widget() const
{
    return mWidget;
}
