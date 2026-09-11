// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "TimelineTools/OdysseyAnimationTimelineTool.h"

class FOdysseyPainterEditorAnimationTimelinePosition;

class FOdysseyAnimationTimelineCutTool
    : public FOdysseyAnimationTimelineTool
{
public:
    virtual ~FOdysseyAnimationTimelineCutTool();
    FOdysseyAnimationTimelineCutTool(TSharedRef<FOdysseyPainterEditorAnimationTimelinePosition> iTimelinePosition);

public:
    virtual FReply OnMouseButtonDown(const FMouseEventParams& iParams) override;
    virtual FReply OnMouseButtonUp(const FMouseEventParams& iParams) override;

    virtual FMouseCursor GetMouseCursor() const override;
};
