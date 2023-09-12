// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationEditorExtension;
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineFrameSelector
	: public SCompoundWidget
{
public:
    DECLARE_DELEGATE_TwoParams(FOnBuildContextMenu, FMenuBuilder& /*iMenuBuilder*/, int /*iFrameIndex*/)

public:
	SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineFrameSelector)
	{}
        SLATE_EVENT(FOnBuildContextMenu, OnBuildContextMenu)
	SLATE_END_ARGS()

	SOdysseyAnimationTimelineFrameSelector();

	void Construct(
		const FArguments& InArgs,
		FOdysseyAnimationEditorExtension* iExtension);

	// SWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	// End of SWidget interfacepublic:

private:
	bool GetSelectedFrames(int& oStartFrame, int& oEndFrame) const;
	void MapActions(TSharedPtr<FUICommandList> iCommandList);
	void BuildContextMenu(FMenuBuilder& iMenuBuilder, int iFrame);

private:
	bool mIsSelecting = false;
	FOdysseyAnimationEditorExtension* mExtension;
	TSharedRef<FUICommandList> mCommandList;

    struct
    {
		int mCursorFrame;
        FInt32Range mSelectedFrames;
    } mSelectionData;

	FOnBuildContextMenu mOnBuildContextMenu;
};
