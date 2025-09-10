// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorAnimationDetailsTab.h"

#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorAnimationSource.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
#include "Widgets/Animation/SOdysseyAnimationDetails.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

const FName&
FOdysseyPainterEditorAnimationDetailsTab::StaticId()
{
    static FName Id = TEXT("OdysseyAnimationEditor_AnimationDetails"); //Dont change, Old Id for retro compatibility
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorAnimationDetailsTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorAnimationDetailsTab::~FOdysseyPainterEditorAnimationDetailsTab()
{
}

FOdysseyPainterEditorAnimationDetailsTab::FOdysseyPainterEditorAnimationDetailsTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab( LOCTEXT( "animation-details-tab.name", "Animation Details" ), FSlateIcon( "OdysseyStyle", "PainterEditor.DetailsTab" ))
    , mEditor(iEditor)
{
}

const FName&
FOdysseyPainterEditorAnimationDetailsTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyPainterEditorAnimationDetailsTab::CreateWidget()
{
    return SNew( SOdysseyAnimationDetails )
        .Animation( this, &FOdysseyPainterEditorAnimationDetailsTab::Animation );
}

bool
FOdysseyPainterEditorAnimationDetailsTab::CanOpen() const
{
    //Always display this tab if we use the Odyssey Ed Mode
    if (!mEditor->GetToolkit()->IsAssetEditor())
        return true;

    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    return source && source->Id() == FOdysseyPainterEditorAnimationSource::StaticId();
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

UOdysseyAnimation*
FOdysseyPainterEditorAnimationDetailsTab::Animation() const
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
    if (!source || source->Id() != FOdysseyPainterEditorAnimationSource::StaticId())
        return nullptr;

    TSharedPtr<FOdysseyPainterEditorAnimationSource> animationSource = StaticCastSharedPtr<FOdysseyPainterEditorAnimationSource>(source);

    return animationSource->GetAnimation();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

#undef LOCTEXT_NAMESPACE
