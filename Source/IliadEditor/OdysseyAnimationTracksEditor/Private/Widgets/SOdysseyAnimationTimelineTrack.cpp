// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Widgets/SOdysseyAnimationTimelineTrack.h"

#include "Widgets/Animation/Timeline/SOdysseyAnimationLayerStackTreeView.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "TrackEditors/SubTrackEditorBase.h"
#include "OdysseyAnimationTimelineSectionEditor.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationTimelineTrack.h"
#include "OdysseyViewportDrawingEditorEdMode.h"
#include "EditorModeManager.h"
#include "OdysseyViewportDrawingEditorToolkit.h"
#include "Widgets/Input/SCheckBox.h"
#include "OdysseyAnimationTimelineSection.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SLATE_IMPLEMENT_WIDGET(SOdysseyAnimationTimelineTrack)
void
SOdysseyAnimationTimelineTrack::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mLayerStack, EInvalidateWidgetReason::None)
    .OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda(
        [](SWidget& Widget)
        {
            static_cast<SOdysseyAnimationTimelineTrack&>(Widget).OnLayerStackChanged();
        }
    ));
}

SOdysseyAnimationTimelineTrack::SOdysseyAnimationTimelineTrack()
    : mLayerStack(*this, nullptr)
{

}

void
SOdysseyAnimationTimelineTrack::Construct(const FArguments& iArgs, UOdysseyAnimationTimelineTrack* iTrack, const FBuildColumnWidgetParams& iParams, TSharedPtr<ISequencer> iSequencer)
{
    mTrack = iTrack;
    mRow = iParams.TreeViewRow;
    mSequencer = iSequencer;
    mLayerStack.Assign(*this, iArgs._LayerStack);
    RebuildWidgets();
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

    UOdysseyAnimationLayerStack* layerStack = mLayerStack.Get();
    if (!layerStack)
        return;

    TSharedPtr<SWidget> widget = SNew(SOdysseyAnimationLayerStackTreeView)
        .LayerStack(layerStack)
        .ExternalScrollbar(SNew(SScrollBar));

    this->ChildSlot.AttachWidget(widget.ToSharedRef());
}

void
SOdysseyAnimationTimelineTrack::OnLayerStackChanged()
{
    RebuildWidgets();
}

#undef LOCTEXT_NAMESPACE
