// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationComponentTrackEditorSection.h"

#include "Widgets/LayerStack/SOdysseyAnimationTimelineTreeView.h"
#include "ISequencer.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationComponentTrackEditorSection::~FOdysseyAnimationComponentTrackEditorSection()
{}

FOdysseyAnimationComponentTrackEditorSection::FOdysseyAnimationComponentTrackEditorSection(TSharedPtr<ISequencer> InSequencer, UOdysseyAnimationComponentSection* InSection)
	: TSubSectionMixin(InSequencer, *InSection)
	, mSection(InSection)
	, mTimelinePosition(MakeShared<FOdysseyAnimationEditorTimelinePosition>())
{
	mTimelinePosition->SetPadding(0.f);
	mTimelinePosition->HasMinZoom(false);
	mTimelinePosition->HasMaxZoom(false);
}

float
FOdysseyAnimationComponentTrackEditorSection::GetLayerHeight(UOdysseyLayer* iLayer) const
{
	float height = 0.f; //line padding

	TArray<FName> rows = iLayer->GetRows();
	for (const FName& row : rows)
	{
		height += iLayer->GetRowHeight(row).Get();
		height += 1.f; //Padding between each subrow
	}

	if (iLayer->DisplayChildren)
	{
		for (UOdysseyLayer* child : iLayer->Children)
		{
			height += GetLayerHeight(child);
		}	
	}

	return height;
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
	TArray<UOdysseyLayer*> layers = layerStack->GetRootLayers();
	int layersHeight = 0.f; //Initial treeview padding
	for (UOdysseyLayer* layer : layers)
	{
		layersHeight += GetLayerHeight(layer);
	}

	return 25.f //Add Button and Timeline tools row
		+ 22.f //headerRow
		+ layersHeight;
}

float
FOdysseyAnimationComponentTrackEditorSection::GetSectionGripHeight(float iSectionHeight) const
{
	return 25.f;
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

void
FOdysseyAnimationComponentTrackEditorSection::Tick( const FGeometry& AllottedGeometry, const FGeometry& ClippedGeometry, const double InCurrentTime, const float InDeltaTime )
{
	UOdysseyAnimationComponentTrack* track = mSection->GetTypedOuter<UOdysseyAnimationComponentTrack>();
	UMovieScene* movieScene = track->GetTypedOuter<UMovieScene>();

	UOdysseyAnimationComponent* component = track->Component;
	if (!component)
		return;

	UOdysseyAnimation* animation = component->GetActiveAnimation();
	if (!animation)
		return;

	//FFrameNumber sequencerFramesPerSecond = movieScene->GetTickResolution().AsFrameNumber(1.0f);
	float animationFramesPerSecond = animation->GetFramesPerSecond();

	FMovieSceneFrameRange sectionRange = mSection->SectionRange;
	FFrameNumber sectionFrameLength = sectionRange.Value.GetUpperBoundValue() - sectionRange.Value.GetLowerBoundValue();
	double sectionSecondLength = movieScene->GetTickResolution().AsSeconds(sectionFrameLength);

	double animationSecondInPixels = (animationFramesPerSecond * mTimelinePosition->GetBaseFrameSize());
	double sequencerSecondInPixels = AllottedGeometry.Size.X / sectionSecondLength;

	double zoom = sequencerSecondInPixels / animationSecondInPixels;
	mTimelinePosition->SetZoom(zoom);
}
#undef LOCTEXT_NAMESPACE