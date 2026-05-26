// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyFlipbookTimelineFrame.h"

#include "Fonts/FontMeasure.h"
#include "Framework/Commands/GenericCommands.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"

#include "SOdysseyFlipbookTimelineFrameHandle.h"
#include "OdysseyStyle.h"

void SOdysseyFlipbookTimelineFrame::Construct( const SOdysseyFlipbookTimelineFrame::FArguments& InArgs )
{
    mFrameSize = InArgs._FrameSize;
    mLength = InArgs._Length;
    mMinLength = InArgs._MinLength;
    mMaxLength = InArgs._MaxLength;
    mOnGenerateContextMenu = InArgs._OnGenerateContextMenu;
    mOnDrop = InArgs._OnDrop;
    mOnDragOver = InArgs._OnDragOver;
    mOnDragDetected = InArgs._OnDragDetected;

    ChildSlot
    [
        SNew(SBox)
        .HeightOverride(50.f) //TODO: Move the size in an other widget or directly in the view or track itself
        .WidthOverride(this, &SOdysseyFlipbookTimelineFrame::GetWidthInPixels)
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            SAssignNew(mContentContainer, SBorder)
            .BorderImage(FOdysseyStyle::GetBrush("FlipbookTimeline.TimelineFrameBackground"))
            .BorderBackgroundColor(FLinearColor(1.f, 1.f, 1.f))
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Fill)
            [
                InArgs._Content.Widget
            ]
        ]
    ];
}

float
SOdysseyFlipbookTimelineFrame::FrameSize() const
{
    return mFrameSize.Get();
}

int32
SOdysseyFlipbookTimelineFrame::Length() const
{
    return mLength;
}

void
SOdysseyFlipbookTimelineFrame::Length(int32 iLength)
{
    if (mMinLength >= 0 && mMaxLength >= 0)
    {
        //Clamp
        iLength = FMath::Clamp(iLength, mMinLength, mMaxLength);
    }
    else if (mMinLength >= 0)
    {
        iLength = FMath::Max(iLength, mMinLength);
    }
    else if (mMaxLength >= 0)
    {
        iLength = FMath::Min(iLength, mMaxLength);
    }

    if (mLength != iLength)
    {
        mLength = iLength;
    }
}

int32
SOdysseyFlipbookTimelineFrame::MinLength() const
{
    return mMinLength;
}

void
SOdysseyFlipbookTimelineFrame::MinLength(int32 iLength)
{
    mMinLength = iLength;
    Length(mLength);
}


int32
SOdysseyFlipbookTimelineFrame::MaxLength() const
{
    return mMaxLength;
}

void
SOdysseyFlipbookTimelineFrame::MaxLength(int32 iLength)
{
    mMaxLength = iLength;
    Length(mLength);
}

FOptionalSize
SOdysseyFlipbookTimelineFrame::GetWidthInPixels() const
{
    return FOptionalSize(FrameSize() * Length());
}

void
SOdysseyFlipbookTimelineFrame::Content(TSharedPtr<SWidget> iWidget)
{
    mContentContainer->SetContent(iWidget.ToSharedRef());
}

TSharedPtr<SWidget>
SOdysseyFlipbookTimelineFrame::Content() const
{
    return mContentContainer->GetContent();
}

FReply
SOdysseyFlipbookTimelineFrame::OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
    if (iMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        return FReply::Handled().DetectDrag(SharedThis(this), EKeys::LeftMouseButton);
    }

    return FReply::Unhandled();
}

FReply
SOdysseyFlipbookTimelineFrame::OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
    if (mOnDragDetected.IsBound())
    {
        return mOnDragDetected.Execute(iGeometry, iMouseEvent);
    }
    return FReply::Unhandled();
}

void
SOdysseyFlipbookTimelineFrame::OnDragEnter(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent)
{
    if (mOnDragEnter.IsBound())
    {
        mOnDragEnter.Execute(iGeometry, iDragDropEvent);
    }
}

void
SOdysseyFlipbookTimelineFrame::OnDragLeave(const FDragDropEvent& iDragDropEvent)
{
    if (mOnDragLeave.IsBound())
    {
        mOnDragLeave.Execute(iDragDropEvent);
    }
}

FReply
SOdysseyFlipbookTimelineFrame::OnDragOver(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent)
{
    if (mOnDragOver.IsBound())
    {
        return mOnDragOver.Execute(iGeometry, iDragDropEvent);
    }
    return FReply::Unhandled();
}


FReply
SOdysseyFlipbookTimelineFrame::OnDrop(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent)
{
    if (mOnDrop.IsBound())
    {
        return mOnDrop.Execute(iGeometry, iDragDropEvent);
    }
    return FReply::Unhandled();
}

FReply
SOdysseyFlipbookTimelineFrame::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
    if (iMouseEvent.GetEffectingButton() == EKeys::RightMouseButton && mOnGenerateContextMenu.IsBound())
    {
        return mOnGenerateContextMenu.Execute(iGeometry, iMouseEvent);
    }
    return FReply::Unhandled();
}
