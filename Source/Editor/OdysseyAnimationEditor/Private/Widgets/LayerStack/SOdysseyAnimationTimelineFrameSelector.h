// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationEditorExtension;
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineFrameSelection
	: public SCompoundWidget
{
public:
	DECLARE_DELEGATE_RetVal(FReply, FOnSelectionDragged)

public:
	SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineFrameSelection)
		: _Content()
	{}
		SLATE_DEFAULT_SLOT( FArguments, Content )
		SLATE_ATTRIBUTE(FInt32Range, SelectedFrames)
		SLATE_EVENT(FOnSelectionDragged, OnSelectionDragged)
	SLATE_END_ARGS()

	SOdysseyAnimationTimelineFrameSelection();

	void Construct(
		const FArguments& InArgs,
		FOdysseyAnimationEditorExtension* iExtension);

	// SWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	// End of SWidget interfacepublic:

private:
	bool GetSelectedFrames(int& oStartFrame, int& oEndFrame) const;

	FReply OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	FReply OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) override;

private:
	TAttribute<FInt32Range> mSelectedFrames;
	
	FOdysseyAnimationEditorExtension* mExtension;

	FOnSelectionDragged mOnSelectionDragged;
};

class SOdysseyAnimationTimelineFrameSelector
	: public SCompoundWidget
{
	DECLARE_DELEGATE_OneParam(FOnSelectionChanged, FInt32Range)
	DECLARE_DELEGATE_OneParam(FOnSelectionStarted, int /* iFrame */)
	DECLARE_DELEGATE_OneParam(FOnSelectionEnded, int /* iFrame */)

public:
	SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineFrameSelector)
	{}
		SLATE_ATTRIBUTE(FInt32Range, SelectableFrames)
		SLATE_EVENT(FOnSelectionStarted, OnSelectionStarted)
		SLATE_EVENT(FOnSelectionEnded, OnSelectionEnded)
		SLATE_EVENT(FOnSelectionChanged, OnSelectionChanged)
	SLATE_END_ARGS()

	void Construct(
		const FArguments& InArgs,
		FOdysseyAnimationEditorExtension* iExtension);

	// SWidget interface
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) override;
	// End of SWidget interfacepublic:
	
private:
	FOdysseyAnimationEditorExtension* mExtension;
	bool mIsSelecting = false;
	TAttribute<FInt32Range> mSelectableFrames;
    struct
    {
		bool mIsDragDetected;
		int mCursorFrame;
        FInt32Range mSelectedFrames;
    } mSelectionData;

	FOnSelectionChanged mOnSelectionChanged;
	FOnSelectionStarted mOnSelectionStarted;
	FOnSelectionEnded mOnSelectionEnded;
};