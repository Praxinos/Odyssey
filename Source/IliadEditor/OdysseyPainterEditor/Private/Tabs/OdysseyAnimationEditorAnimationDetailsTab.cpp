// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyAnimationEditorAnimationDetailsTab.h"

#include "OdysseyAnimationEditorExtension.h"
#include "OdysseyAnimationEditorSource.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
#include "Widgets/Animation/SOdysseyAnimationDetails.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

const FName&
FOdysseyAnimationEditorAnimationDetailsTab::StaticId()
{
    static FName Id = TEXT("OdysseyAnimationEditor_AnimationDetails"); //Keep ColorSelector instead of ColorWheel because changing that ID would show an empty panel to users who already opened the previous ColorSelector Panel
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyAnimationEditorAnimationDetailsTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyAnimationEditorAnimationDetailsTab::~FOdysseyAnimationEditorAnimationDetailsTab()
{
}

FOdysseyAnimationEditorAnimationDetailsTab::FOdysseyAnimationEditorAnimationDetailsTab(FOdysseyAnimationEditorExtension* iExtension)
    : FOdysseyEditorTab( LOCTEXT( "animation-details-tab.name", "Animation Details" ), FSlateIcon( "OdysseyStyle", "PainterEditor.Trombone16" ))
    , mExtension(iExtension)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyAnimationEditorTab interface

const FName&
FOdysseyAnimationEditorAnimationDetailsTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyAnimationEditorAnimationDetailsTab::CreateWidget()
{
    return SNew( SOdysseyAnimationDetails )
        .Animation( this, &FOdysseyAnimationEditorAnimationDetailsTab::Animation );
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

UOdysseyAnimation*
FOdysseyAnimationEditorAnimationDetailsTab::Animation() const
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mExtension->GetEditor()->GetSource();
    if (!source || source->Id() != FOdysseyAnimationEditorSource::StaticId())
        return nullptr;

    TSharedPtr<FOdysseyAnimationEditorSource> animationSource = StaticCastSharedPtr<FOdysseyAnimationEditorSource>(source);

    return animationSource->GetAnimation();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

#undef LOCTEXT_NAMESPACE
