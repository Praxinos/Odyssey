// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "FOdysseyViewportDrawingEditorModeToolbar.h"
#include "OdysseyViewportDrawingEditorGUI.h"

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorModeToolbar"


/////////////////////////////////////////////////////
// FOdysseyViewportDrawingEditorModeToolbar
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyViewportDrawingEditorModeToolbar::FOdysseyViewportDrawingEditorModeToolbar( TSharedRef<FTabManager> iTabManager, FOdysseyViewportDrawingEditorGUI* iGUI )
    : mTabManager( iTabManager )
    , mGUI( iGUI )
{
    TabState tabState;
    tabState.mName = mGUI->GetLayerStackTab()->ID();
    tabState.bIsOpen = false;
    mTabStates.Add( tabState );
}


FOdysseyViewportDrawingEditorModeToolbar::~FOdysseyViewportDrawingEditorModeToolbar()
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Getter / Setter


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Callbacks / Toogle


void FOdysseyViewportDrawingEditorModeToolbar::ToggleLayerStackTab()
{
    if( mTabStates[0].bIsOpen )
    {
        TSharedPtr< SDockTab > tab = mTabManager->FindExistingLiveTab(FTabId(mGUI->GetLayerStackTab()->ID()));

        if (tab.IsValid())
            tab->RequestCloseTab();
    }
    else
    { 
        mTabManager->TryInvokeTab(FTabId(mGUI->GetLayerStackTab()->ID()));
    }
    mTabStates[0].bIsOpen = !mTabStates[0].bIsOpen;
}

#undef LOCTEXT_NAMESPACE
