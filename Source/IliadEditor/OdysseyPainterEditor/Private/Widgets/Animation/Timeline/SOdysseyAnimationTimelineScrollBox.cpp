// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineScrollBox.h"
#include "OdysseyPainterEditorAnimationTimelinePosition.h"
#include "Layout/LayoutUtils.h"

SOdysseyAnimationTimelineScrollBox::FSlot::FSlotArguments
SOdysseyAnimationTimelineScrollBox::Slot()
{
    return FSlot::FSlotArguments(MakeUnique<FSlot>());
}

SOdysseyAnimationTimelineScrollBox::SOdysseyAnimationTimelineScrollBox()
{
}

void
SOdysseyAnimationTimelineScrollBox::Construct(
    const FArguments& iArgs
)
{
    ChildSlot
    [
        SAssignNew(mPanel, SOdysseyAnimationTimelineScrollPanel, MoveTemp(const_cast<TArray<FSlot::FSlotArguments>&>(iArgs._Slots)))
        .TimelinePosition(iArgs._TimelinePosition)
        .Clipping(EWidgetClipping::ClipToBounds)
    ];
}

void
SOdysseyAnimationTimelineScrollBox::ClearChildren()
{
    mPanel->ClearChildren();
}

SOdysseyAnimationTimelineScrollBox::FScopedWidgetSlotArguments
SOdysseyAnimationTimelineScrollBox::AddChild()
{
    return mPanel->AddChild();
}

//=================
//=================
//=================

SOdysseyAnimationTimelineScrollPanel::SOdysseyAnimationTimelineScrollPanel()
    : mChildren(this)
{
}

void
SOdysseyAnimationTimelineScrollPanel::Construct(
    const FArguments& iArgs,
    TArray<SOdysseyAnimationTimelineScrollBox::FSlot::FSlotArguments> iSlots
)
{
    mTimelinePosition = iArgs._TimelinePosition;
    mChildren.AddSlots(MoveTemp(iSlots));
}

FChildren*
SOdysseyAnimationTimelineScrollPanel::GetChildren()
{
    return &mChildren;
}

void
SOdysseyAnimationTimelineScrollPanel::ClearChildren()
{
    mChildren.Empty();
}

SOdysseyAnimationTimelineScrollPanel::FScopedWidgetSlotArguments
SOdysseyAnimationTimelineScrollPanel::AddChild()
{
    return FScopedWidgetSlotArguments{ MakeUnique<FSlot>(), mChildren, INDEX_NONE };
}

FVector2D
SOdysseyAnimationTimelineScrollPanel::ComputeDesiredSize(float) const
{
    FVector2D desiredSize = FVector2D::ZeroVector;
    for (int32 SlotIndex = 0; SlotIndex < mChildren.Num(); ++SlotIndex)
    {
        const FSlot& slot = mChildren[SlotIndex];
        if (slot.GetWidget()->GetVisibility() != EVisibility::Collapsed)
        {
            const FVector2D childDesiredSize = slot.GetWidget()->GetDesiredSize();
            desiredSize.X += childDesiredSize.X + slot.GetPadding().GetTotalSpaceAlong<Orient_Horizontal>();
            desiredSize.Y = FMath::Max(childDesiredSize.Y, desiredSize.Y);
        }
    }
    return desiredSize;
}

void
SOdysseyAnimationTimelineScrollPanel::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
    float scrollPadding = AllottedGeometry.GetLocalSize().X;
    float padding = mTimelinePosition->GetPadding();
    float currentChildOffset = -mTimelinePosition->GetOffset() * mTimelinePosition->GetFrameSize() + padding;

    for (int32 SlotIndex = 0; SlotIndex < mChildren.Num(); ++SlotIndex)
    {
        const FSlot& slot = mChildren[SlotIndex];
        const EVisibility childVisibility = slot.GetWidget()->GetVisibility();

        if (childVisibility == EVisibility::Collapsed)
            continue;

        currentChildOffset = ArrangeChildHorizontalAndReturnOffset(AllottedGeometry, ArrangedChildren, slot, currentChildOffset);
    }
}

float
SOdysseyAnimationTimelineScrollPanel::ArrangeChildHorizontalAndReturnOffset(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren, const SOdysseyAnimationTimelineScrollBox::FSlot& ThisSlot, float CurChildOffset) const
{
    const FMargin& ThisPadding = ThisSlot.GetPadding();
    const FVector2D& WidgetDesiredSize = ThisSlot.GetWidget()->GetDesiredSize();
    const float ThisSlotDesiredWidth = WidgetDesiredSize.X + ThisPadding.GetTotalSpaceAlong<Orient_Horizontal>();

    // Figure out the size and local position of the child within the slot.  There is no horizontal alignment, because
    // it doesn't make sense in a panel where items are stacked horizontally end-to-end.
    AlignmentArrangeResult YAlignmentResult = AlignChild<Orient_Vertical>(AllottedGeometry.GetLocalSize().Y, ThisSlot, ThisPadding);

    ArrangedChildren.AddWidget(AllottedGeometry.MakeChild(ThisSlot.GetWidget(), FVector2D(CurChildOffset + ThisPadding.Left, YAlignmentResult.Offset), FVector2D(WidgetDesiredSize.X, YAlignmentResult.Size)));
    return CurChildOffset + ThisSlotDesiredWidth;
}
