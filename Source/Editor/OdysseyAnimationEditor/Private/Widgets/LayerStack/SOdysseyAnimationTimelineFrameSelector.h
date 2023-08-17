// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationEditorExtension;
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineFrameSelector
	: public SCompoundWidget
{
public:
    DECLARE_DELEGATE_OneParam(FOnBuildContextMenu, FMenuBuilder&)
    DECLARE_DELEGATE_OneParam(FOnMapActions, TSharedPtr<FUICommandList>)

public:
	SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineFrameSelector)
	{}
        SLATE_EVENT(FOnBuildContextMenu, OnBuildContextMenu)
        SLATE_EVENT(FOnMapActions, OnMapActions)
	SLATE_END_ARGS()

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

private:
	bool mIsSelecting = false;
	FOdysseyAnimationEditorExtension* mExtension;

    struct
    {
		int mCursorFrame;
        FInt32Range mSelectedFrames;
    } mSelectionData;

	FOnBuildContextMenu mOnBuildContextMenu;
    FOnMapActions mOnMapActions;
};
