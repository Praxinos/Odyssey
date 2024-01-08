// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationTimelineTool
{
public:
    enum class EMouseEventOrigin
    {
        Layer, //The whole layer timeline
        CellsTimeline, //the timeline section containing all the cells
        Cell, //a single cell

        //As other examples, we could have
        //Or even other origins
        //Lighttable, //the timeline section containing the lighttable keys
        //LighttableKey //a single lighttable key
    };

public:
    virtual ~FOdysseyAnimationTimelineTool();

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
};