// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationEditorExtension;
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineFrameSelector
	: public SCompoundWidget
{
public:
	DECLARE_DELEGATE_OneParam(FOnSelectionChanged, FInt32Range)
	DECLARE_DELEGATE_OneParam(FOnSelectionStarted, int /* iFrame */)
	DECLARE_DELEGATE_OneParam(FOnSelectionEnded, int /* iFrame */)
	DECLARE_DELEGATE_RetVal(FReply, FOnSelectionDragged)

public:
	SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineFrameSelector)
		: _Content()
	{}
		SLATE_DEFAULT_SLOT( FArguments, Content )
		SLATE_ATTRIBUTE(FInt32Range, SelectedFrames)
		SLATE_ATTRIBUTE(FInt32Range, SelectableFrames)
		SLATE_EVENT(FOnSelectionStarted, OnSelectionStarted)
		SLATE_EVENT(FOnSelectionEnded, OnSelectionEnded)
		SLATE_EVENT(FOnSelectionChanged, OnSelectionChanged)
		SLATE_EVENT(FOnSelectionDragged, OnSelectionDragged)
	SLATE_END_ARGS()

	SOdysseyAnimationTimelineFrameSelector();

	void Construct(
		const FArguments& InArgs,
		FOdysseyAnimationEditorExtension* iExtension);

	// SWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	// End of SWidget interfacepublic:

private:
	bool GetSelectedFrames(int& oStartFrame, int& oEndFrame) const;
	void OnFrameSelectionChanged(FInt32Range iSelectedFrames);
	void OnFrameSelectionStarted(int iFrame);
	void OnFrameSelectionEnded(int iFrame);

	FReply OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	FReply OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) override;
	//FReply OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) override;

private:
	TAttribute<FInt32Range> mSelectedFrames;
	TAttribute<FInt32Range> mSelectableFrames;
	
	FOdysseyAnimationEditorExtension* mExtension;
	TSharedRef<FUICommandList> mCommandList;
	
	struct FSeletionData
	{
		bool mIsSelecting = false;
		FInt32Range mSelectedFrames;
	} mSelectionData;

	FOnSelectionChanged mOnSelectionChanged;
	FOnSelectionStarted mOnSelectionStarted;
	FOnSelectionEnded mOnSelectionEnded;
	FOnSelectionDragged mOnSelectionDragged;
};
