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
	FOdysseyAnimationEditor* iEditor
)
{
	mEditor = iEditor;

	ChildSlot
		[
			//use this scrollbox to display other widgets in the timeline header
			//for now there is no wodgets to display, but I can clearly imagine some
			SNew(SOdysseyAnimationTimelineScrollBox, iEditor)
		];
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
	float offset = mEditor->Timeline()->GetOffset();
	const float frameSize = mEditor->Timeline()->GetFrameWidth();
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

	LayerId = SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	return LayerId;
}

FReply SOdysseyAnimationTimelineHeader::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.IsControlDown())
	{
		if (MouseEvent.GetWheelDelta() > 0.f)
		{
			mEditor->Timeline()->ZoomOut();
		}
		else
		{
			mEditor->Timeline()->ZoomIn();
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
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		mIsScrubbing = true;

		const float minScrub = 0.0f;
		float frame = (MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X / mEditor->Timeline()->GetFrameWidth() + mEditor->Timeline()->GetOffset());
		FTimespan time = FTimespan::FromSeconds(frame / mEditor->Animation()->GetFramesPerSecond());
		mEditor->Player()->Stop();
		mEditor->Player()->SeekToTime(time);

		// This has prevent throttling on so that viewports continue to run whilst dragging the slider
		return FReply::Handled().CaptureMouse( SharedThis(this) ).PreventThrottling();
	}

	return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineHeader::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if(mIsScrubbing)
	{
		const float minScrub = 0.0f;
		float frame = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X / mEditor->Timeline()->GetFrameWidth() + mEditor->Timeline()->GetOffset();
		FTimespan time = FTimespan::FromSeconds(frame / mEditor->Animation()->GetFramesPerSecond());
		mEditor->Player()->SeekToTime(time);
		return FReply::Handled();
	}
	
	return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineHeader::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (mIsScrubbing)
	{
		const float minScrub = 0.0f;
		float frame = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X / mEditor->Timeline()->GetFrameWidth() + mEditor->Timeline()->GetOffset();
		FOdysseyObjectEditorUtils::SetPropertyValue(mEditor->Animation(), "CurrentFrame", (int)frame);
		
		mIsScrubbing = false;
		return FReply::Handled().ReleaseMouseCapture();
	}
	
	return FReply::Unhandled();
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
