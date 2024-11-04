// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineControl
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineControl)
        {
			_Visibility = EVisibility::SelfHitTestInvisible;
		}
        SLATE_DEFAULT_SLOT( FArguments, Content )
		SLATE_ARGUMENT(UOdysseyAnimation*, Animation)
		SLATE_ATTRIBUTE(int, CurrentFrame)
		SLATE_ARGUMENT( TSharedPtr<FOdysseyAnimationEditorTimelinePosition>, TimelinePosition )
		SLATE_ATTRIBUTE( FInt32Range, CustomValidRange)
    SLATE_END_ARGS()

public:
    SOdysseyAnimationTimelineControl();

    void Construct(const FArguments& iArgs);

public:
    virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FNavigationReply OnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent) override;
    virtual bool SupportsKeyboardFocus() const override { return true; }
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

private:
    UOdysseyAnimation* mAnimation;
	TSharedPtr<FOdysseyAnimationEditorTimelinePosition> mTimelinePosition;
    TAttribute<int> mCurrentFrame;
	TAttribute<FInt32Range> mCustomValidRange;
    
    FVector2D                             mOffsetMousePosition;
    bool                                 mIsOffsetting;
    
    FVector2D                             mZoomMousePosition;
    float                                 mZoomInitialValue;
    float                                 mOffsetInitialValue;
    float                               mInitialValueFrameWidth;

    bool                                 mIsZooming;
};
