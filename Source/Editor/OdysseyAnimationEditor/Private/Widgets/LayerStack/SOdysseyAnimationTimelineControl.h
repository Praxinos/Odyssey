// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationEditorExtension;

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineControl
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineControl)
        {}
        SLATE_DEFAULT_SLOT( FArguments, Content )
    SLATE_END_ARGS()

public:
    SOdysseyAnimationTimelineControl();

    void Construct(
        const FArguments& iArgs,
        FOdysseyAnimationEditorExtension* iExtension
    );

public:
    virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FNavigationReply OnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent) override;
    virtual bool SupportsKeyboardFocus() const override { return true; }

private:
    FOdysseyAnimationEditorExtension*            mExtension;
	FVector2D 					        mOffsetMousePosition;
	bool 						        mIsOffsetting;
};
