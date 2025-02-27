// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Widgets/SOdysseyAnimationTimelineSection.h"

#include "EditorModeManager.h"
#include "ISequencer.h"
#include "MovieScene.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationCell.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorAnimationOutOfPegsTool.h"
#include "OdysseyPainterEditorAnimationTimelinePosition.h"
#include "OdysseyAnimationTimelineSection.h"
#include "OdysseyAnimationTimelineTrack.h"
#include "OdysseyViewportDrawingEditorEdMode.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineTreeView.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationTimelineSection::SOdysseyAnimationTimelineSection()
    : mTimelinePosition( MakeShared<FOdysseyPainterEditorAnimationTimelinePosition>() )
{
    mTimelinePosition->SetPadding(0.f);
    mTimelinePosition->HasMinZoom(false);
    mTimelinePosition->HasMaxZoom(false);
}

void
SOdysseyAnimationTimelineSection::Construct(const FArguments& iArgs, TSharedPtr<ISequencer> iSequencer, UOdysseyAnimationTimelineSection* iSection, UOdysseyAnimationComponent* iComponent)
{
    ensure(iComponent);
    mComponent = iComponent;
    mSection = iSection;
    mSequencer = iSequencer;
    RebuildWidgets();

    mComponent->OnAnimationChanged().AddSP(this, &SOdysseyAnimationTimelineSection::OnAnimationChanged);
    mComponent->OnPlayerChanged().AddSP(this, &SOdysseyAnimationTimelineSection::OnPlayerChanged);
    mComponent->OnModeChanged().AddSP(this, &SOdysseyAnimationTimelineSection::OnModeChanged);
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

void
SOdysseyAnimationTimelineSection::RebuildWidgets()
{
    this->ChildSlot.DetachWidget();

    UOdysseyAnimation* animation = mComponent->GetActiveAnimation();
    if (!animation)
        return;

    TSharedPtr<SWidget> widget =
        SNew( SOdysseyAnimationTimelineTreeView )
        .PainterEditor_Lambda(
            [animation]() ->FOdysseyPainterEditor*
            {
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
        )
        .Visibility(this, &SOdysseyAnimationTimelineSection::GetLayersVisibility)
        .LayerStack(animation->GetLayerStack())
        .TimelinePosition(mTimelinePosition)
        .OnActivateOutOfPegs_Lambda(
            [](UOdysseyAnimationCell* iCell)
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
                FOdysseyPainterEditor* editor = odysseyEdMode->GetEditor();
                if (!editor)
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
        )
        .ExternalScrollbar( SNew(SScrollBar) );

    this->ChildSlot.AttachWidget(widget.ToSharedRef());
}

void
SOdysseyAnimationTimelineSection::OnAnimationChanged()
{
    RebuildWidgets();
}

void
SOdysseyAnimationTimelineSection::OnPlayerChanged()
{
    RebuildWidgets();
}

void
SOdysseyAnimationTimelineSection::OnModeChanged()
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

    UOdysseyAnimation* animation = mComponent->GetActiveAnimation();
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

EVisibility
SOdysseyAnimationTimelineSection::GetLayersVisibility() const
{
    UOdysseyAnimationTimelineTrack* track = mSection->GetTypedOuter<UOdysseyAnimationTimelineTrack>();
    if (!track)
        return EVisibility::Collapsed;

    return track->DisplayLayers ? EVisibility::Visible : EVisibility::Collapsed;
}

#undef LOCTEXT_NAMESPACE
