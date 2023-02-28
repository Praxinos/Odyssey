// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyAnimationTimelineHeader.h"
#include "Fonts/FontMeasure.h"

#include "OdysseyAnimation.h"
#include "MediaPlayer.h"

#define LOCTEXT_NAMESPACE "OdysseyTimeline"

//////////////////////////////////////////////////////////////////////////
// SOdysseyAnimationTimelineHeader

void
SOdysseyAnimationTimelineHeader::Construct(const FArguments& InArgs)
{
	mAnimation = InArgs._Animation;
	mMediaPlayer = InArgs._MediaPlayer;
	mZoom = InArgs._Zoom;
    mOffset = InArgs._Offset;
	mFrameWidth = InArgs._FrameWidth;
	
	mOnOffsetChanged = InArgs._OnOffsetChanged;
	mOnZoomChanged = InArgs._OnZoomChanged;

	mIsOffsetting = false;
    mIsScrubbing = false;

	/* ChildSlot
	[
	]; */
}

int32 SOdysseyAnimationTimelineHeader::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const bool bActiveFeedback = IsHovered() || mIsScrubbing;

	const FSlateBrush* GenericBrush = FCoreStyle::Get().GetBrush( "GenericWhiteBox" );
	const FLinearColor& backgroundColorEven = FOdysseyStyle::GetColor("ScrubWidget.backgroundColorEven");
	const FLinearColor& backgroundColorOdd = FOdysseyStyle::GetColor("ScrubWidget.backgroundColorOdd");

	FAppStyle::GetBrush( TEXT( "ProgressBar.Background" ) );

	const int32 backgroundLayer = LayerId;
	const int32 textLayer = backgroundLayer + 1;

	const FSlateFontInfo textFontInfo = FCoreStyle::GetDefaultFontStyle("Regular", 10);
	const FSlateBrush* backgroundBrush = FAppStyle::GetBrush( TEXT( "ProgressBar.Background" ) );

	// const bool bEnabled = ShouldBeEnabled( bParentEnabled );
	// const ESlateDrawEffect DrawEffects = bEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

	const float height = AllottedGeometry.GetLocalSize().Y;
	const float width = AllottedGeometry.GetLocalSize().X;
	float offset = mOffset.Get();
	const float frameSize = mFrameWidth.Get() * mZoom.Get();
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

	// Draw a current frame
	++LayerId;

	FLinearColor lineColor = FLinearColor::Red;
	lineColor.A = 0.3f;

	int currentFrame = mAnimation->CurrentFrame;
	float currentFramePos = currentFrame * frameSize;

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(FVector2D(currentFramePos, 0.f), FVector2D(frameSize, height)),
		GenericBrush,
		ESlateDrawEffect::None,
		lineColor
	);

	//Draw Current Time

	float currentTime = mMediaPlayer->GetTime().GetTotalSeconds() * mAnimation->GetFramesPerSecond();
	float currentTimePos = currentTime * frameSize;

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(FVector2D(currentTimePos, 0.f), FVector2D(1.f, height)),
		GenericBrush,
		ESlateDrawEffect::None,
		FLinearColor::Red
	);

	/*
	
	const float linePosition = ( mScrubPosition - offset) * frameSize;
	
	FLinearColor lineColor = FLinearColor::Red;
	lineColor.A = 0.3f;

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(FVector2D(linePosition - 8.0f, 0.f), FVector2D(17.0f, height)),
		GenericBrush,
		ESlateDrawEffect::None,
		lineColor
	);

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(FVector2D(linePosition - 1.0f, 0.f), FVector2D(3.0f, height)),
		GenericBrush,
		ESlateDrawEffect::None,
		FLinearColor::Red
	);*/

	return LayerId;
}

FReply SOdysseyAnimationTimelineHeader::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.IsControlDown())
	{
		const float minZoom = 0.01f;
		const float maxZoom = 1.0f;
		const float directionScale = 0.08f;
		const float direction = MouseEvent.GetWheelDelta();

		float zoom = mZoom.Get() * (1.0f + direction * directionScale);
		mOnZoomChanged.ExecuteIfBound(zoom);
		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

FReply SOdysseyAnimationTimelineHeader::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (MouseEvent.IsControlDown())
		{
			mIsOffsetting = true;
			mOffsetMousePosition = MouseEvent.GetScreenSpacePosition();
			mOffsetMousePosition.Y = mOffset.Get();
			return FReply::Handled();
		}
		else 
		{
			mIsScrubbing = true;

			const float minScrub = 0.0f;
			float frame = (MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X / mFrameWidth.Get() + mOffset.Get());
			FTimespan time = FTimespan::FromSeconds(frame / mAnimation->GetFramesPerSecond());
			mMediaPlayer->Pause();
			mMediaPlayer->Seek(time);

			// This has prevent throttling on so that viewports continue to run whilst dragging the slider
			return FReply::Handled().CaptureMouse( SharedThis(this) ).PreventThrottling();
		}
	}
	else
	{
		return FReply::Unhandled();
	}
}

FReply SOdysseyAnimationTimelineHeader::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (mIsOffsetting)
	{
		const float minOffset = 0.0f;
		float mouseOffset = MouseEvent.GetScreenSpacePosition().X - mOffsetMousePosition.X;
		float offset = FMath::Max(minOffset, mOffsetMousePosition.Y - (mouseOffset / mFrameWidth.Get())); //mOffsetMousePosition.Y contains the starting offset instead of the Y position
		mOnOffsetChanged.ExecuteIfBound(offset);
		return FReply::Handled();
	}
	else if(mIsScrubbing)
	{
		const float minScrub = 0.0f;
		float frame = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X / mFrameWidth.Get() + mOffset.Get();
		FTimespan time = FTimespan::FromSeconds(frame / mAnimation->GetFramesPerSecond());
		mMediaPlayer->Seek(time);
		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

FReply SOdysseyAnimationTimelineHeader::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (mIsOffsetting)
	{
		mIsOffsetting = false;
		return FReply::Handled();
	}
	else if (mIsScrubbing)
	{
		const float minScrub = 0.0f;
		float frame = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X / mFrameWidth.Get() + mOffset.Get();
		FTimespan time = FTimespan::FromSeconds(frame / mAnimation->GetFramesPerSecond());
		mMediaPlayer->Seek(time);
		FOdysseyObjectEditorUtils::SetPropertyValue(mAnimation, "CurrentFrame", (int)frame);
		
		mIsScrubbing = false;
		return FReply::Handled().ReleaseMouseCapture();
	}
	else
	{
		return FReply::Unhandled();
	}
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
