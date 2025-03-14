// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyEditorContextMenu.h"

/////////////////////////////////////////////////////
// FOdysseyEditorContextMenu
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyEditorContextMenu::~FOdysseyEditorContextMenu()
{
}

FOdysseyEditorContextMenu::FOdysseyEditorContextMenu(FName iID, FText iDisplayName, FSlateIcon iIcon)
    : mMenu(true, NULL)
    , mID(iID)
    , mDisplayName(iDisplayName)
    , mIcon(iIcon)
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
