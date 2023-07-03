// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyEditorContextMenu.h"

/////////////////////////////////////////////////////
// FOdysseyEditorContextMenu
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyEditorContextMenu::~FOdysseyEditorContextMenu()
{
}

FOdysseyEditorContextMenu::FOdysseyEditorContextMenu(FName iID, FText iDisplayName, FSlateIcon iIcon)
	: mID(iID)
	, mDisplayName(iDisplayName)
	, mIcon(iIcon)
    , mMenu(true, NULL)
	, mWidget(nullptr)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyEditorContextMenu::Init()
{
	mWidget = CreateWidget();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FOdysseyEditorContextMenu::BindShortcuts(FBaseToolkit* iToolkit)
{
}
/*
void
FOdysseyEditorContextMenu::OnToolkitInitialized( FBaseToolkit* iToolkit )
{
}

void
FOdysseyEditorContextMenu::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
}
*/
//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

const FName&
FOdysseyEditorContextMenu::ID() const
{
	return mID;
}

const FText&
FOdysseyEditorContextMenu::DisplayName() const
{
	return mDisplayName;
}

const FSlateIcon&
FOdysseyEditorContextMenu::Icon() const
{
	return mIcon;
}

const TSharedPtr<SWidget>&
FOdysseyEditorContextMenu::Widget() const
{
	return mWidget;
}

#undef LOCTEXT_NAMESPACE
