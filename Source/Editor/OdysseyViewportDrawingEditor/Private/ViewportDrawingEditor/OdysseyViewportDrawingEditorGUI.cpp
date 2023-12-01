// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "ViewportDrawingEditor/OdysseyViewportDrawingEditorGUI.h"
#include "AnimationEditor/OdysseyAnimationEditorTimelineTab.h"

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
		SNew(STextBlock)
        .Text(LOCTEXT("animation.timeline-tab.empty-timeline-message", "The actor (plane, 3D object, etc.) selected in the 3D viewport contains no 2D animation. If you are using a Sequencer, position your cursor at the time when a 2D animation is applied to the actor."))
	);
}

#undef LOCTEXT_NAMESPACE