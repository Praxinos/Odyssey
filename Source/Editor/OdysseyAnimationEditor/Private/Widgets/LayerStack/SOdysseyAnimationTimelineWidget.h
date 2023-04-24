// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

/**
 * Implements a layer row widget
 */
class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineWidget
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineWidget)
        {}
    SLATE_END_ARGS()

public:
    void Construct(
        const FArguments& iArgs,
        TSharedPtr<SOdysseyAnimationLayerStack> iLayerStackWidget
    );

public:
    TSharedPtr<SOdysseyAnimationLayerStack> GetLayerStackWidget() const;

public:
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
    TWeakPtr<SOdysseyAnimationLayerStack> mLayerStackWidget;

	//State
	FVector2D 					mOffsetMousePosition;
	bool 						mIsOffsetting;
    bool 						mIsScrubbing;
};
