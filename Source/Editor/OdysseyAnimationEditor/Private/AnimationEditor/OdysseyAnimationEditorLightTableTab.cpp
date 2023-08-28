// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditorLightTableTab.h"

#include "Widgets/SOdysseyAnimationLightTable.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationEditorLightTableTab"


const FName&
FOdysseyAnimationEditorLightTableTab::StaticId()
{
    static FName Id = TEXT("OdysseyAnimationEditor_LightTable");
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyAnimationEditorLightTableTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyAnimationEditorLightTableTab::~FOdysseyAnimationEditorLightTableTab()
{
}

FOdysseyAnimationEditorLightTableTab::FOdysseyAnimationEditorLightTableTab(FOdysseyAnimationEditorExtension* iExtension)
	: FOdysseyEditorTab( LOCTEXT( "OdysseyAnimationEditorLightTableTab", "LightTable" )
    , FSlateIcon( "OdysseyStyle", "Animation.LightTable16" ))
    , mExtension(iExtension)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyAnimationEditorTab interface

const FName&
FOdysseyAnimationEditorLightTableTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyAnimationEditorLightTableTab::CreateWidget()
{
    return SNew(SOdysseyAnimationLightTable, mExtension)
        .LayerStack(this, &FOdysseyAnimationEditorLightTableTab::LayerStack);
}

UOdysseyAnimationLayerStack*
FOdysseyAnimationEditorLightTableTab::LayerStack() const
{
    return mExtension->LayerStack();
}

#undef LOCTEXT_NAMESPACE
