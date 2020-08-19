// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "SOdysseyTimeline.h"

#include "Fonts/FontMeasure.h"
#include "Widgets/Layout/SGridPanel.h"

#define LOCTEXT_NAMESPACE "OdysseyTimeline"

//////////////////////////////////////////////////////////////////////////
// SOdysseyTimeline

const float defaultFrameSize = 50.f; //in pixels
const float defaultHeight = 25.f; //in pixels

void SOdysseyTimeline::Construct(const FArguments& InArgs)
{
	mZoom = FMath::Clamp(InArgs._Zoom, 0.01f, 1.0f);
    mOffset = InArgs._Offset;
	mScrubPosition = InArgs._ScrubPosition;
	
	mOnScrubStarted = InArgs._OnScrubStarted;
    mOnScrubPositionChanged = InArgs._OnScrubPositionChanged;
    mOnScrubStopped = InArgs._OnScrubStopped;
	mOnOffsetChanged = InArgs._OnOffsetChanged;
	mOnZoomChanged = InArgs._OnZoomChanged;

	mContent = InArgs._Content.Widget;

	mIsOffsetting = false;
    mIsScrubbing = false;

	SAssignNew(mScrollBarH, SScrollBar)
		.Orientation(Orient_Horizontal)
		.AlwaysShowScrollbar(false);

	SAssignNew(mScrollBoxH, SScrollBox)
		.Orientation(Orient_Horizontal)
		.ExternalScrollbar(mScrollBarH)
		.OnUserScrolled(this, &SOdysseyTimeline::OnScrollBarUserScrolled)
		+ SScrollBox::Slot()
		[
			SNew(SBox)
			.MinDesiredWidth(this, &SOdysseyTimeline::GetScrollBoxHWidth)
			[
				mContent.ToSharedRef()
			]
		];

	ChildSlot
	.Padding(0.f ,defaultHeight, 0.f, 0.f)
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			mScrollBoxH.ToSharedRef()
		]
		+ SVerticalBox::Slot() //Spacer
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			mScrollBarH.ToSharedRef()
		]
	];
}

int32 SOdysseyTimeline::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const bool bActiveFeedback = IsHovered() || mIsScrubbing;

	const FSlateBrush* GenericBrush = FCoreStyle::Get().GetBrush( "GenericWhiteBox" );
	const FLinearColor& backgroundColorEven = FOdysseyStyle::GetColor("ScrubWidget.backgroundColorEven");
	const FLinearColor& backgroundColorOdd = FOdysseyStyle::GetColor("ScrubWidget.backgroundColorOdd");

	FEditorStyle::GetBrush( TEXT( "ProgressBar.Background" ) );

	const int32 backgroundLayer = LayerId;
	const int32 textLayer = backgroundLayer + 1;

	const FSlateFontInfo textFontInfo = FCoreStyle::GetDefaultFontStyle("Regular", 10);
	const FSlateBrush* backgroundBrush = FEditorStyle::GetBrush( TEXT( "ProgressBar.Background" ) );

	// const bool bEnabled = ShouldBeEnabled( bParentEnabled );
	// const ESlateDrawEffect DrawEffects = bEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

	const float height = AllottedGeometry.GetLocalSize().Y;
	const float width = AllottedGeometry.GetLocalSize().X;
	const float contentWidth = mContent->GetCachedGeometry().GetLocalSize().X;
	float offsetPercent = mScrollBarH->DistanceFromTop();
	float offset = FMath::Max(0.0f, (offsetPercent * contentWidth / FrameSize()));
	const float frameSize = FrameSize();
	const float frameNumberMinSize = 30.f;
	const int32 frameNumberFrequency = FMath::Max(1, FGenericPlatformMath::CeilToInt(frameNumberMinSize / frameSize));
	int32 startKey = FGenericPlatformMath::FloorToInt(offset);
	int32 endKey = FGenericPlatformMath::CeilToInt(offset + (width / frameSize));
	for(int32 keyNum = startKey; keyNum <= endKey; keyNum++)
	{
		float x = (keyNum - offset) * frameSize;

		//Draw background
		const FColor backgroundColor = (keyNum & 1) ? backgroundColorOdd.ToFColor(true) : backgroundColorEven.ToFColor(true);
		const FVector2D pos(x, 0.f);
		const FVector2D size(frameSize, height);

		FSlateDrawElement::MakeBox(
			OutDrawElements,
			backgroundLayer,
			AllottedGeometry.ToPaintGeometry(pos, size),
			GenericBrush,
			ESlateDrawEffect::None,
			InWidgetStyle.GetColorAndOpacityTint() * backgroundColor
		);

		//Draw key line
		/* const FVector2D pos(x, 0.f);
		const FVector2D size(1, height);
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			backgroundLayer,
			AllottedGeometry.ToPaintGeometry(pos, size),
			backgroundBrush,
			ESlateDrawEffect::None,
			InWidgetStyle.GetColorAndOpacityTint()
			); */



		//Draw key num
		if (!(keyNum % frameNumberFrequency))
		{
			const FString frameString = FString::Printf(TEXT("%d"), keyNum);
			const FVector2D textPos(x + 2.f, 0.f);

			const TSharedRef< FSlateFontMeasure > fontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
			const FVector2D textSize = fontMeasureService->Measure(frameString, textFontInfo);

			FSlateDrawElement::MakeText(
				OutDrawElements,
				textLayer,
				AllottedGeometry.ToPaintGeometry(textPos, textSize),
				frameString, 
				textFontInfo, 
				ESlateDrawEffect::None);
		}
	}

	LayerId = SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	// Draw a line for the current scrub cursor
	++LayerId;
	
	const float linePosition = ( mScrubPosition - offset) * frameSize;

	TArray<FVector2D> LinePoints;
	LinePoints.Add(FVector2D(linePosition, 0.f));
	LinePoints.Add(FVector2D(linePosition, AllottedGeometry.GetLocalSize().Y));

	FSlateDrawElement::MakeLines(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(),
		LinePoints,
		ESlateDrawEffect::None,
		FLinearColor::Red
	);

	return LayerId;
}

FReply SOdysseyTimeline::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.IsControlDown())
	{
		const float minZoom = 0.01f;
		const float maxZoom = 1.0f;
		const float directionScale = 0.08f;
		const float direction = MouseEvent.GetWheelDelta();

		Zoom(mZoom * (1.0f + direction * directionScale));
		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

FReply SOdysseyTimeline::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (MouseEvent.IsControlDown())
		{
			mIsOffsetting = true;
			mOffsetMousePosition = MouseEvent.GetScreenSpacePosition();
			mOffsetMousePosition.Y = mOffset;
			return FReply::Handled();
		}
		else 
		{
			mIsScrubbing = true;
			mOnScrubStarted.ExecuteIfBound();

			const float minScrub = 0.0f;
			ScrubPosition(MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X / FrameSize() + mOffset);

			// This has prevent throttling on so that viewports continue to run whilst dragging the slider
			return FReply::Handled().CaptureMouse( SharedThis(this) ).PreventThrottling();
		}
	}
	else
	{
		return FReply::Unhandled();
	}
}

FReply SOdysseyTimeline::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (mIsOffsetting)
	{
		const float minOffset = 0.0f;
		float mouseOffset = MouseEvent.GetScreenSpacePosition().X - mOffsetMousePosition.X;
		Offset(FMath::Max(minOffset, mOffsetMousePosition.Y - (mouseOffset / FrameSize()))); //mOffsetMousePosition.Y contains the starting offset instead of the Y position
		return FReply::Handled();
	}
	else if(mIsScrubbing)
	{
		const float minScrub = 0.0f;
		ScrubPosition(MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X / FrameSize() + mOffset);
		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

FReply SOdysseyTimeline::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (mIsOffsetting)
	{
		mIsOffsetting = false;
		return FReply::Handled();
	}
	else if (mIsScrubbing)
	{
		const float minScrub = 0.0f;
		ScrubPosition(MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X / FrameSize() + mOffset);
		
		mIsScrubbing = false;
		mOnScrubStopped.ExecuteIfBound();
		return FReply::Handled().ReleaseMouseCapture();
	}
	else
	{
		return FReply::Unhandled();
	}
}

void
SOdysseyTimeline::OnScrollBarUserScrolled(float iScrollOffset)
{
	Offset(FMath::Max(0.0f, iScrollOffset / FrameSize()));
}

// 1.0 = 100%
float
SOdysseyTimeline::Zoom() const
{
	return mZoom;
}

void
SOdysseyTimeline::Zoom(float iZoom)
{
	const float oldZoom = mZoom;
	mZoom = FMath::Clamp(iZoom, 0.01f, 1.0f);
	mOnZoomChanged.ExecuteIfBound(oldZoom);
}

float
SOdysseyTimeline::Offset() const
{
	return mOffset;
}

void
SOdysseyTimeline::Offset(float iOffset)
{
	const float oldOffset = mOffset;
	mOffset = iOffset;
	mScrollBoxH->SetScrollOffset(iOffset * FrameSize());
	mOnOffsetChanged.ExecuteIfBound(oldOffset);
}

float
SOdysseyTimeline::ScrubPosition() const
{
	return mScrubPosition;
}

void
SOdysseyTimeline::ScrubPosition(float iPosition)
{
	float oldPosition = mScrubPosition;
	mScrubPosition = FMath::Max(iPosition, 0.0f);
	mOnScrubPositionChanged.ExecuteIfBound(oldPosition);
}

// Offset of the timeline in seconds
float
SOdysseyTimeline::FrameSize() const
{
	return defaultFrameSize * mZoom;
}

bool
SOdysseyTimeline::IsScrubbing() const
{
	return mIsScrubbing;
}

FOptionalSize
SOdysseyTimeline::GetScrollBoxHWidth() const
{
	if (!mScrollBoxH.IsValid())
		return FOptionalSize(0.0f);

	float hSize = mScrollBoxH->GetCachedGeometry().GetLocalSize().X - 1.0f;
	return FOptionalSize(hSize);
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
