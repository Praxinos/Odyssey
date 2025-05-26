// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

class SOdysseyAnimationTimelineScrollPanel;
class FOdysseyPainterEditorAnimationTimelinePosition;

class SOdysseyAnimationTimelineScrollBox
    : public SCompoundWidget
{
public:
    using FSlot = FBasicLayoutWidgetSlot;
    using FScopedWidgetSlotArguments = TPanelChildren<FSlot>::FScopedWidgetSlotArguments;

    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineScrollBox)
        {}
        SLATE_SLOT_ARGUMENT( FSlot, Slots )
        SLATE_ARGUMENT( TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>, TimelinePosition )
    SLATE_END_ARGS()

public:
    SOdysseyAnimationTimelineScrollBox();

    void Construct(const FArguments& iArgs);

public:
    /** @return a new slot. Slots contain children */
    static FSlot::FSlotArguments Slot();
    void ClearChildren();
    FScopedWidgetSlotArguments AddChild();

private:
    TSharedPtr<SOdysseyAnimationTimelineScrollPanel> mPanel;
};

class SOdysseyAnimationTimelineScrollPanel
    : public SPanel
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineScrollPanel)
        {}
        SLATE_ARGUMENT( TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>, TimelinePosition )
    SLATE_END_ARGS()

public:
    SOdysseyAnimationTimelineScrollPanel();

    void Construct(
        const FArguments& iArgs,
        TArray<SOdysseyAnimationTimelineScrollBox::FSlot::FSlotArguments> iSlots
    );

public:
    using FSlot = FBasicLayoutWidgetSlot;
    using FScopedWidgetSlotArguments = TPanelChildren<FSlot>::FScopedWidgetSlotArguments;

    void ClearChildren();
    FScopedWidgetSlotArguments AddChild();

public:
    virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;
    virtual FChildren* GetChildren() override;

protected:
    // Begin SWidget overrides.
    virtual FVector2D ComputeDesiredSize(float) const override;
    // End SWidget overrides.

private:
    float ArrangeChildHorizontalAndReturnOffset(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren, const SOdysseyAnimationTimelineScrollBox::FSlot& ThisSlot, float CurChildOffset) const;

private:
    //State
    TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition> mTimelinePosition;
    TPanelChildren<SOdysseyAnimationTimelineScrollBox::FSlot>   mChildren;
};
