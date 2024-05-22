// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Tools/OdysseyAnimationTimelineTool.h"

class FOdysseyAnimationEditorTimeline;
class FOdysseyAnimationCell;

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

    virtual FReply OnKeyDown(const FKeyEvent& iKeyEvent) override;

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

    FOdysseyAnimationEditorTimeline* mTimelineParams;
	bool mIsSelecting = false;
	bool mIsDragDetected = false;
    bool mIsDragnDrop = false;
    ESelectionMode mSelectionMode = ESelectionMode::Default;
    bool mShouldDeselect = false;
    TArray<TSharedPtr<FOdysseyAnimationCell>> mInitialSelection;
    TSharedPtr<FOdysseyAnimationCell> mCellCursor; //Most of the time same as mTimelineParams->GetCellSelectionCursor() but can be different in some cases
};