// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class SOdysseyAnimationTimelineScrollPanel;
class FOdysseyAnimationEditorExtension;

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineScrollBox
    : public SCompoundWidget
{
public:
    using FSlot = FBasicLayoutWidgetSlot;
    using FScopedWidgetSlotArguments = TPanelChildren<FSlot>::FScopedWidgetSlotArguments;

    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineScrollBox)
        {}
        SLATE_SLOT_ARGUMENT( FSlot, Slots )
    SLATE_END_ARGS()

public:
    SOdysseyAnimationTimelineScrollBox();

    void Construct(
        const FArguments& iArgs,
        FOdysseyAnimationEditorExtension* iExtension
    );

public:
    /** @return a new slot. Slots contain children */
    static FSlot::FSlotArguments Slot();

    FOdysseyAnimationEditorExtension* GetExtension() const;
    void ClearChildren();
    FScopedWidgetSlotArguments AddChild();

private:
    FOdysseyAnimationEditorExtension*            mExtension;
    TSharedPtr<SOdysseyAnimationTimelineScrollPanel> mPanel;
};

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineScrollPanel
    : public SPanel
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineScrollPanel)
        {}
    SLATE_END_ARGS()

public:
    SOdysseyAnimationTimelineScrollPanel();

    void Construct(
        const FArguments& iArgs,
        TArray<SOdysseyAnimationTimelineScrollBox::FSlot::FSlotArguments> iSlots,
        FOdysseyAnimationEditorExtension* iExtension
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
    FOdysseyAnimationEditorExtension*            mExtension;

    //State
    TPanelChildren<SOdysseyAnimationTimelineScrollBox::FSlot>   mChildren;
};
