// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Tools/OdysseyAnimationTimelineTool.h"

class FOdysseyAnimationEditorTimeline;

class FOdysseyAnimationTimelineCutTool
    : public FOdysseyAnimationTimelineTool
{
public:
    virtual ~FOdysseyAnimationTimelineCutTool();
    FOdysseyAnimationTimelineCutTool(FOdysseyAnimationEditorTimeline* iTimelineParams);

public:
    virtual FReply OnMouseButtonUp(const FMouseEventParams& iParams) override;

private:
    FOdysseyAnimationEditorTimeline* mTimelineParams;
};