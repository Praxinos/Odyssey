// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/SOdysseyAnimationComponentTrack.h"

#include "Widgets/LayerStack/SOdysseyAnimationLayerStackTreeView.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "TrackEditors/SubTrackEditorBase.h"
#include "OdysseyAnimationTrackEditorSection.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

void
SOdysseyAnimationComponentTrack::Construct(const FArguments& iArgs, UOdysseyAnimationComponent* iComponent, UOdysseyAnimationComponentTrack* iTrack, const FBuildColumnWidgetParams& iParams)
{
	ensure(iComponent);
	mComponent = iComponent;
	mTrack = iTrack;
	mRow = iParams.TreeViewRow;
    RebuildWidgets();
}

void
SOdysseyAnimationComponentTrack::RebuildWidgets()
{
	this->ChildSlot.DetachWidget();

	UOdysseyAnimation* animation = mComponent->GetActiveAnimation();
	if (!animation)
		return;

	const FCheckBoxStyle* displayLayersToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("Sequencer.AnimationComponentTrack.DisplayLayersToggle");

	TSharedPtr<SWidget> widget = SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBox)
			.HeightOverride(FOdysseyAnimationTrackEditorSection::GetCollapsedSectionHeight())
			.VAlign(VAlign_Center)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.Padding(TAttribute<FMargin>(this, &SOdysseyAnimationComponentTrack::GetDisplayLayersPadding))
				.VAlign(VAlign_Center)
				.AutoWidth()
				[
					SNew(SCheckBox)
					.Style(displayLayersToggleStyle)
					.OnCheckStateChanged(this, &SOdysseyAnimationComponentTrack::OnDisplayLayersCheckBoxStateChanged)
					.IsChecked(this, &SOdysseyAnimationComponentTrack::GetDisplayLayersCheckBoxState)
				]
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("sequencer.animation-timeline-track.name", "Timeline"))
				]
			]
		]
		+ SVerticalBox::Slot()
		[
			SNew(SOdysseyAnimationLayerStackTreeView)
			.Visibility(this, &SOdysseyAnimationComponentTrack::GetLayersVisibility)
			.LayerStack(animation->GetLayerStack())
			.ExternalScrollbar(SNew(SScrollBar))
		];

	this->ChildSlot.AttachWidget(widget.ToSharedRef());
}

void
SOdysseyAnimationComponentTrack::OnDisplayLayersCheckBoxStateChanged(ECheckBoxState iState)
{
	FOdysseyObjectEditorUtils::SetPropertyValue(mTrack, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationComponentTrack, DisplayLayers), iState == ECheckBoxState::Checked);
}

ECheckBoxState
SOdysseyAnimationComponentTrack::GetDisplayLayersCheckBoxState() const
{
	return mTrack->DisplayLayers ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

EVisibility
SOdysseyAnimationComponentTrack::GetLayersVisibility() const
{
	return mTrack->DisplayLayers ? EVisibility::Visible : EVisibility::Collapsed;
}					

FMargin
SOdysseyAnimationComponentTrack::GetDisplayLayersPadding() const
{
	const int32 NestingDepth = FMath::Max(0, mRow->GetIndentLevel());
	const float Indent = 10.f;
	return FMargin( NestingDepth * Indent, 0.f, 2.f, 0.f );
}

#undef LOCTEXT_NAMESPACE