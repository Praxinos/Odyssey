// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "TimelineTools/OdysseyAnimationTimelineTool.h"
#include "Input/Reply.h"

class FOdysseyLayerCellSelection;
class FOdysseyPainterEditorAnimationTimelinePosition;
class UOdysseyLayerCell;

class FOdysseyAnimationTimelineSelectionTool
    : public FOdysseyAnimationTimelineTool
{
public:
    virtual ~FOdysseyAnimationTimelineSelectionTool();
    FOdysseyAnimationTimelineSelectionTool(TSharedRef<FOdysseyPainterEditorAnimationTimelinePosition> iTimelinePosition, TSharedRef<FOdysseyLayerCellSelection> iTimelineCellSelection);

public:
    virtual FReply OnMouseButtonDown(const FMouseEventParams& iParams) override;
    virtual FReply OnMouseButtonUp(const FMouseEventParams& iParams) override;
    virtual FReply OnMouseMove(const FMouseEventParams& iParams) override;
    virtual FReply OnDragDetected(const FMouseEventParams& iParams) override;

    virtual FReply OnKeyDown(const FKeyEvent& iKeyEvent) override;

    virtual FMouseCursor GetMouseCursor() const override;

private:
    FReply OnDefaultSelectionMouseButtonDown(const FMouseEventParams& iParams);
    FReply OnDefaultSelectionMouseButtonUp(const FMouseEventParams& iParams);
    FReply OnDefaultSelectionMouseMove(const FMouseEventParams& iParams);
    FReply OnDefaultSelectionDragDetected(const FMouseEventParams& iParams);

    FReply OnContiguousSelectionMouseButtonDown(const FMouseEventParams& iParams);
    FReply OnContiguousSelectionMouseButtonUp(const FMouseEventParams& iParams);
    FReply OnContiguousSelectionMouseMove(const FMouseEventParams& iParams);
    FReply OnContiguousSelectionDragDetected(const FMouseEventParams& iParams);

    FReply OnNonContiguousSelectionMouseButtonDown(const FMouseEventParams& iParams);
    FReply OnNonContiguousSelectionMouseButtonUp(const FMouseEventParams& iParams);
    FReply OnNonContiguousSelectionMouseMove(const FMouseEventParams& iParams);
    FReply OnNonContiguousSelectionDragDetected(const FMouseEventParams& iParams);

private:
    int GetFrameUnderCursor(const FMouseEventParams& iParams);
    bool SelectFromCursorToFrame(UOdysseyAnimationLayer* iLayer, int iFrame, bool iDeselect = false);
    bool SetCellSelectionCursorAtFrame(UOdysseyAnimationLayer* iLayer, int iFrame);
    bool IsFrameSelected(UOdysseyAnimationLayer* iLayer, int iFrame) const;

private:
    enum class ESelectionMode
    {
        Default,
        Contiguous,
        NonContiguous
    };

    TSharedRef<FOdysseyLayerCellSelection> mTimelineCellSelection;
    bool mIsSelecting = false;
    bool mIsDragDetected = false;
    bool mIsDragnDrop = false;
    ESelectionMode mSelectionMode = ESelectionMode::Default;
    bool mShouldDeselect = false;
    TArray<UOdysseyLayerCell*> mInitialSelection;
    UOdysseyLayerCell* mCellCursor; //Most of the time same as mTimelineParams->GetCellSelectionCursor() but can be different in some cases
};
