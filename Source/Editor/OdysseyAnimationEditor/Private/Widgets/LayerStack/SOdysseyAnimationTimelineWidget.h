// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class SOdysseyAnimationTimelinePanel;

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineWidget
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineWidget)
        : _BaseOffset(0.f)
        {}
        SLATE_ARGUMENT(float, BaseOffset)
    SLATE_END_ARGS()

public:
    SOdysseyAnimationTimelineWidget();

    void Construct(
        const FArguments& iArgs,
        TSharedPtr<SOdysseyAnimationLayerStack> iLayerStackWidget
    );

public:
    using FSlot = FBasicLayoutWidgetSlot;
    using FScopedWidgetSlotArguments = TPanelChildren<FSlot>::FScopedWidgetSlotArguments;

    TSharedPtr<SOdysseyAnimationLayerStack> GetLayerStackWidget() const;
    void ClearChildren();
    FScopedWidgetSlotArguments AddChild();

public:
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
    TWeakPtr<SOdysseyAnimationLayerStack> mLayerStackWidget;
    TSharedPtr<SOdysseyAnimationTimelinePanel> mPanel;
	FVector2D 					        mOffsetMousePosition;
	bool 						        mIsOffsetting;
    bool 						        mIsScrubbing;
};

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelinePanel
    : public SPanel
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelinePanel)
        : _BaseOffset(0.f)
        {}
        SLATE_ARGUMENT(float, BaseOffset)
    SLATE_END_ARGS()

public:
    SOdysseyAnimationTimelinePanel();

    void Construct(
        const FArguments& iArgs,
        TSharedPtr<SOdysseyAnimationLayerStack> iLayerStackWidget
    );

public:
    using FSlot = FBasicLayoutWidgetSlot;
    using FScopedWidgetSlotArguments = TPanelChildren<FSlot>::FScopedWidgetSlotArguments;

    TSharedPtr<SOdysseyAnimationLayerStack> GetLayerStackWidget() const;
    void ClearChildren();
    FScopedWidgetSlotArguments AddChild();

public:
    virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;
	virtual FChildren* GetChildren() override;

protected:
	// Begin SWidget overrides.
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	// End SWidget overrides.

private:
    float ArrangeChildHorizontalAndReturnOffset(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren, const SScrollBox::FSlot& ThisSlot, float CurChildOffset) const;

private:
    TWeakPtr<SOdysseyAnimationLayerStack> mLayerStackWidget;

	//State
	TPanelChildren<SScrollBox::FSlot>   mChildren;
    float mBaseOffset;
};
