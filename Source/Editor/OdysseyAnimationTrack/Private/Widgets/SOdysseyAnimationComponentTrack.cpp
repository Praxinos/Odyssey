// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/SOdysseyAnimationComponentTrack.h"

#include "Widgets/LayerStack/SOdysseyAnimationLayerStackTreeView.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

void
SOdysseyAnimationComponentTrack::Construct(const FArguments& iArgs, UOdysseyAnimationComponent* iComponent)
{
	ensure(iComponent);
	mComponent = iComponent;

    RebuildWidgets();
}

void
SOdysseyAnimationComponentTrack::RebuildWidgets()
{
	this->ChildSlot.DetachWidget();

	UOdysseyAnimation* animation = mComponent->GetActiveAnimation();
	if (!animation)
		return;

	TSharedPtr<SWidget> widget = SNew(SOdysseyAnimationLayerStackTreeView)
		.LayerStack(animation->GetLayerStack())
		.ExternalScrollbar(SNew(SScrollBar));

	this->ChildSlot.AttachWidget(widget.ToSharedRef());
}

#undef LOCTEXT_NAMESPACE