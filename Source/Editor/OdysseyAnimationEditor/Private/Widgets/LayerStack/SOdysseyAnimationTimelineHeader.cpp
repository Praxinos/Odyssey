// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyAnimationTimelineHeader.h"
#include "Fonts/FontMeasure.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationPlayer.h"

#define LOCTEXT_NAMESPACE "OdysseyTimeline"

//////////////////////////////////////////////////////////////////////////
// SOdysseyAnimationTimelineHeader

void
SOdysseyAnimationTimelineHeader::Construct(
	const FArguments& InArgs,
	TSharedPtr<SOdysseyAnimationLayerStack> iLayerStackWidget
)
{
	SOdysseyAnimationTimelineWidget::Construct(
		SOdysseyAnimationTimelineWidget::FArguments(),
		iLayerStackWidget
	);
}

int32 SOdysseyAnimationTimelineHeader::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const FSlateBrush* GenericBrush = FCoreStyle::Get().GetBrush( "GenericWhiteBox" );
	const FLinearColor& backgroundColorEven = FOdysseyStyle::GetColor("ScrubWidget.backgroundColorEven");
	const FLinearColor& backgroundColorOdd = FOdysseyStyle::GetColor("ScrubWidget.backgroundColorOdd");

	FAppStyle::GetBrush( TEXT( "ProgressBar.Background" ) );

	const int32 backgroundLayer = LayerId;
	const int32 textLayer = backgroundLayer + 1;

	const FSlateFontInfo textFontInfo = FCoreStyle::GetDefaultFontStyle("Regular", 10);
	const FSlateBrush* backgroundBrush = FAppStyle::GetBrush( TEXT( "ProgressBar.Background" ) );

	const float height = AllottedGeometry.GetLocalSize().Y;  
	const float width = AllottedGeometry.GetLocalSize().X;
	float offset = GetLayerStackWidget()->GetTimelineOffset();
	const float frameSize = GetLayerStackWidget()->GetTimelineFrameWidth();
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

	LayerId = SOdysseyAnimationTimelineWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	return LayerId;
}

FReply SOdysseyAnimationTimelineHeader::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.IsControlDown())
	{
		if (MouseEvent.GetWheelDelta() > 0.f)
		{
			GetLayerStackWidget()->TimelineZoomOut();
		}
		else
		{
			GetLayerStackWidget()->TimelineZoomIn();
		}
		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

FReply 
SOdysseyAnimationTimelineHeader::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply reply = SOdysseyAnimationTimelineWidget::OnMouseButtonDown(MyGeometry, MouseEvent);
	if (reply.IsEventHandled())
		return reply;
	
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		mIsScrubbing = true;

		const float minScrub = 0.0f;
		float frame = (MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X / GetLayerStackWidget()->GetTimelineFrameWidth() + GetLayerStackWidget()->GetTimelineOffset());
		FTimespan time = FTimespan::FromSeconds(frame / GetLayerStackWidget()->GetAnimation()->GetFramesPerSecond());
		GetLayerStackWidget()->GetPlayer()->Stop();
		GetLayerStackWidget()->GetPlayer()->SeekToTime(time);

		// This has prevent throttling on so that viewports continue to run whilst dragging the slider
		return FReply::Handled().CaptureMouse( SharedThis(this) ).PreventThrottling();
	}

	return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineHeader::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply reply = SOdysseyAnimationTimelineWidget::OnMouseMove(MyGeometry, MouseEvent);
	if (reply.IsEventHandled())
		return reply;

	if(mIsScrubbing)
	{
		const float minScrub = 0.0f;
		float frame = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X / GetLayerStackWidget()->GetTimelineFrameWidth() + GetLayerStackWidget()->GetTimelineOffset();
		FTimespan time = FTimespan::FromSeconds(frame / GetLayerStackWidget()->GetAnimation()->GetFramesPerSecond());
		GetLayerStackWidget()->GetPlayer()->SeekToTime(time);
		return FReply::Handled();
	}
	
	return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineHeader::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply reply = SOdysseyAnimationTimelineWidget::OnMouseButtonUp(MyGeometry, MouseEvent);
	if (reply.IsEventHandled())
		return reply;

	if (mIsScrubbing)
	{
		const float minScrub = 0.0f;
		float frame = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X / GetLayerStackWidget()->GetTimelineFrameWidth() + GetLayerStackWidget()->GetTimelineOffset();
		FOdysseyObjectEditorUtils::SetPropertyValue(GetLayerStackWidget()->GetAnimation(), "CurrentFrame", (int)frame);
		
		mIsScrubbing = false;
		return FReply::Handled().ReleaseMouseCapture();
	}
	
	return FReply::Unhandled();
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
