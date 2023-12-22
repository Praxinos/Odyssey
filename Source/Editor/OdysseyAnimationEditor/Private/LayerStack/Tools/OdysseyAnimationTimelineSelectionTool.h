// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Tools/OdysseyAnimationTimelineTool.h"

class FOdysseyAnimationEditorTimeline;

class FOdysseyAnimationTimelineSelectionTool
    : public FOdysseyAnimationTimelineTool
{
public:
    virtual ~FOdysseyAnimationTimelineSelectionTool();
    FOdysseyAnimationTimelineSelectionTool(FOdysseyAnimationEditorTimeline* iTimelineParams);

public:
    virtual FReply OnMouseButtonDown(const FMouseEventParams& iParams) override;
    virtual FReply OnMouseButtonUp(const FMouseEventParams& iParams) override;
    virtual FReply OnMouseMove(const FMouseEventParams& iParams) override;
    virtual FReply OnDragDetected(const FMouseEventParams& iParams) override;

private:
    FOdysseyAnimationEditorTimeline* mTimelineParams;
	bool mIsSelecting = false;
	TAttribute<FInt32Range> mSelectableFrames;
    struct
    {
		bool mIsDragDetected = false;
		int mCursorFrame;
        FInt32Range mSelectedFrames;
    } mSelectionData;
};