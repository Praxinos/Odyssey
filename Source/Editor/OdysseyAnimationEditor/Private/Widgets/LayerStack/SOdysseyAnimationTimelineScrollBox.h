// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class SOdysseyAnimationTimelineScrollPanel;
class FOdysseyAnimationEditor;

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineScrollBox
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineScrollBox)
        {}
    SLATE_END_ARGS()

public:
    SOdysseyAnimationTimelineScrollBox();

    void Construct(
        const FArguments& iArgs,
        FOdysseyAnimationEditor* iEditor
    );

public:
    using FSlot = FBasicLayoutWidgetSlot;
    using FScopedWidgetSlotArguments = TPanelChildren<FSlot>::FScopedWidgetSlotArguments;

    FOdysseyAnimationEditor* GetEditor() const;
    void ClearChildren();
    FScopedWidgetSlotArguments AddChild();

private:
    FOdysseyAnimationEditor*            mEditor;
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
        FOdysseyAnimationEditor* iEditor
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
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	// End SWidget overrides.

private:
    float ArrangeChildHorizontalAndReturnOffset(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren, const SScrollBox::FSlot& ThisSlot, float CurChildOffset) const;

private:
    FOdysseyAnimationEditor*            mEditor;

	//State
	TPanelChildren<SScrollBox::FSlot>   mChildren;
};
