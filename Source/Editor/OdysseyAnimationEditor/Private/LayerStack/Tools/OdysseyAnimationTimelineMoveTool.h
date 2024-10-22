// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Tools/OdysseyAnimationTimelineTool.h"

class FOdysseyAnimationEditorTimelinePosition;
class FOdysseyAnimationCellsMutator;

class FOdysseyAnimationTimelineMoveTool
    : public FOdysseyAnimationTimelineTool
{
public:
    virtual ~FOdysseyAnimationTimelineMoveTool();
    FOdysseyAnimationTimelineMoveTool(TSharedRef<FOdysseyAnimationEditorTimelinePosition> iTimelinePosition);

public:
    virtual FReply OnMouseButtonDown(const FMouseEventParams& iParams) override;
    virtual FReply OnMouseButtonUp(const FMouseEventParams& iParams) override;
    virtual FReply OnMouseMove(const FMouseEventParams& iParams) override;
    virtual FReply OnDragDetected(const FMouseEventParams& iParams) override;

private:
    TSharedRef<FOdysseyAnimationEditorTimelinePosition> mTimelinePosition;
    bool mOffsettingLayer;
    struct
    {
        float mInitialOffset;
        bool mIsDragDetected = false;
        double mMousePosition;
    } mLayerOffsetData;
};