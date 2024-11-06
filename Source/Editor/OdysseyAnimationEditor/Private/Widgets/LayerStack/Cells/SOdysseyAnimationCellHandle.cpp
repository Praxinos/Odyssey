// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/Cells/SOdysseyAnimationCellHandle.h"

void
SOdysseyAnimationCellHandle::Construct( const SOdysseyAnimationCellHandle::FArguments& InArgs)
{
    mOnDragStarted = InArgs._OnDragStarted;
    mOnDragged = InArgs._OnDragged;
    mOnDragStopped = InArgs._OnDragStopped;

    ChildSlot
    .HAlign(HAlign_Fill)
    .VAlign(VAlign_Fill)
    [
        InArgs._Content.Widget
    ];
}

FReply
SOdysseyAnimationCellHandle::OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
    if (iMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        mIsDragging = true;
        mOnDragStarted.ExecuteIfBound(iGeometry, iMouseEvent);
        return FReply::Handled().CaptureMouse(AsShared()).PreventThrottling();
    }
    return FReply::Unhandled();
}

FReply
SOdysseyAnimationCellHandle::OnMouseMove(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
    if (mIsDragging)
    {
        mOnDragged.ExecuteIfBound(iGeometry, iMouseEvent);
        return FReply::Handled();
    }
    return FReply::Unhandled();
}

FReply
SOdysseyAnimationCellHandle::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
    if (mIsDragging)
    {
        mIsDragging = false;
        mOnDragStopped.ExecuteIfBound(iGeometry, iMouseEvent);
        return FReply::Handled().ReleaseMouseCapture();
    }
    return FReply::Unhandled();
}

TOptional<EMouseCursor::Type>
SOdysseyAnimationCellHandle::GetCursor() const
{
    if (!IsEnabled())
        return TOptional<EMouseCursor::Type>();

    return EMouseCursor::ResizeLeftRight;
}