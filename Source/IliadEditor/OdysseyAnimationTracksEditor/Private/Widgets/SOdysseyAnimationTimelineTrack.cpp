// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Widgets/SOdysseyAnimationTimelineTrack.h"

#include "Widgets/Animation/Timeline/SOdysseyAnimationLayerStackTreeView.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "TrackEditors/SubTrackEditorBase.h"
#include "OdysseyAnimationTimelineSectionEditor.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyStyle.h"
#include "OdysseyAnimation.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyAnimationTimelineTrack.h"
#include "OdysseyViewportDrawingEditorEdMode.h"
#include "EditorModeManager.h"
#include "OdysseyViewportDrawingEditorToolkit.h"
#include "Widgets/Input/SCheckBox.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

void
SOdysseyAnimationTimelineTrack::Construct(const FArguments& iArgs, UOdysseyAnimationComponent* iComponent, UOdysseyAnimationTimelineTrack* iTrack, const FBuildColumnWidgetParams& iParams)
{
    ensure(iComponent);
    mComponent = iComponent;
    mTrack = iTrack;
    mRow = iParams.TreeViewRow;
    RebuildWidgets();

    mComponent->OnAnimationChanged().AddSP(this, &SOdysseyAnimationTimelineTrack::OnAnimationChanged);
    mComponent->OnPlayerChanged().AddSP(this, &SOdysseyAnimationTimelineTrack::OnPlayerChanged);
    mComponent->OnModeChanged().AddSP(this, &SOdysseyAnimationTimelineTrack::OnModeChanged);
}

void
SOdysseyAnimationTimelineTrack::RebuildWidgets()
{
    this->ChildSlot.DetachWidget();

    UOdysseyAnimation* animation = mComponent->GetActiveAnimation();
    if (!animation)
        return;

    const FCheckBoxStyle* displayLayersToggleStyle = &FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("Sequencer.AnimationTimelineTrack.DisplayLayersToggle");

    TSharedPtr<SWidget> widget = SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SBox)
            .HeightOverride(FOdysseyAnimationTimelineSectionEditor::GetCollapsedSectionHeight())
            .VAlign(VAlign_Center)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .Padding(TAttribute<FMargin>(this, &SOdysseyAnimationTimelineTrack::GetDisplayLayersPadding))
                .VAlign(VAlign_Center)
                .AutoWidth()
                [
                    SNew(SCheckBox)
                    .Style(displayLayersToggleStyle)
                    .OnCheckStateChanged(this, &SOdysseyAnimationTimelineTrack::OnDisplayLayersCheckBoxStateChanged)
                    .IsChecked(this, &SOdysseyAnimationTimelineTrack::GetDisplayLayersCheckBoxState)
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
        .AutoHeight()
        [
            /* SNew(SBox)
            .HeightOverride(this, &SOdysseyAnimationTimelineTrack::GetTreeViewHeight)
            [ */
                SNew(SOdysseyAnimationLayerStackTreeView)
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
                .Visibility(this, &SOdysseyAnimationTimelineTrack::GetLayersVisibility)
                .LayerStack(animation->GetLayerStack())
                .ExternalScrollbar(SNew(SScrollBar))
            //]
        ];

    this->ChildSlot.AttachWidget(widget.ToSharedRef());
}

/* FOptionalSize
SOdysseyAnimationTimelineTrack::GetTreeViewHeight() const
{
    return FOdysseyAnimationTrackEditorSection::GetTreeViewHeight(mComponent);
} */

void
SOdysseyAnimationTimelineTrack::OnDisplayLayersCheckBoxStateChanged(ECheckBoxState iState)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(mTrack, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationTimelineTrack, DisplayLayers), iState == ECheckBoxState::Checked);
}

ECheckBoxState
SOdysseyAnimationTimelineTrack::GetDisplayLayersCheckBoxState() const
{
    return mTrack->DisplayLayers ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

EVisibility
SOdysseyAnimationTimelineTrack::GetLayersVisibility() const
{
    return mTrack->DisplayLayers ? EVisibility::Visible : EVisibility::Collapsed;
}

FMargin
SOdysseyAnimationTimelineTrack::GetDisplayLayersPadding() const
{
    TSharedPtr<UE::Sequencer::ISequencerTreeViewRow> row = mRow.Pin();
    if (!row)
        return FMargin();

    const int32 NestingDepth = FMath::Max(0, row->GetIndentLevel());
    const float Indent = 10.f;
    return FMargin( NestingDepth * Indent, 0.f, 2.f, 0.f );
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
