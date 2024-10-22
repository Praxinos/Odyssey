// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationComponentTrackEditorSection.h"

#include "Widgets/LayerStack/SOdysseyAnimationTimelineTreeView.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationComponentTrackEditorSection::~FOdysseyAnimationComponentTrackEditorSection()
{}

FOdysseyAnimationComponentTrackEditorSection::FOdysseyAnimationComponentTrackEditorSection(TSharedPtr<ISequencer> InSequencer, UOdysseyAnimationComponentSection* InSection)
	: TSubSectionMixin(InSequencer, *InSection)
	, mSection(InSection)
	, mTimelinePosition(MakeShared<FOdysseyAnimationEditorTimelinePosition>())
{
}

float
FOdysseyAnimationComponentTrackEditorSection::GetSectionHeight( const UE::Sequencer::FViewDensityInfo& ViewDensity ) const
{
	UOdysseyAnimationComponentTrack* track = mSection->GetTypedOuter<UOdysseyAnimationComponentTrack>();
	UOdysseyAnimationComponent* component = track->Component;
	if (!component)
		return TSubSectionMixin::GetSectionHeight(ViewDensity);

	UOdysseyAnimation* animation = component->GetActiveAnimation();
	if (!animation)
		return TSubSectionMixin::GetSectionHeight(ViewDensity);
		
	UOdysseyAnimationLayerStack* layerStack = animation->GetLayerStack();
	TArray<UOdysseyLayer*> layers = layerStack->GetLayers();
	int layersHeight = 2.f; //Initial treeview padding
	for (UOdysseyLayer* layer : layers)
	{
		TArray<FName> rows = layer->GetRows();
		for (const FName& row : rows)
		{
			layersHeight += layer->GetRowHeight(row).Get();
			layersHeight += 1.f; //Padding between each line
		}
	}

	return 25.f //Add Button and Timeline tools row
		+ 20.f //headerRow
		+ layersHeight;
}

FText
FOdysseyAnimationComponentTrackEditorSection::GetSectionTitle() const
{
    return FText::GetEmpty();
}

FText
FOdysseyAnimationComponentTrackEditorSection::GetSectionToolTip() const
{
	return FText::GetEmpty();
}

TSharedRef<SWidget>
FOdysseyAnimationComponentTrackEditorSection::GenerateSectionWidget()
{	
	UOdysseyAnimationComponentTrack* track = mSection->GetTypedOuter<UOdysseyAnimationComponentTrack>();

	UOdysseyAnimationComponent* component = track->Component;
	if (!component)
		return SNullWidget::NullWidget;

	UOdysseyAnimation* animation = component->GetActiveAnimation();
	if (!animation)
		return SNullWidget::NullWidget;

	UOdysseyAnimationPlayer* player = component->GetActivePlayer();
	if (!player)
		return SNullWidget::NullWidget;

	UOdysseyAnimationLayerStack* layerStack = animation->GetLayerStack();

    return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBox)
			.HeightOverride(25.f)
			[
				SNullWidget::NullWidget
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew( SOdysseyAnimationTimelineTreeView )
			.LayerStack(layerStack)
			.Player(player)
			.TimelinePosition(mTimelinePosition)
			.HeaderHeight(20.f)
			/* SLATE_EVENT(SOdysseyAnimationTimelineOutOfPegsKey::FOnActivateOutOfPegs, OnActivateOutOfPegs)
			SLATE_EVENT(FSimpleDelegate, OnInactivateOutOfPegs)
			SLATE_EVENT(SOdysseyAnimationTimelineOutOfPegsKey::FOnIsOutOfPegsChecked, OnIsOutOfPegsChecked) */
			.ExternalScrollbar( SNew(SScrollBar) )
		];
}

#undef LOCTEXT_NAMESPACE