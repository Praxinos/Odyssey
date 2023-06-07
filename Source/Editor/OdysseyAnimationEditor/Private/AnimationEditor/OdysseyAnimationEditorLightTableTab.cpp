// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditorLightTableTab.h"

#include "Widgets/SOdysseyAnimationLightTable.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationEditorLightTableTab"

/////////////////////////////////////////////////////
// FOdysseyAnimationEditorLightTableTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyAnimationEditorLightTableTab::~FOdysseyAnimationEditorLightTableTab()
{
}

FOdysseyAnimationEditorLightTableTab::FOdysseyAnimationEditorLightTableTab(FOdysseyAnimationEditor* iEditor)
	: FOdysseyEditorTab(TEXT("OdysseyAnimationEditor_LightTable")
    , LOCTEXT( "OdysseyAnimationEditorLightTableTab", "LightTable" )
    , FSlateIcon( "OdysseyStyle", "Animation.LightTable16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyAnimationEditorTab interface

TSharedPtr<SWidget>
FOdysseyAnimationEditorLightTableTab::CreateWidget()
{
    return SNew(SOdysseyAnimationLightTable, mEditor);
}

#undef LOCTEXT_NAMESPACE
