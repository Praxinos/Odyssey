// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Tools/OdysseyAnimationTimelineSelectionTool.h"
#include "OdysseyAnimationEditorTimeline.h"
#include "DragDropOperations/OdysseyAnimationCellsDragDropOperation.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"

FOdysseyAnimationTimelineSelectionTool::~FOdysseyAnimationTimelineSelectionTool()
{
}

FOdysseyAnimationTimelineSelectionTool::FOdysseyAnimationTimelineSelectionTool(FOdysseyAnimationEditorTimeline* iTimelineParams)
    : mTimelineParams(iTimelineParams)
{   
}

FReply
FOdysseyAnimationTimelineSelectionTool::OnMouseButtonDown(const FMouseEventParams& iParams)
{
	switch(iParams.mOrigin)
	{
		case EMouseEventOrigin::Layer:
		{
			if (iParams.mMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
			{
				mTimelineParams->SetSelectedCells({});
				return FReply::Handled();
			}
			return FReply::Unhandled();
		}
		break;

		case EMouseEventOrigin::CellsTimeline:
		{
			if (iParams.mMouseEvent.IsShiftDown() && iParams.mMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
				return OnContiguousSelectionMouseButtonDown(iParams);
			else if (iParams.mMouseEvent.IsControlDown() && iParams.mMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
				return OnNonContiguousSelectionMouseButtonDown(iParams);
			else if ( iParams.mMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
				return OnDefaultSelectionMouseButtonDown(iParams);
		}
		break;
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
		mTimelineParams->SetSelectedCells({});
		return FReply::Handled();
	}
	return FReply::Unhandled();
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
	mInitialSelection = {};

	if (!SetCellSelectionCursorAtFrame(iParams.mLayer, frame))
		return FReply::Unhandled();

	mCellCursor = mTimelineParams->GetCellSelectionCursor();

	if (!SelectFromCursorToFrame(iParams.mLayer, frame))
		return FReply::Unhandled();
		
	mIsSelecting = true;

	return FReply::Handled().DetectDrag(iParams.mWidget.ToSharedRef(), EKeys::LeftMouseButton);
}

FReply
FOdysseyAnimationTimelineSelectionTool::OnDefaultSelectionDragDetected(const FMouseEventParams& iParams)
{
	if (mIsDragnDrop)
	{
		mIsDragnDrop = false;
		mIsSelecting = false;
		mIsDragDetected = false;
		TSharedRef<FOdysseyAnimationCellsDragDropOperation> operation = FOdysseyAnimationCellsDragDropOperation::Create(iParams.mLayer, mTimelineParams->GetSelectedCells());
    	return FReply::Handled().BeginDragDrop(operation);
	}
	
    if (mIsSelecting)
	{
		mIsDragDetected = true;
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
    if(!mIsSelecting)
		return FReply::Unhandled();

	mIsSelecting = false;
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
	mInitialSelection = {};

	float frame = GetFrameUnderCursor(iParams);

	mCellCursor = mTimelineParams->GetCellSelectionCursor();

	if (!mCellCursor)
	{	
		if (!SetCellSelectionCursorAtFrame(iParams.mLayer, frame))
			return FReply::Unhandled();
	
		mCellCursor = mTimelineParams->GetCellSelectionCursor();
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
	mInitialSelection = mTimelineParams->GetSelectedCells();
	
	float frame = GetFrameUnderCursor(iParams);

	mShouldDeselect = IsFrameSelected(iParams.mLayer, frame);
		
	if (mShouldDeselect)
	{
		UOdysseyAnimationCell* cell = iParams.mLayer->GetCellAtFrame(frame);
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
	
		mCellCursor = mTimelineParams->GetCellSelectionCursor();

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
	int frame = (int)mTimelineParams->MousePositionToFrame(posX);

	return frame;
}

bool
FOdysseyAnimationTimelineSelectionTool::SelectFromCursorToFrame(UOdysseyAnimationLayer* iLayer, int iFrame, bool iDeselect)
{
	if (!mCellCursor)
		return false;

	FInt32Range cursorCellFrameRange = mCellCursor->GetFrameRange();

	TArray<UOdysseyAnimationCell*> affectedCells = {};
	affectedCells.AddUnique(mCellCursor);

	int currentFrame = cursorCellFrameRange.GetLowerBoundValue() - 1;
	while(currentFrame >= iFrame)
	{
		UOdysseyAnimationCell* cell = iLayer->GetCellAtFrame(currentFrame);
		if (!cell)
			break;

		affectedCells.AddUnique(cell);
		currentFrame -= cell->Length;
	}

	currentFrame = cursorCellFrameRange.GetUpperBoundValue() + 1;
	while(currentFrame <= iFrame)
	{
		UOdysseyAnimationCell* cell = iLayer->GetCellAtFrame(currentFrame);
		if (!cell)
			break;

		affectedCells.AddUnique(cell);
		currentFrame += cell->Length;
	}
	
	TArray<UOdysseyAnimationCell*> selectedCells = { mInitialSelection };
	if (iDeselect)
	{
		selectedCells.RemoveAll(
			[&affectedCells](UOdysseyAnimationCell* iCell)
			{
				return affectedCells.Contains(iCell);
			}
		);
	}
	else
	{
		for ( UOdysseyAnimationCell* affectedCell : affectedCells)
		{
			selectedCells.AddUnique(affectedCell);
		}
	}
	mTimelineParams->SetSelectedCells(selectedCells);
	return true;
}

bool
FOdysseyAnimationTimelineSelectionTool::SetCellSelectionCursorAtFrame(UOdysseyAnimationLayer* iLayer, int iFrame)
{
	UOdysseyAnimationCell* cell = iLayer->GetCellAtFrame(iFrame);
	if (!cell)
		return false;

	mTimelineParams->SelectCell(cell, true);
	return true;
}

bool
FOdysseyAnimationTimelineSelectionTool::IsFrameSelected(UOdysseyAnimationLayer* iLayer, int iFrame) const
{
	UOdysseyAnimationCell* cell = iLayer->GetCellAtFrame(iFrame);
	if (!cell)
		return false;

	return mTimelineParams->GetSelectedCells().Contains(cell);
}