// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationEditor;
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineHeader
	: public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineHeader)
	{}
	SLATE_END_ARGS()

	void Construct(
		const FArguments& InArgs,
		FOdysseyAnimationEditor* iEditor);

	// SWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	// End of SWidget interfacepublic:

private:
	bool mIsScrubbing = false;
	FOdysseyAnimationEditor* mEditor;
};
