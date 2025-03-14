// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Widgets/SOdysseyAnimationTimelineSection.h"

#include "EditorModeManager.h"
#include "ISequencer.h"
#include "ITimeSlider.h"
#include "MovieScene.h"
#include "MVVM/ViewModels/SequencerEditorViewModel.h"
#include "MVVM/ViewModels/TrackAreaViewModel.h"
#include "SEnumCombo.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationCell.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationPlayer.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorAnimationOutOfPegsTool.h"
#include "OdysseyPainterEditorAnimationTimelinePosition.h"
#include "OdysseyAnimationTimelineSection.h"
#include "OdysseyAnimationTimelineTrack.h"
#include "OdysseyViewportDrawingEditorEdMode.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineTreeView.h"

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
SOdysseyAnimationTimelineSection::Construct(const FArguments& iArgs, TSharedPtr<ISequencer> iSequencer, UOdysseyAnimationTimelineSection* iSection)
{
    mAnimation.Assign(*this, iArgs._Animation);
    mPreBehaviour = iArgs._PreBehaviour;
    mPostBehaviour = iArgs._PostBehaviour;
    mOnPreBehaviourChanged = iArgs._OnPreBehaviourChanged;
    mOnPostBehaviourChanged = iArgs._OnPostBehaviourChanged;
    mStartFrameOffset = iArgs._StartFrameOffset;

    mSection = iSection;
    mSequencer = iSequencer;
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
    if ((selectedTracks.Num() == 1 && selectedTracks.Contains(track)) || (selectedSections.Num() == 1 && selectedSections.Contains(mSection)))
        return SCompoundWidget::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);

    sequencer->EmptySelection();
    sequencer->SelectSection(mSection);
    return SCompoundWidget::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);
}

FOdysseyPainterEditor*
SOdysseyAnimationTimelineSection::GetPainterEditor() const
{
    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return nullptr;

    if (!GLevelEditorModeTools().IsModeActive( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId ))
        return nullptr;

    FEdMode* edMode = GLevelEditorModeTools().GetActiveMode( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId );
    if (!edMode)
        return nullptr;

    FOdysseyViewportDrawingEditorEdMode* odysseyEdMode = static_cast<FOdysseyViewportDrawingEditorEdMode*>(edMode);

    TSharedPtr<FOdysseyViewportDrawingEditorToolkit> toolkit = odysseyEdMode->GetViewportDrawingEditorToolkit();
    if(!toolkit)
        return nullptr;

    FOdysseyPainterEditor* editor = odysseyEdMode->GetEditor();
    if (!editor)
        return nullptr;

    if (editor->GetAnimation() != animation)
        return nullptr;

    return editor;
}

void
SOdysseyAnimationTimelineSection::OnActivateOutOfPegs(UOdysseyAnimationCell* iCell)
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

    UOdysseyAnimation* animation = iCell->GetAnimation();
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
SOdysseyAnimationTimelineSection::OnIsOutOfPegsChecked(UOdysseyAnimationCell* iCell)
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

    UOdysseyAnimation* animation = iCell->GetAnimation();
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

    TSharedPtr<SWidget> widget = SNew(SHorizontalBox)
        .Visibility(this, &SOdysseyAnimationTimelineSection::GetLayersVisibility)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SBox)
            .WidthOverride_Lambda(
                [this, animation]()
                {
                    TSharedPtr<ISequencer> sequencer = mSequencer.Pin();
                    if (!sequencer)
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
            )
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .VAlign(VAlign_Top)
                [
                    SNew(SEnumComboBox, StaticEnum<EOdysseyAnimationPlayerPostBehaviour>())
                    .CurrentValue_Lambda(
                        [this]() -> int32
                        {
                            return int32(mPreBehaviour.Get());
                        }
                    )
                    .ContentPadding(FMargin(0))
                    .OnEnumSelectionChanged(this, &SOdysseyAnimationTimelineSection::OnPrebehaviourComboBoxChanged)
                ]
                + SVerticalBox::Slot()
                [
                    SNullWidget::NullWidget
                ]
            ]
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SBox)
            .WidthOverride_Lambda(
                [this]()
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

                    animationStartFrame = FMath::Max(animationStartFrame, FFrameTime(mStartFrameOffset.Get()));
                    animationEndFrame = FMath::Min(animationEndFrame, sectionDuration + mStartFrameOffset.Get());

                    FFrameTime animationDuration = FMath::Max(FFrameTime(0), animationEndFrame - animationStartFrame);
                    return timeToPixel.FrameDeltaToPixel(animationDuration);
                }
            )
            [
                SNew( SOdysseyAnimationTimelineTreeView )
                .PainterEditor(this, &SOdysseyAnimationTimelineSection::GetPainterEditor)
                .LayerStack(animation->GetLayerStack())
                .TimelinePosition(mTimelinePosition)
                .OnActivateOutOfPegs(this, &SOdysseyAnimationTimelineSection::OnActivateOutOfPegs)
                .OnInactivateOutOfPegs(this, &SOdysseyAnimationTimelineSection::OnInactivateOutOfPegs)
                .OnIsOutOfPegsChecked(this, &SOdysseyAnimationTimelineSection::OnIsOutOfPegsChecked)
                .ExternalScrollbar( SNew(SScrollBar) )
            ]
        ]
        + SHorizontalBox::Slot()
        [
            SNew(SBox)
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .VAlign(VAlign_Top)
                [
                    SNew(SEnumComboBox, StaticEnum<EOdysseyAnimationPlayerPostBehaviour>())
                    .CurrentValue_Lambda(
                        [this]()
                        {
                            return int32(mPostBehaviour.Get());
                        }
                    )
                    .ContentPadding(FMargin(0))
                    .OnEnumSelectionChanged(this, &SOdysseyAnimationTimelineSection::OnPostbehaviourComboBoxChanged)
                ]
                + SVerticalBox::Slot()
                [
                    SNullWidget::NullWidget
                ]
            ]
        ];

    this->ChildSlot.AttachWidget(widget.ToSharedRef());
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
    FFrameTime startOffset = FMath::Max(FFrameTime(0), FFrameTime(mStartFrameOffset.Get()));
    startOffset = FFrameRate::TransformTime(startOffset, movieScene->GetTickResolution(), animationFrameRate);
    float offset = FMath::Max(animation->GetLeftBoundValue(), startOffset.GetFrame().Value + startOffset.GetSubFrame());

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

#undef LOCTEXT_NAMESPACE
