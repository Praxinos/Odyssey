// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "FOdysseyViewportDrawingEditorModeToolbar.h"
#include "OdysseyViewportDrawingEditorGUI.h"

#include "LevelEditor.h" 

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorModeToolbar"


/////////////////////////////////////////////////////
// FOdysseyViewportDrawingEditorModeToolbar
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyViewportDrawingEditorModeToolbar::FOdysseyViewportDrawingEditorModeToolbar( FOdysseyViewportDrawingEditorGUI* iGUI )
    : mGUI( iGUI )
{
    FLevelEditorModule& levelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(FName("LevelEditor"));
    mLevelEditorTabManager = levelEditorModule.GetLevelEditorTabManager();
}


FOdysseyViewportDrawingEditorModeToolbar::~FOdysseyViewportDrawingEditorModeToolbar()
{
    TSharedPtr< SDockTab > tab = mLevelEditorTabManager->FindExistingLiveTab(FTabId(mGUI->GetLayerStackTab()->ID()));

    if (tab.IsValid())
        tab->RequestCloseTab();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Getter / Setter


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Callbacks / Toogle


void FOdysseyViewportDrawingEditorModeToolbar::ToggleLayerStackTab()
{
    if( !mLevelEditorTabManager )
        return;

    mLevelEditorTabManager->TryInvokeTab(FTabId(mGUI->GetLayerStackTab()->ID()));
}

#undef LOCTEXT_NAMESPACE
