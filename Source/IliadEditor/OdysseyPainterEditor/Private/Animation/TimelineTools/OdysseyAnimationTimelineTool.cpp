// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "TimelineTools/OdysseyAnimationTimelineTool.h"

FOdysseyAnimationTimelineTool::~FOdysseyAnimationTimelineTool()
{

}

FOdysseyAnimationTimelineTool::FOdysseyAnimationTimelineTool(TSharedRef<FOdysseyPainterEditorAnimationTimelinePosition> iTimelinePosition)
    : mTimelinePosition(iTimelinePosition)
{

}

FReply
FOdysseyAnimationTimelineTool::OnMouseButtonDown(const FMouseEventParams& iParams)
{
    return FReply::Unhandled();
}

FReply
FOdysseyAnimationTimelineTool::OnMouseButtonUp(const FMouseEventParams& iParams)
{
    return FReply::Unhandled();
}

FReply
FOdysseyAnimationTimelineTool::OnMouseMove(const FMouseEventParams& iParams)
{
    return FReply::Unhandled();
}

FReply
FOdysseyAnimationTimelineTool::OnDragDetected(const FMouseEventParams& iParams)
{
    return FReply::Unhandled();
}

FReply
FOdysseyAnimationTimelineTool::OnKeyDown(const FKeyEvent& iKeyEvent)
{
    return FReply::Unhandled();
}

FReply
FOdysseyAnimationTimelineTool::OnKeyUp(const FKeyEvent& iKeyEvent)
{
    return FReply::Unhandled();
}

float
FOdysseyAnimationTimelineTool::MousePositionToFrame(float iX) const
{
    return iX / mTimelinePosition->GetFrameSize();
}

float
FOdysseyAnimationTimelineTool::FrameToMousePosition(float iFrame) const
{
    return iFrame * mTimelinePosition->GetFrameSize();
}

FMouseCursor
FOdysseyAnimationTimelineTool::GetMouseCursor() const
{
    return EMouseCursor::Default;
}
