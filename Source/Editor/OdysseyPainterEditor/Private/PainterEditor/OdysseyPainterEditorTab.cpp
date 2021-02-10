// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorTab.h"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorTab::~FOdysseyPainterEditorTab()
{
}

FOdysseyPainterEditorTab::FOdysseyPainterEditorTab(FName iID, FText iDisplayName, FSlateIcon iIcon)
	: mID(iID)
	, mDisplayName(iDisplayName)
	, mIcon(iIcon)
	, mWidget(nullptr)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyPainterEditorTab::Init()
{
	mWidget = CreateWidget();
}

void
FOdysseyPainterEditorTab::RegisterTabSpawner( const TSharedRef< class FTabManager >& iTabManager, TSharedRef<FWorkspaceItem>& iWorkspaceMenuCategoryRef)
{
    //The viewport: the central area where we can draw on images
    iTabManager->RegisterTabSpawner( mID, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorTab::SpawnTab ) )
        .SetDisplayName( mDisplayName )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( mIcon );
}

void
FOdysseyPainterEditorTab::UnregisterTabSpawner( const TSharedRef< class FTabManager >& iTabManager )
{
    iTabManager->UnregisterTabSpawner( mID );
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Spawner callbacks

TSharedRef< SDockTab >
FOdysseyPainterEditorTab::SpawnTab( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == mID );

    return SNew( SDockTab )
        .ShouldAutosize( true )
        .Label( mDisplayName )
        [
            mWidget.ToSharedRef()
        ];
}

const FName&
FOdysseyPainterEditorTab::ID() const
{
	return mID;
}

const FText&
FOdysseyPainterEditorTab::DisplayName() const
{
	return mDisplayName;
}

const FSlateIcon&
FOdysseyPainterEditorTab::Icon() const
{
	return mIcon;
}

const TSharedPtr<SWidget>&
FOdysseyPainterEditorTab::Widget() const
{
	return mWidget;
}

#undef LOCTEXT_NAMESPACE
