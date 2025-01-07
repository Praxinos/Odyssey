// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023
#include "Widgets/SOdysseyAnimationComponentTrack.h"

#include "Widgets/LayerStack/SOdysseyAnimationLayerStackTreeView.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "TrackEditors/SubTrackEditorBase.h"
#include "OdysseyAnimationTrackEditorSection.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyStyleSet.h"
#include "OdysseyAnimation.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyAnimationComponentTrack.h"
#include "OdysseyViewportDrawingEditorEdMode.h"
#include "EditorModeManager.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "OdysseyViewportDrawingEditorToolkit.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

void
SOdysseyAnimationComponentTrack::Construct(const FArguments& iArgs, UOdysseyAnimationComponent* iComponent, UOdysseyAnimationComponentTrack* iTrack, const FBuildColumnWidgetParams& iParams)
{
    ensure(iComponent);
    mComponent = iComponent;
    mTrack = iTrack;
    mRow = iParams.TreeViewRow;
    RebuildWidgets();

    mComponent->OnAnimationChanged().AddSP(this, &SOdysseyAnimationComponentTrack::OnAnimationChanged);
    mComponent->OnPlayerChanged().AddSP(this, &SOdysseyAnimationComponentTrack::OnPlayerChanged);
    mComponent->OnModeChanged().AddSP(this, &SOdysseyAnimationComponentTrack::OnModeChanged);
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
        .AutoHeight()
        [
            /* SNew(SBox)
            .HeightOverride(this, &SOdysseyAnimationComponentTrack::GetTreeViewHeight)
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

                        TSharedPtr<FOdysseyAnimationEditorExtension> animationExtension = toolkit->GetAnimationExtension();
                        if (!animationExtension)
                            return nullptr;

                        if (animationExtension->Animation() != animation)
                            return nullptr;

                        FOdysseyPainterEditor* editor = odysseyEdMode->GetEditor();
                        if (!editor)
                            return nullptr;

                        return editor;
                    }
                )
                .Visibility(this, &SOdysseyAnimationComponentTrack::GetLayersVisibility)
                .LayerStack(animation->GetLayerStack())
                .ExternalScrollbar(SNew(SScrollBar))
            //]
        ];

    this->ChildSlot.AttachWidget(widget.ToSharedRef());
}

/* FOptionalSize
SOdysseyAnimationComponentTrack::GetTreeViewHeight() const
{
    return FOdysseyAnimationTrackEditorSection::GetTreeViewHeight(mComponent);
} */

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
    TSharedPtr<UE::Sequencer::ISequencerTreeViewRow> row = mRow.Pin();
    if (!row)
        return FMargin();

    const int32 NestingDepth = FMath::Max(0, row->GetIndentLevel());
    const float Indent = 10.f;
    return FMargin( NestingDepth * Indent, 0.f, 2.f, 0.f );
}

void
SOdysseyAnimationComponentTrack::OnAnimationChanged()
{
    RebuildWidgets();
}

void
SOdysseyAnimationComponentTrack::OnPlayerChanged()
{
    RebuildWidgets();
}

void
SOdysseyAnimationComponentTrack::OnModeChanged()
{
    RebuildWidgets();
}

#undef LOCTEXT_NAMESPACE
