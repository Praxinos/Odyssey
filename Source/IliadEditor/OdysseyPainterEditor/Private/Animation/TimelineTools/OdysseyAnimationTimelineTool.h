// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Layout/Geometry.h"
#include "Input/Events.h"
#include "Input/Reply.h"

#include "OdysseyPainterEditorAnimationTimelinePosition.h"
#include "OdysseyPainterEditorToolMouseCursor.h"

class FOdysseyAnimationTimelineTool
{
public:
    enum class EMouseEventOrigin
    {
        Layer, //The whole layer timeline
        CellsTimeline, //the timeline section containing all the cells

        //As other examples, we could have
        //Or even other origins
        //Cell, //a single cell
        //Lighttable, //the timeline section containing the lighttable keys
        //LighttableKey //a single lighttable key
    };

public:
    virtual ~FOdysseyAnimationTimelineTool();

    FOdysseyAnimationTimelineTool(TSharedRef<FOdysseyPainterEditorAnimationTimelinePosition> iTimelinePosition);

public:
    struct FMouseEventParams
    {
        const FGeometry mGeometry;
        const FPointerEvent& mMouseEvent;
        TSharedPtr<SWidget> mWidget;
        EMouseEventOrigin mOrigin;
        class UOdysseyAnimationLayer* mLayer;
    };

    virtual FReply OnMouseButtonDown(const FMouseEventParams& iParams);
    virtual FReply OnMouseButtonUp(const FMouseEventParams& iParams);
    virtual FReply OnMouseMove(const FMouseEventParams& iParams);
    virtual FReply OnDragDetected(const FMouseEventParams& iParams);

    virtual FReply OnKeyDown(const FKeyEvent& iKeyEvent);
    virtual FReply OnKeyUp(const FKeyEvent& iKeyEvent);

    virtual FMouseCursor GetMouseCursor() const;

public:
    float MousePositionToFrame(float iX) const;
    float FrameToMousePosition(float iFrame) const;

protected:
    TSharedRef<FOdysseyPainterEditorAnimationTimelinePosition> mTimelinePosition;
};
