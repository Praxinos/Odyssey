// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyPainterEditorAnimationLightTableTab.h"

#include "Widgets/Animation/SOdysseyAnimationLightTable.h"
#include "OdysseyAnimationLayerStack.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"


const FName&
FOdysseyPainterEditorAnimationLightTableTab::StaticId()
{
    static FName Id = TEXT("OdysseyAnimationEditor_LightTable"); //Dont change, Old Id for retro compatibility
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorAnimationLightTableTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorAnimationLightTableTab::~FOdysseyPainterEditorAnimationLightTableTab()
{
}

FOdysseyPainterEditorAnimationLightTableTab::FOdysseyPainterEditorAnimationLightTableTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab( LOCTEXT( "lighttable-tab.name", "LightTable" )
    , FSlateIcon( "OdysseyStyle", "Animation.LightTable16" ))
    , mEditor(iEditor)
{
}

const FName&
FOdysseyPainterEditorAnimationLightTableTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyPainterEditorAnimationLightTableTab::CreateWidget()
{
    return SNew(SOdysseyAnimationLightTable)
        .LayerStack(this, &FOdysseyPainterEditorAnimationLightTableTab::LayerStack);
}

UOdysseyAnimationLayerStack*
FOdysseyPainterEditorAnimationLightTableTab::LayerStack() const
{
    return Cast<UOdysseyAnimationLayerStack>(mEditor->LayerStack());
}

#undef LOCTEXT_NAMESPACE
