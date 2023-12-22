// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Tools/OdysseyAnimationTimelineTool.h"

class FOdysseyAnimationEditorTimeline;

class FOdysseyAnimationTimelineMoveTool
    : public FOdysseyAnimationTimelineTool
{
public:
    virtual ~FOdysseyAnimationTimelineMoveTool();
    FOdysseyAnimationTimelineMoveTool(FOdysseyAnimationEditorTimeline* iTimelineParams);

public:
    virtual FReply OnMouseButtonDown(const FMouseEventParams& iParams) override;
    virtual FReply OnMouseButtonUp(const FMouseEventParams& iParams) override;
    virtual FReply OnMouseMove(const FMouseEventParams& iParams) override;
    virtual FReply OnDragDetected(const FMouseEventParams& iParams) override;

private:
    FOdysseyAnimationEditorTimeline* mTimelineParams;
    TSharedPtr<FOdysseyAnimationCellsMutator> mCellsMutator;
    bool mOffsettingLayer;
    struct
    {
        float mInitialOffset;
        bool mIsDragDetected = false;
        double mMousePosition;
    } mLayerOffsetData;
};