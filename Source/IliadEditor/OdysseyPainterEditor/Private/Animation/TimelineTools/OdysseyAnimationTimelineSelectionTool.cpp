// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "TimelineTools/OdysseyAnimationTimelineSelectionTool.h"
#include "OdysseyPainterEditorAnimationTimelinePosition.h"
#include "OdysseyAnimationCellsDragDropOperation.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyLayerCell.h"
#include "OdysseyLayerCellSelection.h"

FOdysseyAnimationTimelineSelectionTool::~FOdysseyAnimationTimelineSelectionTool()
{
}

FOdysseyAnimationTimelineSelectionTool::FOdysseyAnimationTimelineSelectionTool(TSharedRef<FOdysseyPainterEditorAnimationTimelinePosition> iTimelinePosition, TSharedRef<FOdysseyLayerCellSelection> iTimelineCellSelection)
    : FOdysseyAnimationTimelineTool(iTimelinePosition)
    , mTimelineCellSelection(iTimelineCellSelection)
{
}

FReply
FOdysseyAnimationTimelineSelectionTool::OnMouseButtonDown(const FMouseEventParams& iParams)
{
    if (iParams.mMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
        return FReply::Unhandled();

    switch(iParams.mOrigin)
    {
        case EMouseEventOrigin::Layer:
        {
            mTimelineCellSelection->SetSelectedCells({});
            return FReply::Handled();
        }

        case EMouseEventOrigin::CellsTimeline:
        {
            if (iParams.mMouseEvent.IsShiftDown())
                return OnContiguousSelectionMouseButtonDown(iParams);
            else if (iParams.mMouseEvent.IsControlDown())
                return OnNonContiguousSelectionMouseButtonDown(iParams);
            else
                return OnDefaultSelectionMouseButtonDown(iParams);
        }
    }

    return FReply::Unhandled();
}

FReply
FOdysseyAnimationTimelineSelectionTool::OnDragDetected(const FMouseEventParams& iParams)
{
    if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
        return FReply::Unhandled();

    switch(mSelectionMode)
    {
        case ESelectionMode::Default: return OnDefaultSelectionDragDetected(iParams);
        case ESelectionMode::Contiguous: return OnContiguousSelectionDragDetected(iParams);
        case ESelectionMode::NonContiguous: return OnNonContiguousSelectionDragDetected(iParams);
    }

    return FReply::Handled();
}

FReply
FOdysseyAnimationTimelineSelectionTool::OnMouseMove(const FMouseEventParams& iParams)
{
    if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
        return FReply::Unhandled();

    switch(mSelectionMode)
    {
        case ESelectionMode::Default: return OnDefaultSelectionMouseMove(iParams);
        case ESelectionMode::Contiguous: return OnContiguousSelectionMouseMove(iParams);
        case ESelectionMode::NonContiguous: return OnNonContiguousSelectionMouseMove(iParams);
    }

    //updateSelection
    return FReply::Unhandled();
}

FReply
FOdysseyAnimationTimelineSelectionTool::OnMouseButtonUp(const FMouseEventParams& iParams)
{
    if (iParams.mMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
        return FReply::Unhandled();

    if (iParams.mOrigin != EMouseEventOrigin::CellsTimeline)
        return FReply::Unhandled();

    switch(mSelectionMode)
    {
        case ESelectionMode::Default: return OnDefaultSelectionMouseButtonUp(iParams);
        case ESelectionMode::Contiguous: return OnContiguousSelectionMouseButtonUp(iParams);
        case ESelectionMode::NonContiguous: return OnNonContiguousSelectionMouseButtonUp(iParams);
    }

    return FReply::Unhandled();
}

FReply
FOdysseyAnimationTimelineSelectionTool::OnKeyDown(const FKeyEvent& iKeyEvent)
{
    if (iKeyEvent.GetKey() == EKeys::Escape)
    {
        mTimelineCellSelection->SetSelectedCells({});
        return FReply::Handled();
    }
    return FReply::Unhandled();
}

FMouseCursor
FOdysseyAnimationTimelineSelectionTool::GetMouseCursor() const //override
{
    return EMouseCursor::Default;
}

//--------------------
// Default
//--------------------

FReply
FOdysseyAnimationTimelineSelectionTool::OnDefaultSelectionMouseButtonDown(const FMouseEventParams& iParams)
{
    if (mIsSelecting)
        return FReply::Handled();

    mSelectionMode = ESelectionMode::Default;
    mIsDragDetected = false;
    mShouldDeselect = false;

    float frame = GetFrameUnderCursor(iParams);

    if (IsFrameSelected(iParams.mLayer, frame))
    {
        mIsSelecting = false;
        mIsDragnDrop = true;
        return FReply::Handled().DetectDrag(iParams.mWidget.ToSharedRef(), EKeys::LeftMouseButton);
    }

    mIsDragnDrop = false;
    mInitialSelection = TArray<UOdysseyLayerCell*>();

    if (!SetCellSelectionCursorAtFrame(iParams.mLayer, frame))
        return FReply::Unhandled();

    mCellCursor = mTimelineCellSelection->GetCellSelectionCursor();

    if (!SelectFromCursorToFrame(iParams.mLayer, frame))
        return FReply::Unhandled();

    mIsSelecting = true;

    return FReply::Handled().DetectDrag(iParams.mWidget.ToSharedRef(), EKeys::LeftMouseButton);
}

FReply
FOdysseyAnimationTimelineSelectionTool::OnDefaultSelectionDragDetected(const FMouseEventParams& iParams)
{
    mIsDragDetected = true;
    if (mIsDragnDrop)
    {
        mIsSelecting = false;
        TSharedRef<FOdysseyAnimationCellsDragDropOperation> operation = FOdysseyAnimationCellsDragDropOperation::Create(iParams.mLayer, mTimelineCellSelection->GetSelectedCells());
        return FReply::Handled().BeginDragDrop(operation);
    }

    if (mIsSelecting)
    {
        return FReply::Handled().CaptureMouse( iParams.mWidget.ToSharedRef() ).PreventThrottling();
    }

    return FReply::Unhandled();

}

FReply
FOdysseyAnimationTimelineSelectionTool::OnDefaultSelectionMouseMove(const FMouseEventParams& iParams)
{
    if(!mIsSelecting)
        return FReply::Unhandled();

    if (!mIsDragDetected)
        return FReply::Unhandled();

    float frame = GetFrameUnderCursor(iParams);

    if (!SelectFromCursorToFrame(iParams.mLayer, frame))
        return FReply::Unhandled();

    return FReply::Handled();
}

FReply
FOdysseyAnimationTimelineSelectionTool::OnDefaultSelectionMouseButtonUp(const FMouseEventParams& iParams)
{
    if (!mIsSelecting && !mIsDragDetected)
    {
        mTimelineCellSelection->SetSelectedCells({});
        return FReply::Handled();
    }

    mIsSelecting = false;
    mIsDragnDrop = false;
    mIsDragDetected = false;

    return FReply::Handled().ReleaseMouseCapture();
}

//--------------------
// Contiguous
//--------------------

FReply
FOdysseyAnimationTimelineSelectionTool::OnContiguousSelectionMouseButtonDown(const FMouseEventParams& iParams)
{
    if (mIsSelecting)
        return FReply::Handled();

    mSelectionMode = ESelectionMode::Contiguous;

    mShouldDeselect = false;
    mIsDragDetected = false;
    mInitialSelection = TArray<UOdysseyLayerCell*>();

    float frame = GetFrameUnderCursor(iParams);

    mCellCursor = mTimelineCellSelection->GetCellSelectionCursor();

    if (!mCellCursor)
    {
        if (!SetCellSelectionCursorAtFrame(iParams.mLayer, frame))
            return FReply::Unhandled();

        mCellCursor = mTimelineCellSelection->GetCellSelectionCursor();
    }

    if (!SelectFromCursorToFrame(iParams.mLayer, frame))
        return FReply::Unhandled();

    mIsSelecting = true;

    return FReply::Handled().DetectDrag(iParams.mWidget.ToSharedRef(), EKeys::LeftMouseButton);
}

FReply
FOdysseyAnimationTimelineSelectionTool::OnContiguousSelectionDragDetected(const FMouseEventParams& iParams)
{
    if(!mIsSelecting)
        return FReply::Unhandled();

    return FReply::Handled().CaptureMouse( iParams.mWidget.ToSharedRef() ).PreventThrottling();
}

FReply
FOdysseyAnimationTimelineSelectionTool::OnContiguousSelectionMouseMove(const FMouseEventParams& iParams)
{
    if(!mIsSelecting)
        return FReply::Unhandled();

    float frame = GetFrameUnderCursor(iParams);

    if (!SelectFromCursorToFrame(iParams.mLayer, frame))
        return FReply::Unhandled();

    return FReply::Handled();
}

FReply
FOdysseyAnimationTimelineSelectionTool::OnContiguousSelectionMouseButtonUp(const FMouseEventParams& iParams)
{
    if(!mIsSelecting)
        return FReply::Unhandled();

    mIsSelecting = false;
    mIsDragDetected = false;

    return FReply::Handled().ReleaseMouseCapture();
}

//--------------------
// Non Contiguous
//--------------------

FReply
FOdysseyAnimationTimelineSelectionTool::OnNonContiguousSelectionMouseButtonDown(const FMouseEventParams& iParams)
{
    if (mIsSelecting)
        return FReply::Handled();

    mSelectionMode = ESelectionMode::NonContiguous;
    mIsDragDetected = false;
    mInitialSelection = mTimelineCellSelection->GetSelectedCells();

    float frame = GetFrameUnderCursor(iParams);

    mShouldDeselect = IsFrameSelected(iParams.mLayer, frame);

    if (mShouldDeselect)
    {
        UOdysseyLayerCell* cell = iParams.mLayer->GetCellAtFrame(frame);
        if (!cell)
            return FReply::Unhandled();

        mCellCursor = cell;
        if (!SelectFromCursorToFrame(iParams.mLayer, frame, true))
            return FReply::Unhandled();
    }
    else
    {
        if (!SetCellSelectionCursorAtFrame(iParams.mLayer, frame))
            return FReply::Unhandled();

        mCellCursor = mTimelineCellSelection->GetCellSelectionCursor();

        if (!SelectFromCursorToFrame(iParams.mLayer, frame))
            return FReply::Unhandled();
    }

    mIsSelecting = true;

    return FReply::Handled().DetectDrag(iParams.mWidget.ToSharedRef(), EKeys::LeftMouseButton);
}

FReply
FOdysseyAnimationTimelineSelectionTool::OnNonContiguousSelectionDragDetected(const FMouseEventParams& iParams)
{
    if(!mIsSelecting)
        return FReply::Unhandled();

    return FReply::Handled().CaptureMouse( iParams.mWidget.ToSharedRef() ).PreventThrottling();
}

FReply
FOdysseyAnimationTimelineSelectionTool::OnNonContiguousSelectionMouseMove(const FMouseEventParams& iParams)
{
    if(!mIsSelecting)
        return FReply::Unhandled();

    float frame = GetFrameUnderCursor(iParams);

    if (!SelectFromCursorToFrame(iParams.mLayer, frame, mShouldDeselect))
        return FReply::Unhandled();

    return FReply::Handled();
}

FReply
FOdysseyAnimationTimelineSelectionTool::OnNonContiguousSelectionMouseButtonUp(const FMouseEventParams& iParams)
{
    if(!mIsSelecting)
        return FReply::Unhandled();

    mIsSelecting = false;
    mIsDragDetected = false;

    return FReply::Handled().ReleaseMouseCapture();
}

int
FOdysseyAnimationTimelineSelectionTool::GetFrameUnderCursor(const FMouseEventParams& iParams)
{
    float posX = iParams.mGeometry.AbsoluteToLocal(iParams.mMouseEvent.GetScreenSpacePosition()).X;
    int frame = (int)MousePositionToFrame(posX);

    return frame;
}

bool
FOdysseyAnimationTimelineSelectionTool::SelectFromCursorToFrame(UOdysseyAnimationLayer* iLayer, int iFrame, bool iDeselect)
{
    if (!mCellCursor)
        return false;

    FInt32Range cursorCellFrameRange = mCellCursor->GetFrameRange();

    TArray<UOdysseyLayerCell*> affectedCells = {};
    affectedCells.AddUnique(mCellCursor);

    int currentFrame = cursorCellFrameRange.GetLowerBoundValue() - 1;
    while(currentFrame >= iFrame)
    {
        UOdysseyLayerCell* cell = iLayer->GetCellAtFrame(currentFrame);
        if (!cell)
            break;

        affectedCells.AddUnique(cell);
        currentFrame -= cell->GetExposure();
    }

    currentFrame = cursorCellFrameRange.GetUpperBoundValue() + 1;
    while(currentFrame <= iFrame)
    {
        UOdysseyLayerCell* cell = iLayer->GetCellAtFrame(currentFrame);
        if (!cell)
            break;

        affectedCells.AddUnique(cell);
        currentFrame += cell->GetExposure();
    }

    TArray<UOdysseyLayerCell*> selectedCells = { mInitialSelection };
    if (iDeselect)
    {
        selectedCells.RemoveAll(
            [&affectedCells](UOdysseyLayerCell* iCell)
            {
                return affectedCells.Contains(iCell);
            }
        );
    }
    else
    {
        for ( UOdysseyLayerCell* affectedCell : affectedCells)
        {
            selectedCells.AddUnique(affectedCell);
        }
    }
    mTimelineCellSelection->SetSelectedCells(selectedCells);
    return true;
}

bool
FOdysseyAnimationTimelineSelectionTool::SetCellSelectionCursorAtFrame(UOdysseyAnimationLayer* iLayer, int iFrame)
{
    UOdysseyLayerCell* cell = iLayer->GetCellAtFrame(iFrame);
    if (!cell)
        return false;

    mTimelineCellSelection->SelectCell(cell, true);
    return true;
}

bool
FOdysseyAnimationTimelineSelectionTool::IsFrameSelected(UOdysseyAnimationLayer* iLayer, int iFrame) const
{
    UOdysseyLayerCell* cell = iLayer->GetCellAtFrame(iFrame);
    if (!cell)
        return false;

    return mTimelineCellSelection->GetSelectedCells().Contains(cell);
}
