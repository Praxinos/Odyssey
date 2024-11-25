// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Tools/OdysseyAnimationTimelineTool.h"

class FOdysseyAnimationEditorTimelinePosition;

class FOdysseyAnimationTimelineCutAndClearTool
    : public FOdysseyAnimationTimelineTool
{
public:
    virtual ~FOdysseyAnimationTimelineCutAndClearTool();
    FOdysseyAnimationTimelineCutAndClearTool(TSharedRef<FOdysseyAnimationEditorTimelinePosition> iTimelinePosition);

public:
    virtual FReply OnMouseButtonUp(const FMouseEventParams& iParams) override;

private:
    TSharedRef<FOdysseyAnimationEditorTimelinePosition> mTimelinePosition;
};
