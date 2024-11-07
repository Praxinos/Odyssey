// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "ViewportDrawingEditor/OdysseyViewportDrawingEditorGUI.h"
#include "AnimationEditor/OdysseyAnimationEditorTimelineTab.h"
#include "OdysseyPainterEditor.h"
#include "ViewportDrawingEditor/OdysseyViewportDrawingEditorExtension.h"

#define LOCTEXT_NAMESPACE "ViewportDrawingEditor"

/////////////////////////////////////////////////////
// FOdysseyViewportDrawingEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyViewportDrawingEditorGUI::~FOdysseyViewportDrawingEditorGUI()
{
}

FOdysseyViewportDrawingEditorGUI::FOdysseyViewportDrawingEditorGUI(FOdysseyViewportDrawingEditorExtension* iExtension)
    : mExtension(iExtension)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyViewportDrawingEditorGUI::Initialize()
{
    CustomizeAnimationEditorTimeline();
}

void
FOdysseyViewportDrawingEditorGUI::Finalize()
{

}

void
FOdysseyViewportDrawingEditorGUI::CustomizeAnimationEditorTimeline()
{
    FOdysseyPainterEditor* editor = mExtension->GetEditor();
    TSharedPtr<FOdysseyAnimationEditorTimelineTab> timelineTab = editor->FindTab<FOdysseyAnimationEditorTimelineTab>();
    if (!timelineTab)
        return;

    timelineTab->SetEmptyTimelineWidget(
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("animation.timeline-tab.empty-timeline-message-1", "Your timeline panel is empty for one of the following reasons:"))
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("animation.timeline-tab.empty-timeline-message-2", "- The selected Actor does not use a 2D Animation"))
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("animation.timeline-tab.empty-timeline-message-3", "- The Sequencer is not open."))
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("animation.timeline-tab.empty-timeline-message-4", "- The 2D Animation is not used in the Sequencer as a Media Track."))
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("animation.timeline-tab.empty-timeline-message-5", "- The cursor in the Sequencer is off of the corresponding Media Track."))
        ]
    );
}

#undef LOCTEXT_NAMESPACE
