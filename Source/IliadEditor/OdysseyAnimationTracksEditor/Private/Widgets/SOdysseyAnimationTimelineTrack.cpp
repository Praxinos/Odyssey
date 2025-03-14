// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Widgets/SOdysseyAnimationTimelineTrack.h"

#include "Widgets/Animation/Timeline/SOdysseyAnimationLayerStackTreeView.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "TrackEditors/SubTrackEditorBase.h"
#include "OdysseyAnimationTimelineSectionEditor.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationTimelineTrack.h"
#include "OdysseyViewportDrawingEditorEdMode.h"
#include "EditorModeManager.h"
#include "OdysseyViewportDrawingEditorToolkit.h"
#include "Widgets/Input/SCheckBox.h"
#include "OdysseyAnimationTimelineSection.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

void
SOdysseyAnimationTimelineTrack::Construct(const FArguments& iArgs, UOdysseyAnimationComponent* iComponent, UOdysseyAnimationTimelineTrack* iTrack, const FBuildColumnWidgetParams& iParams, TSharedPtr<ISequencer> iSequencer)
{
    ensure(iComponent);
    mComponent = iComponent;
    mTrack = iTrack;
    mRow = iParams.TreeViewRow;
    mSequencer = iSequencer;
    RebuildWidgets();

    mComponent->OnAnimationChanged().AddSP(this, &SOdysseyAnimationTimelineTrack::OnAnimationChanged);
    mComponent->OnPlayerChanged().AddSP(this, &SOdysseyAnimationTimelineTrack::OnPlayerChanged);
    mComponent->OnModeChanged().AddSP(this, &SOdysseyAnimationTimelineTrack::OnModeChanged);
}

FReply
SOdysseyAnimationTimelineTrack::OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    TSharedPtr<ISequencer> sequencer = mSequencer.Pin();
    if (!sequencer)
        return SCompoundWidget::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);

    TArray<UMovieSceneTrack*> selectedTracks;
    sequencer->GetSelectedTracks(selectedTracks);

    TArray<UMovieSceneSection*> selectedSections;
    sequencer->GetSelectedSections(selectedSections);

    bool sectionSelected = selectedSections.ContainsByPredicate(
        [this](UMovieSceneSection* iSection)
        {
            return iSection->GetTypedOuter<UOdysseyAnimationTimelineTrack>() == mTrack;
        }
    );
    if ((selectedTracks.Num() == 1 && selectedTracks.Contains(mTrack)) || (selectedSections.Num() == 1 && sectionSelected))
        return SCompoundWidget::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);

    sequencer->EmptySelection();
    sequencer->SelectTrack(mTrack);
    return SCompoundWidget::OnPreviewMouseButtonDown(MyGeometry, MouseEvent);
}

void
SOdysseyAnimationTimelineTrack::RebuildWidgets()
{
    this->ChildSlot.DetachWidget();

    UOdysseyAnimation* animation = mComponent->GetActiveAnimation();
    if (!animation)
        return;

    TSharedPtr<SWidget> widget = SNew(SOdysseyAnimationLayerStackTreeView)
        .PainterEditor_Lambda(
            [animation]() -> FOdysseyPainterEditor*
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

                return editor;
            }
        )
        .LayerStack(animation->GetLayerStack())
        .ExternalScrollbar(SNew(SScrollBar));

    this->ChildSlot.AttachWidget(widget.ToSharedRef());
}

void
SOdysseyAnimationTimelineTrack::OnAnimationChanged()
{
    RebuildWidgets();
}

void
SOdysseyAnimationTimelineTrack::OnPlayerChanged()
{
    RebuildWidgets();
}

void
SOdysseyAnimationTimelineTrack::OnModeChanged()
{
    RebuildWidgets();
}

#undef LOCTEXT_NAMESPACE
