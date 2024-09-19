// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyEditorTab.h"

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
	, mShouldOpenByDefault(false)
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

FMinorTabConfig
FOdysseyEditorTab::GetMinorTabConfig()
{
	//Used to register tabs into the mode toolkit
	//Also allows us to define a default layout
	FMinorTabConfig config(GetId());
	config.OnSpawnTab = FOnSpawnTab::CreateSP( AsShared(), &FOdysseyEditorTab::SpawnTab );
	config.TabLabel = DisplayName();
	config.TabIcon = Icon();

	return config;
}

bool
FOdysseyEditorTab::ShouldOpenByDefault() const
{
	return mShouldOpenByDefault;
}

void
FOdysseyEditorTab::ShouldOpenByDefault(bool iShouldOpenByDefault)
{
	mShouldOpenByDefault = iShouldOpenByDefault;
}

void
FOdysseyEditorTab::Register( TSharedRef<FWorkspaceItem>& iWorkspaceMenuCategoryRef)
{
	TSharedPtr< FTabManager > tabManager = GetTabManager();
	if (!tabManager)
		return;

	FOnSpawnTab onSpawnTab = FOnSpawnTab::CreateSP( AsShared(), &FOdysseyEditorTab::SpawnTab );
	tabManager->RegisterTabSpawner(GetId(), onSpawnTab )
		.SetDisplayName( DisplayName() )
		.SetGroup(iWorkspaceMenuCategoryRef)
		.SetIcon( Icon() );
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
