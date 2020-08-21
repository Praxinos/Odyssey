// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "SOdysseyTimelineFrame.h"

#include "Fonts/FontMeasure.h"
#include "Framework/Commands/GenericCommands.h"

#include "SOdysseyTimelineFrameHandle.h"

#define LOCTEXT_NAMESPACE "OdysseyTimeline"

void SOdysseyTimelineFrame::Construct( const SOdysseyTimelineFrame::FArguments& InArgs )
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
		.WidthOverride(this, &SOdysseyTimelineFrame::GetWidthInPixels)
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
SOdysseyTimelineFrame::FrameSize() const
{
	return mFrameSize.Get();
}

int32
SOdysseyTimelineFrame::Length() const
{
	return mLength;
}

void
SOdysseyTimelineFrame::Length(int32 iLength)
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
SOdysseyTimelineFrame::MinLength() const
{
	return mMinLength;
}

void
SOdysseyTimelineFrame::MinLength(int32 iLength)
{
	mMinLength = iLength;
	Length(mLength);
}


int32
SOdysseyTimelineFrame::MaxLength() const
{
	return mMaxLength;
}

void
SOdysseyTimelineFrame::MaxLength(int32 iLength)
{
	mMaxLength = iLength;
	Length(mLength);
}

FOptionalSize
SOdysseyTimelineFrame::GetWidthInPixels() const
{
	return FOptionalSize(FrameSize() * Length());
}

void
SOdysseyTimelineFrame::Content(TSharedPtr<SWidget> iWidget)
{
	mContentContainer->SetContent(iWidget.ToSharedRef());
}

TSharedPtr<SWidget>
SOdysseyTimelineFrame::Content() const
{
	return mContentContainer->GetContent();
}

FReply
SOdysseyTimelineFrame::OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
	if (iMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return FReply::Handled().DetectDrag(SharedThis(this), EKeys::LeftMouseButton);
	}

	return FReply::Unhandled();
}

FReply
SOdysseyTimelineFrame::OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
	if (mOnDragDetected.IsBound())
	{
		return mOnDragDetected.Execute(iGeometry, iMouseEvent);
	}
	return FReply::Unhandled();
}

void
SOdysseyTimelineFrame::OnDragEnter(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent)
{
	if (mOnDragEnter.IsBound())
	{
		mOnDragEnter.Execute(iGeometry, iDragDropEvent);
	}
}

void
SOdysseyTimelineFrame::OnDragLeave(const FDragDropEvent& iDragDropEvent)
{
	if (mOnDragLeave.IsBound())
	{
		mOnDragLeave.Execute(iDragDropEvent);
	}
}

FReply
SOdysseyTimelineFrame::OnDragOver(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent)
{
	if (mOnDragOver.IsBound())
	{
		return mOnDragOver.Execute(iGeometry, iDragDropEvent);
	}
	return FReply::Unhandled();
}


FReply
SOdysseyTimelineFrame::OnDrop(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent)
{
	if (mOnDrop.IsBound())
	{
		return mOnDrop.Execute(iGeometry, iDragDropEvent);
	}
	return FReply::Unhandled();
}

FReply
SOdysseyTimelineFrame::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
	if (iMouseEvent.GetEffectingButton() == EKeys::RightMouseButton && mOnGenerateContextMenu.IsBound())
	{
		return mOnGenerateContextMenu.Execute(iGeometry, iMouseEvent);
	}
	return FReply::Unhandled();
}

#undef LOCTEXT_NAMESPACE 
