// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "SOdysseyFlipbookTimelineFrameHandle.h"

#include "Fonts/FontMeasure.h"

void SOdysseyFlipbookTimelineFrameHandle::Construct( const SOdysseyFlipbookTimelineFrameHandle::FArguments& InArgs )
{
    mFrameSize = InArgs._FrameSize;
    mIsDragging = false;
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
SOdysseyFlipbookTimelineFrameHandle::OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
    if (iMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        mDragScreenSpacePosition = iMouseEvent.GetScreenSpacePosition();
        mIsDragging = true;
        mOnDragStarted.ExecuteIfBound();
        return FReply::Handled().CaptureMouse(this->AsShared());
    }
    return FReply::Unhandled();
}

FReply
SOdysseyFlipbookTimelineFrameHandle::OnMouseMove(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
    if (mIsDragging)
    {
        float offset = iMouseEvent.GetScreenSpacePosition().X - mDragScreenSpacePosition.X;
        int32 frameOffset = FGenericPlatformMath::RoundToInt(offset / mFrameSize.Get());
        mOnDragged.ExecuteIfBound(frameOffset);
        return FReply::Handled();
    }
    return FReply::Unhandled();
}

FReply
SOdysseyFlipbookTimelineFrameHandle::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
    if (mIsDragging)
    {
        mIsDragging = false;
        mOnDragStopped.ExecuteIfBound();
        return FReply::Handled().ReleaseMouseCapture();
    }
    return FReply::Unhandled();
}


float
SOdysseyFlipbookTimelineFrameHandle::FrameSize() const
{
    return mFrameSize.Get();
}
