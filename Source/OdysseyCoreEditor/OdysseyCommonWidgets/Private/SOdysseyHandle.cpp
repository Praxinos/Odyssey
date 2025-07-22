// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyHandle.h"

void
SOdysseyHandle::Construct( const SOdysseyHandle::FArguments& InArgs)
{
    mIsDraggable = InArgs._IsDraggable;
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
SOdysseyHandle::OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
    if (!mIsDraggable.Get())
        return FReply::Unhandled();

    if (iMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        mIsDragging = true;
        mOnDragStarted.ExecuteIfBound(iGeometry, iMouseEvent);
        return FReply::Handled().CaptureMouse(AsShared()).PreventThrottling();
    }
    return FReply::Unhandled();
}

FReply
SOdysseyHandle::OnMouseMove(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
    if (!mIsDraggable.Get())
        return FReply::Unhandled();

    if (mIsDragging)
    {
        mOnDragged.ExecuteIfBound(iGeometry, iMouseEvent);
        return FReply::Handled();
    }
    return FReply::Unhandled();
}

FReply
SOdysseyHandle::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
    if (!mIsDraggable.Get())
        return FReply::Unhandled();

    if (mIsDragging)
    {
        mIsDragging = false;
        mOnDragStopped.ExecuteIfBound(iGeometry, iMouseEvent);
        return FReply::Handled().ReleaseMouseCapture();
    }
    return FReply::Unhandled();
}

TOptional<EMouseCursor::Type>
SOdysseyHandle::GetCursor() const
{
    if (!mIsDraggable.Get())
        return TOptional<EMouseCursor::Type>();

    if (!IsEnabled())
        return TOptional<EMouseCursor::Type>();

    return EMouseCursor::ResizeLeftRight;
}
