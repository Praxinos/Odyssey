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
    BindShortcuts();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FOdysseyPainterEditorTab::BindShortcuts()
{
}

void
FOdysseyPainterEditorTab::OnToolkitInitialized()
{
}

void
FOdysseyPainterEditorTab::FillExtender(TSharedPtr<FExtender>& ioExtender)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Spawner callback

TSharedRef< SDockTab >
FOdysseyPainterEditorTab::SpawnTab( const FSpawnTabArgs& iArgs )
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
