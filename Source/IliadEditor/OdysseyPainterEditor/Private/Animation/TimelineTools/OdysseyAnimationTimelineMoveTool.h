// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "TimelineTools/OdysseyAnimationTimelineTool.h"

class FOdysseyPainterEditorAnimationTimelinePosition;
class FOdysseyAnimationCellsMutator;

class FOdysseyAnimationTimelineMoveTool
    : public FOdysseyAnimationTimelineTool
{
public:
    virtual ~FOdysseyAnimationTimelineMoveTool();
    FOdysseyAnimationTimelineMoveTool(TSharedRef<FOdysseyPainterEditorAnimationTimelinePosition> iTimelinePosition);

public:
    virtual FReply OnMouseButtonDown(const FMouseEventParams& iParams) override;
    virtual FReply OnMouseButtonUp(const FMouseEventParams& iParams) override;
    virtual FReply OnMouseMove(const FMouseEventParams& iParams) override;
    virtual FReply OnDragDetected(const FMouseEventParams& iParams) override;

    virtual FMouseCursor GetMouseCursor() const override;

private:
    bool mOffsettingLayer;
    struct
    {
        float mInitialOffset;
        bool mIsDragDetected = false;
        FVector2D mMousePosition;
    } mLayerOffsetData;
};
