// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/SOdysseyAnimationTimelineSection.h"

#include "EditorModeManager.h"
#include "ISequencer.h"
#include "ITimeSlider.h"
#include "MovieScene.h"
#include "MVVM/ViewModels/SequencerEditorViewModel.h"
#include "MVVM/ViewModels/TrackAreaViewModel.h"
#include "SEnumCombo.h"
#include "Widgets/Colors/SColorBlock.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationPlayer.h"
#include "OdysseyAnimationLayerStack.h"
#include "OdysseyLayerCell.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorAnimationOutOfPegsTool.h"
#include "OdysseyPainterEditorAnimationTimelinePosition.h"
#include "OdysseyAnimationTimelineSection.h"
#include "OdysseyAnimationTimelineTrack.h"
#include "OdysseyViewportDrawingEditorEdMode.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "SOdysseyAnimationTimelineTreeView.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SLATE_IMPLEMENT_WIDGET(SOdysseyAnimationTimelineSection)
void
SOdysseyAnimationTimelineSection::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mAnimation, EInvalidateWidgetReason::None)
    .OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda(
        [](SWidget& Widget)
        {
            static_cast<SOdysseyAnimationTimelineSection&>(Widget).OnAnimationChanged();
        }
    ));
}

SOdysseyAnimationTimelineSection::SOdysseyAnimationTimelineSection()
    : mAnimation(*this, nullptr)
    , mTimelinePosition( MakeShared<FOdysseyPainterEditorAnimationTimelinePosition>() )
{
    mTimelinePosition->SetPadding(0.f);
    mTimelinePosition->HasMinZoom(false);
    mTimelinePosition->HasMaxZoom(false);
}

void
SOdysseyAnimationTimelineSection::Construct(const FArguments& iArgs, TSharedPtr<ISequencer> iSequencer, UOdysseyAnimationComponent* iComponent, UOdysseyAnimationTimelineSection* iSection)
{
    mAnimation.Assign(*this, iArgs._Animation);
    mPreBehaviour = iArgs._PreBehaviour;
    mPostBehaviour = iArgs._PostBehaviour;
    mOnPreBehaviourChanged = iArgs._OnPreBehaviourChanged;
    mOnPostBehaviourChanged = iArgs._OnPostBehaviourChanged;
    mStartFrameOffset = iArgs._StartFrameOffset;

    mSequencer = iSequencer;
    mComponent = iComponent;
    mSection = iSection;
    RebuildWidgets();
}

FReply
SOdysseyAnimationTimelineSection::OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    TSharedPtr<ISequencer> sequencer = mSequencer.Pin();
    if (!sequencer)
        return SCompoundWidget::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);

    UOdysseyAnimationTimelineTrack* track = mSection->GetTypedOuter<UOdysseyAnimationTimelineTrack>();
    if (!track)
        return SCompoundWidget::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);

    TArray<UMovieSceneSection*> selectedSections;
    sequencer->GetSelectedSections(selectedSections);

    TArray<UMovieSceneTrack*> selectedTracks;
    sequencer->GetSelectedTracks(selectedTracks);
    if (selectedTracks.Num() == 1 && selectedTracks.Contains(track))
        return SCompoundWidget::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);

    FScopedTransaction Transaction(LOCTEXT("timeline-section.transaction.select-section", "Select Actors/Components"));
    sequencer->EmptySelection();
    sequencer->SelectTrack(track);
    sequencer->SelectSection(mSection);

    return SCompoundWidget::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);
}

void
SOdysseyAnimationTimelineSection::OnActivateOutOfPegs(UOdysseyLayerCell* iCell)
{
    if (!GLevelEditorModeTools().IsModeActive( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId ))
        return;

    FEdMode* edMode = GLevelEditorModeTools().GetActiveMode( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
    if (!edMode)
        return;

    FOdysseyViewportDrawingEditorEdMode* odysseyEdMode = static_cast<FOdysseyViewportDrawingEditorEdMode*>(edMode);
    FOdysseyPainterEditor* editor = odysseyEdMode->GetEditor();
    if (!editor)
        return;

    TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = odysseyEdMode->GetViewportDrawingEditorToolkit();
    if(!toolkit)
        return;

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(iCell->GetLayer());
    if (!layer)
        return;

    UOdysseyAnimation* animation = layer->GetAnimation();
    if (!animation)
        return;

    if (editor->GetAnimation() != animation)
        return;

    editor->GetOutOfPegsTool()->SetCell(iCell);
    editor->ActivateTemporaryTool(editor->GetOutOfPegsTool());
}

void
SOdysseyAnimationTimelineSection::OnInactivateOutOfPegs()
{
    if (!GLevelEditorModeTools().IsModeActive( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId ))
        return;

    FEdMode* edMode = GLevelEditorModeTools().GetActiveMode( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
    if (!edMode)
        return;

    FOdysseyViewportDrawingEditorEdMode* odysseyEdMode = static_cast<FOdysseyViewportDrawingEditorEdMode*>(edMode);
    FOdysseyPainterEditor* editor = odysseyEdMode->GetEditor();
    if (!editor)
        return;

    editor->InactivateTemporaryTool();
}

ECheckBoxState
SOdysseyAnimationTimelineSection::OnIsOutOfPegsChecked(UOdysseyLayerCell* iCell)
{
    if (!GLevelEditorModeTools().IsModeActive( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId))
        return ECheckBoxState::Unchecked;

    FEdMode* edMode = GLevelEditorModeTools().GetActiveMode( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
    if (!edMode)
        return ECheckBoxState::Unchecked;

    FOdysseyViewportDrawingEditorEdMode* odysseyEdMode = static_cast<FOdysseyViewportDrawingEditorEdMode*>(edMode);
    FOdysseyPainterEditor* editor = odysseyEdMode->GetEditor();
    if (!editor)
        return ECheckBoxState::Unchecked;

    TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = odysseyEdMode->GetViewportDrawingEditorToolkit();
    if(!toolkit)
        return ECheckBoxState::Unchecked;

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(iCell->GetLayer());
    if (!layer)
        return ECheckBoxState::Unchecked;

    UOdysseyAnimation* animation = layer->GetAnimation();
    if (!animation)
        return ECheckBoxState::Unchecked;

    if (editor->GetAnimation() != animation)
        return ECheckBoxState::Unchecked;

    UOdysseyPainterEditorTool* tool = editor->GetCurrentTool();
    if (!tool)
        return ECheckBoxState::Unchecked;

    bool isToolActive = tool->IsA(UOdysseyPainterEditorAnimationOutOfPegsTool::StaticClass());
    if (!isToolActive)
        return ECheckBoxState::Unchecked;

    UOdysseyPainterEditorAnimationOutOfPegsTool* outOfPegsTool = Cast<UOdysseyPainterEditorAnimationOutOfPegsTool>(tool);
    if (outOfPegsTool->GetCell() != iCell)
        return ECheckBoxState::Unchecked;

    return ECheckBoxState::Checked;
}

void
SOdysseyAnimationTimelineSection::RebuildWidgets()
{
    this->ChildSlot.DetachWidget();

    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(animation->GetLayerStack());

    TSharedRef<SEnumComboBox> preBehaviourComboBoxWidget = SNew(SEnumComboBox, StaticEnum<EOdysseyAnimationPlayerPostBehaviour>())
        .Clipping(EWidgetClipping::ClipToBounds)
        .CurrentValue_Lambda(
            [this]() -> int32
            {
                return int32(mPreBehaviour.Get());
            }
        )
        .ContentPadding(FMargin(0))
        .OnEnumSelectionChanged(this, &SOdysseyAnimationTimelineSection::OnPrebehaviourComboBoxChanged);

    TSharedRef<SEnumComboBox> postBehaviourComboBoxWidget = SNew(SEnumComboBox, StaticEnum<EOdysseyAnimationPlayerPostBehaviour>())
        .Clipping(EWidgetClipping::ClipToBounds)
        .CurrentValue_Lambda(
            [this]()
            {
                return int32(mPostBehaviour.Get());
            }
        )
        .ContentPadding(FMargin(0))
        .OnEnumSelectionChanged(this, &SOdysseyAnimationTimelineSection::OnPostbehaviourComboBoxChanged);

    TSharedRef<SWidget> preBehaviourWidget = SNew(SBox)
        .Clipping(EWidgetClipping::ClipToBounds)
        .WidthOverride(this, &SOdysseyAnimationTimelineSection::GetPreBehaviourWidth)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                [
                    SNew(SImage)
                    .Image(this, &SOdysseyAnimationTimelineSection::GetPrePostBehaviourBrush)
                    .ColorAndOpacity(FLinearColor(0.25f, 0.25f, 0.25f, 1.f))
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    preBehaviourComboBoxWidget
                ]
            ]
            + SVerticalBox::Slot()
            [
                SNew(SColorBlock)
                .Color(FLinearColor(0, 0, 0, 0.75f))
            ]
        ];

    TSharedRef<SWidget> postBehaviourWidget = SNew(SBox)
        .Clipping(EWidgetClipping::ClipToBounds)
        .WidthOverride(this, &SOdysseyAnimationTimelineSection::GetPostBehaviourWidth)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    postBehaviourComboBoxWidget
                ]
                + SHorizontalBox::Slot()
                [
                    SNew(SImage)
                    .Image(this, &SOdysseyAnimationTimelineSection::GetPrePostBehaviourBrush)
                    .ColorAndOpacity(FLinearColor(0.25f, 0.25f, 0.25f, 1.f))
                ]
            ]
            + SVerticalBox::Slot()
            [
                SNew(SColorBlock)
                .Color(FLinearColor(0, 0, 0, 0.75f))
            ]
        ];

    TSharedRef<SWidget> timelineWidget = SNew( SOdysseyAnimationTimelineTreeView )
            .CurrentFrame(this, &SOdysseyAnimationTimelineSection::GetCurrentFrame)
            .LayerStack(layerStack)
            .TimelinePosition(mTimelinePosition)
            .OnActivateOutOfPegs(this, &SOdysseyAnimationTimelineSection::OnActivateOutOfPegs)
            .OnInactivateOutOfPegs(this, &SOdysseyAnimationTimelineSection::OnInactivateOutOfPegs)
            .OnIsOutOfPegsChecked(this, &SOdysseyAnimationTimelineSection::OnIsOutOfPegsChecked)
            .ExternalScrollbar( SNew(SScrollBar) );

    TSharedRef<SWidget> widget = SNew(SOverlay)
        .Visibility(this, &SOdysseyAnimationTimelineSection::GetLayersVisibility)
        + SOverlay::Slot()
        [
            timelineWidget
        ]
        + SOverlay::Slot()
        .HAlign(HAlign_Left)
        [
            preBehaviourWidget
        ]
        + SOverlay::Slot()
        .HAlign(HAlign_Right)
        [
            postBehaviourWidget
        ];



    /*
    TSharedRef<SWidget> widget = SNew(SHorizontalBox)
        .Visibility(this, &SOdysseyAnimationTimelineSection::GetLayersVisibility)
        .Clipping(EWidgetClipping::ClipToBounds)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            preBehaviourWidget
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            timelineWidget
        ]
        + SHorizontalBox::Slot()
        [
            postBehaviourWidget
        ];
    */

    this->ChildSlot.AttachWidget(widget);
}

void
SOdysseyAnimationTimelineSection::OnPrebehaviourComboBoxChanged(int32 iValue, ESelectInfo::Type iSelectInfo)
{
    mOnPreBehaviourChanged.ExecuteIfBound(EOdysseyAnimationPlayerPostBehaviour(iValue));
}

void
SOdysseyAnimationTimelineSection::OnPostbehaviourComboBoxChanged(int32 iValue, ESelectInfo::Type iSelectInfo)
{
    mOnPostBehaviourChanged.ExecuteIfBound(EOdysseyAnimationPlayerPostBehaviour(iValue));
}

void
SOdysseyAnimationTimelineSection::OnAnimationChanged()
{
    RebuildWidgets();
}

void
SOdysseyAnimationTimelineSection::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick( AllottedGeometry, InCurrentTime, InDeltaTime );

    UOdysseyAnimationTimelineTrack* track = mSection->GetTypedOuter<UOdysseyAnimationTimelineTrack>();
    if (!track)
        return;

    UMovieScene* movieScene = track->GetTypedOuter<UMovieScene>();
    if (!movieScene)
        return;

    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return;


    FMovieSceneFrameRange sectionRange = mSection->SectionRange;
    FFrameNumber sectionFrameLength = sectionRange.Value.GetUpperBoundValue() - sectionRange.Value.GetLowerBoundValue();
    double sectionSecondLength = movieScene->GetTickResolution().AsSeconds(sectionFrameLength);

    float animationFramesPerSecond = animation->GetFramesPerSecond();
    double animationSecondInPixels = (animationFramesPerSecond * mTimelinePosition->GetBaseFrameSize());
    double sequencerSecondInPixels = AllottedGeometry.Size.X / sectionSecondLength;
    double zoom = sequencerSecondInPixels / animationSecondInPixels;
    mTimelinePosition->SetZoom(zoom);

    FFrameRate animationFrameRate(animation->GetFramesPerSecond() * 100, 100);
    FFrameTime animationLeftBound = FFrameRate::TransformTime(animation->GetLeftBoundValue(), animationFrameRate, movieScene->GetTickResolution());
    FFrameTime startOffset = FFrameTime(mStartFrameOffset.Get());
    startOffset = FFrameRate::TransformTime(startOffset, movieScene->GetTickResolution(), animationFrameRate);
    float offset = startOffset.GetFrame().Value + startOffset.GetSubFrame();

    mTimelinePosition->SetOffset( offset );
}

EVisibility
SOdysseyAnimationTimelineSection::GetLayersVisibility() const
{
    UOdysseyAnimationTimelineTrack* track = mSection->GetTypedOuter<UOdysseyAnimationTimelineTrack>();
    if (!track)
        return EVisibility::Collapsed;

    return track->DisplayLayers ? EVisibility::Visible : EVisibility::Collapsed;
}

FOptionalSize
SOdysseyAnimationTimelineSection::GetPreBehaviourWidth() const
{
    TSharedPtr<ISequencer> sequencer = mSequencer.Pin();
    if (!sequencer)
        return 0.f;

    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return 0.f;

    TSharedPtr<UE::Sequencer::FSequencerEditorViewModel> editor_model = sequencer->GetViewModel();
    TSharedPtr<UE::Sequencer::FTrackAreaViewModel> track_model = editor_model->GetTrackArea();
    FGeometry geometry( sequencer->GetTopTimeSliderWidget()->GetTickSpaceGeometry() );
    FVector2f local_size = geometry.GetLocalSize();
    FTimeToPixel timeToPixel = track_model->GetTimeToPixel( local_size.X );

    FFrameRate animationFrameRate(animation->GetFramesPerSecond() * 100, 100);
    FFrameTime animationLeftBound = FFrameRate::TransformTime(animation->GetLeftBoundValue(), animationFrameRate, sequencer->GetFocusedTickResolution());

    return timeToPixel.FrameDeltaToPixel(animationLeftBound - mStartFrameOffset.Get());
}

FOptionalSize
SOdysseyAnimationTimelineSection::GetPostBehaviourWidth() const
{
    TSharedPtr<ISequencer> sequencer = mSequencer.Pin();
    if (!sequencer)
        return 0.f;

    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return 0.f;

    TSharedPtr<UE::Sequencer::FSequencerEditorViewModel> editor_model = sequencer->GetViewModel();
    TSharedPtr<UE::Sequencer::FTrackAreaViewModel> track_model = editor_model->GetTrackArea();
    FGeometry geometry( sequencer->GetTopTimeSliderWidget()->GetTickSpaceGeometry() );
    FVector2f local_size = geometry.GetLocalSize();
    FTimeToPixel timeToPixel = track_model->GetTimeToPixel( local_size.X );

    FFrameRate animationFrameRate(animation->GetFramesPerSecond() * 100, 100);
    FFrameTime animationStartFrame(animation->GetLeftBoundValue());
    FFrameTime animationEndFrame(animation->GetRightBoundValue() + 1);
    FFrameTime sectionDuration = mSection->GetRange().GetUpperBoundValue() - mSection->GetRange().GetLowerBoundValue();

    animationStartFrame = FFrameRate::TransformTime(animationStartFrame, animationFrameRate, sequencer->GetFocusedTickResolution());
    animationEndFrame = FFrameRate::TransformTime(animationEndFrame, animationFrameRate, sequencer->GetFocusedTickResolution());

    //FFrameTime postBehaviourDuration = sectionDuration - animationDuration - FFrameTime(mStartFrameOffset.Get());
    FFrameTime postBehaviourDuration = sectionDuration - animationEndFrame + FFrameTime(mStartFrameOffset.Get());
    postBehaviourDuration = FMath::Max(FFrameTime(0), postBehaviourDuration);
    return timeToPixel.FrameDeltaToPixel(postBehaviourDuration);
}



const FSlateBrush*
SOdysseyAnimationTimelineSection::GetPrePostBehaviourBrush() const
{
    return FOdysseyStyle::GetBrush("Sequencer.AnimationTimelineTrack.PrePostBehaviourOverlay");
}

int
SOdysseyAnimationTimelineSection::GetCurrentFrame() const
{
    if (!mComponent)
        return INDEX_NONE;

    if (mComponent->GetAnimation() != mAnimation.Get())
        return INDEX_NONE;

    UOdysseyAnimationPlayer* player = mComponent->GetPlayer();
    if (!player)
        return INDEX_NONE;

    return player->GetCurrentFrame().FrameNumber.Value;
}

#undef LOCTEXT_NAMESPACE
