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

FOdysseyEditorTab::FOdysseyEditorTab(FName iID, FText iDisplayName, FSlateIcon iIcon)
	: mID(iID)
	, mDisplayName(iDisplayName)
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

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FOdysseyEditorTab::BindShortcuts(FBaseToolkit* iToolkit)
{
}

void
FOdysseyEditorTab::OnToolkitInitialized(FBaseToolkit* iToolkit)
{
}

void
FOdysseyEditorTab::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Spawner callback

TSharedRef< SDockTab >
FOdysseyEditorTab::SpawnTab( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == mID );

    return SNew( SDockTab )
        .Label( mDisplayName )
        [
            mWidget.ToSharedRef()
        ];
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

const FName&
FOdysseyEditorTab::ID() const
{
	return mID;
}

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

#undef LOCTEXT_NAMESPACE
