// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationTrackEditorSection.h"

#include "Widgets/LayerStack/SOdysseyAnimationTimelineTreeView.h"
#include "ISequencer.h"
#include "OdysseyAnimationEditorTimelinePosition.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "OdysseyViewportDrawingEditorEdMode.h"
#include "EditorModeManager.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "OdysseyViewportDrawingEditorToolkit.h"
#include "Tools/OutOfPegsTool/OdysseyAnimationEditorOutOfPegsTool.h"

#define LOCTEXT_NAMESPACE "AnimationTrack"

FOdysseyAnimationTrackEditorSection::~FOdysseyAnimationTrackEditorSection()
{
}

FOdysseyAnimationTrackEditorSection::FOdysseyAnimationTrackEditorSection(TSharedPtr<ISequencer> InSequencer, UOdysseyAnimationComponentSection* InSection)
	: TSubSectionMixin(InSequencer, *InSection)
	, mSection(InSection)
	, mTimelinePosition(MakeShared<FOdysseyAnimationEditorTimelinePosition>())
{
	mTimelinePosition->SetPadding(0.f);
	mTimelinePosition->HasMinZoom(false);
	mTimelinePosition->HasMaxZoom(false);
}

float
FOdysseyAnimationTrackEditorSection::GetLayerHeight(UOdysseyLayer* iLayer) const
{
	float height = 0.f; //line padding

	TArray<FName> rows = iLayer->GetRows();
	for (const FName& row : rows)
	{
		if (!iLayer->IsRowVisible(row))
			continue;

		height += iLayer->GetRowHeight(row);
		FMargin padding = iLayer->GetRowPadding(row);
		height += padding.Top + padding.Bottom; //Padding between each subrow
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
FOdysseyAnimationTrackEditorSection::GetSectionHeight( const UE::Sequencer::FViewDensityInfo& ViewDensity ) const
{
	UOdysseyAnimationComponent* component = GetComponent();
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
FOdysseyAnimationTrackEditorSection::GetSectionGripHeight(float iSectionHeight) const
{
	UOdysseyAnimationComponent* component = GetComponent();
	if (!component)
		return TSubSectionMixin::GetSectionGripHeight(iSectionHeight);

	UOdysseyAnimation* animation = component->GetActiveAnimation();
	
	if (!animation)
		return TSubSectionMixin::GetSectionGripHeight(iSectionHeight);

	return 25.f;
}

FText
FOdysseyAnimationTrackEditorSection::GetSectionTitle() const
{
    return FText::GetEmpty();
}

FText
FOdysseyAnimationTrackEditorSection::GetSectionToolTip() const
{
	return FText::GetEmpty();
}

TSharedRef<SWidget>
FOdysseyAnimationTrackEditorSection::GenerateSectionWidget()
{	
	mSectionWidget = SNew(SBox);
	RebuildSectionWidget();
    return mSectionWidget.ToSharedRef();
}

void
FOdysseyAnimationTrackEditorSection::RebuildSectionWidget()
{
	UOdysseyAnimationComponent* component = GetComponent();
	if (!component)
		return;

	UOdysseyAnimation* animation = component->GetActiveAnimation();

	if (!animation)
	{
		mSectionWidget->SetContent(SNullWidget::NullWidget);
		return;
	}

	UOdysseyAnimationLayerStack* layerStack = animation->GetLayerStack();

	mSectionWidget->SetContent(
		SNew(SVerticalBox)
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
			.TimelinePosition(mTimelinePosition)
			.HeaderHeight(20.f)
			.OnActivateOutOfPegs_Lambda(
				[](UOdysseyAnimationCell* iCell)
				{
					if (!GLevelEditorModeTools().IsModeActive( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId ))
						return;

					FEdMode* edMode = GLevelEditorModeTools().GetActiveMode( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
					if (!edMode)
						return;

					FOdysseyViewportDrawingEditorEdMode* odysseyEdMode = static_cast<FOdysseyViewportDrawingEditorEdMode*>(edMode);
					TSharedPtr<FOdysseyPainterEditor> editor = odysseyEdMode->GetEditor();
					if (!editor)
						return;

					TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = odysseyEdMode->GetViewportDrawingEditorToolkit();
					if(!toolkit)
						return;

					UOdysseyAnimation* animation = iCell->GetAnimation();
					if (!animation)
						return;
					
					TSharedPtr<FOdysseyAnimationEditorExtension> animationExtension = toolkit->GetAnimationExtension();
					if (!animationExtension)
						return;

					if (animationExtension->Animation() != animation)
						return;

					animationExtension->GetOutOfPegsTool()->SetCell(iCell);
					editor->ActivateTemporaryTool(animationExtension->GetOutOfPegsTool());
				}
			)
			.OnInactivateOutOfPegs_Lambda(
				[]()
				{
					if (!GLevelEditorModeTools().IsModeActive( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId ))
						return;

					FEdMode* edMode = GLevelEditorModeTools().GetActiveMode( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
					if (!edMode)
						return;

					FOdysseyViewportDrawingEditorEdMode* odysseyEdMode = static_cast<FOdysseyViewportDrawingEditorEdMode*>(edMode);
					TSharedPtr<FOdysseyPainterEditor> editor = odysseyEdMode->GetEditor();
					if (!editor)
						return;

					TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = odysseyEdMode->GetViewportDrawingEditorToolkit();
					if(!toolkit)
						return;

					TSharedPtr<FOdysseyAnimationEditorExtension> animationExtension = toolkit->GetAnimationExtension();
					if (!animationExtension)
						return;

					editor->InactivateTemporaryTool();
				}
			)
			.OnIsOutOfPegsChecked_Lambda(
				[](UOdysseyAnimationCell* iCell)
				{
					if (!GLevelEditorModeTools().IsModeActive( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId))
						return ECheckBoxState::Unchecked;

					FEdMode* edMode = GLevelEditorModeTools().GetActiveMode( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
					if (!edMode)
						return ECheckBoxState::Unchecked;

					FOdysseyViewportDrawingEditorEdMode* odysseyEdMode = static_cast<FOdysseyViewportDrawingEditorEdMode*>(edMode);
					TSharedPtr<FOdysseyPainterEditor> editor = odysseyEdMode->GetEditor();
					if (!editor)
						return ECheckBoxState::Unchecked;

					TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = odysseyEdMode->GetViewportDrawingEditorToolkit();
					if(!toolkit)
						return ECheckBoxState::Unchecked;

					UOdysseyAnimation* animation = iCell->GetAnimation();
					if (!animation)
						return ECheckBoxState::Unchecked;
					
					TSharedPtr<FOdysseyAnimationEditorExtension> animationExtension = toolkit->GetAnimationExtension();
					if (!animationExtension)
						return ECheckBoxState::Unchecked;

					if (animationExtension->Animation() != animation)
						return ECheckBoxState::Unchecked;

					UOdysseyPainterEditorTool* tool = editor->GetCurrentTool();
					if (!tool)
						return ECheckBoxState::Unchecked;

					bool isToolActive = tool->IsA(UOdysseyAnimationEditorOutOfPegsTool::StaticClass());
					if (!isToolActive)
						return ECheckBoxState::Unchecked;

					UOdysseyAnimationEditorOutOfPegsTool* outOfPegsTool = Cast<UOdysseyAnimationEditorOutOfPegsTool>(tool);
					if (outOfPegsTool->GetCell() != iCell)
						return ECheckBoxState::Unchecked;
					
					return ECheckBoxState::Checked;
				}
			)
			.ExternalScrollbar( SNew(SScrollBar) )
		]
	);
}

void
FOdysseyAnimationTrackEditorSection::Tick( const FGeometry& AllottedGeometry, const FGeometry& ClippedGeometry, const double InCurrentTime, const float InDeltaTime )
{
	UOdysseyAnimationComponentTrack* track = mSection->GetTypedOuter<UOdysseyAnimationComponentTrack>();
	UMovieScene* movieScene = track->GetTypedOuter<UMovieScene>();

	UOdysseyAnimationComponent* component = GetComponent();
	if (!component)
		return;

	UOdysseyAnimation* animation = component->GetActiveAnimation();
	if (!animation)
		return;

	float animationFramesPerSecond = animation->GetFramesPerSecond();

	FMovieSceneFrameRange sectionRange = mSection->SectionRange;
	FFrameNumber sectionFrameLength = sectionRange.Value.GetUpperBoundValue() - sectionRange.Value.GetLowerBoundValue();
	double sectionSecondLength = movieScene->GetTickResolution().AsSeconds(sectionFrameLength);

	double animationSecondInPixels = (animationFramesPerSecond * mTimelinePosition->GetBaseFrameSize());
	double sequencerSecondInPixels = AllottedGeometry.Size.X / sectionSecondLength;

	double zoom = sequencerSecondInPixels / animationSecondInPixels;
	mTimelinePosition->SetZoom(zoom);


	double offset = movieScene->GetTickResolution().AsSeconds(mSection->StartFrameOffset) * animationFramesPerSecond;
	mTimelinePosition->SetOffset(offset);
}

void
FOdysseyAnimationTrackEditorSection::OnSectionChanged()
{
	RebuildSectionWidget();
}

UOdysseyAnimationComponent*
FOdysseyAnimationTrackEditorSection::GetComponent() const
{
	UOdysseyAnimationComponentTrack* track = mSection->GetTypedOuter<UOdysseyAnimationComponentTrack>();

	TSharedPtr<ISequencer> sequencer = GetSequencer();
	if (!sequencer)
		return nullptr;

	TArrayView<TWeakObjectPtr<>> boundObjects = sequencer->FindObjectsInCurrentSequence(track->FindObjectBindingGuid());
	for (TWeakObjectPtr<>& boundObjectPtr : boundObjects)
	{
		UObject* boundObject = boundObjectPtr.Get();
		if (!boundObject)
			continue;

		if (!boundObject->IsA<UOdysseyAnimationComponent>())
			continue;

		UOdysseyAnimationComponent* animationComponent = Cast<UOdysseyAnimationComponent>(boundObject);
		if (!animationComponent)
			continue;

		return animationComponent;
	}

	return nullptr;
}

void
FOdysseyAnimationTrackEditorSection::BeginResizeSection()
{
	mInitialStartOffsetDuringResize = mSection->StartFrameOffset;
	mInitialStartTimeDuringResize = mSection->HasStartFrame() ? mSection->GetInclusiveStartFrame() : 0;

	TSubSectionMixin::BeginResizeSection();
}

void
FOdysseyAnimationTrackEditorSection::ResizeSection(ESequencerSectionResizeMode iResizeMode, FFrameNumber iResizeTime)
{
	if (iResizeMode == SSRM_LeadingEdge)
	{
		FFrameNumber mStartOffset = iResizeTime - mInitialStartTimeDuringResize;
		mStartOffset += mInitialStartOffsetDuringResize;

		// Ensure start offset is not less than 0
		if (mStartOffset < 0)
		{
			iResizeTime = iResizeTime - mStartOffset;
			mStartOffset = FFrameNumber(0);
		}

		mSection->StartFrameOffset = mStartOffset;
	}

	TSubSectionMixin::ResizeSection(iResizeMode, iResizeTime);
}

#undef LOCTEXT_NAMESPACE