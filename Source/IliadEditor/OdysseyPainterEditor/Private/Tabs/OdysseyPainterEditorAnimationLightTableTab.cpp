// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyPainterEditorAnimationLighttableTab.h"

#include "Widgets/Animation/SOdysseyLighttable.h"
#include "OdysseyAnimationLayerStack.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorAnimationSource.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"


const FName&
FOdysseyPainterEditorAnimationLighttableTab::StaticId()
{
    static FName Id = TEXT("OdysseyAnimationEditor_Lighttable"); //Dont change, Old Id for retro compatibility
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorAnimationLighttableTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorAnimationLighttableTab::~FOdysseyPainterEditorAnimationLighttableTab()
{
}

FOdysseyPainterEditorAnimationLighttableTab::FOdysseyPainterEditorAnimationLighttableTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab( LOCTEXT( "lighttable-tab.name", "Lighttable" )
    , FSlateIcon( "OdysseyStyle", "Animation.Lighttable16" ))
    , mEditor(iEditor)
{
}

const FName&
FOdysseyPainterEditorAnimationLighttableTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyPainterEditorAnimationLighttableTab::CreateWidget()
{
    return SNew(SOdysseyLighttable)
        .LayerStack(this, &FOdysseyPainterEditorAnimationLighttableTab::LayerStack);
}

bool
FOdysseyPainterEditorAnimationLighttableTab::CanOpen() const
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    return source && source->Id() == FOdysseyPainterEditorAnimationSource::StaticId();
}

UOdysseyAnimationLayerStack*
FOdysseyPainterEditorAnimationLighttableTab::LayerStack() const
{
    return Cast<UOdysseyAnimationLayerStack>(mEditor->LayerStack());
}

#undef LOCTEXT_NAMESPACE
