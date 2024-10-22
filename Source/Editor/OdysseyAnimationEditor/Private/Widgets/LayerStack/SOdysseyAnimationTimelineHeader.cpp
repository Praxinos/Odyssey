// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyAnimationTimelineHeader.h"
#include "Fonts/FontMeasure.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationPlayer.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineScrollBox.h"
#include "OdysseyStyleSet.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyAnimationProxy.h"

//////////////////////////////////////////////////////////////////////////
// SOdysseyAnimationTimelineHeader

void
SOdysseyAnimationTimelineHeader::Construct(const FArguments& InArgs)
{
	mAnimation = InArgs._Animation;
	mPlayer = InArgs._Player;
	mTimelinePosition = InArgs._TimelinePosition;

	ChildSlot
	[
		//use this scrollbox to display other widgets in the timeline header
		//for now there is no wodgets to display, but I can clearly imagine some
		SNew(SOdysseyAnimationTimelineScrollBox)
		.TimelinePosition(mTimelinePosition)
	];
}

int32 SOdysseyAnimationTimelineHeader::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const FSlateBrush* GenericBrush = FCoreStyle::Get().GetBrush( "GenericWhiteBox" );
	const FLinearColor& backgroundColorEven = FOdysseyStyle::GetColor("TimelineHeader.backgroundColorEven");
	const FLinearColor& backgroundColorOdd = FOdysseyStyle::GetColor("TimelineHeader.backgroundColorOdd");

	FAppStyle::GetBrush( TEXT( "ProgressBar.Background" ) );

	const int32 backgroundLayer = LayerId;
	const int32 textLayer = backgroundLayer + 1;
	const int32 proxyLayer = textLayer + 1;

	const FSlateFontInfo textFontInfo = FCoreStyle::GetDefaultFontStyle("Regular", 10);
	const FSlateBrush* backgroundBrush = FAppStyle::GetBrush( TEXT( "ProgressBar.Background" ) );

	const float height = AllottedGeometry.GetLocalSize().Y;  
	const float width = AllottedGeometry.GetLocalSize().X;
	const float frameSize = mTimelinePosition->GetFrameSize();
	float padding = mTimelinePosition->GetPadding() / frameSize; //fixed padding in pixels to display layer pre behaviour
	float offset = mTimelinePosition->GetOffset();
	const float frameNumberMinSize = 30.f;
	const int32 frameNumberFrequency = FMath::Max(1, FGenericPlatformMath::CeilToInt(frameNumberMinSize / frameSize));
	int32 startKey = FMath::Max(0, FGenericPlatformMath::FloorToInt(offset - padding ));
	int32 endKey = FMath::Max(0, FGenericPlatformMath::CeilToInt(offset - padding + (width / frameSize)));

	UOdysseyAnimation* animation = mAnimation;
	TSharedPtr<FOdysseyAnimationProxy> proxy = animation->GetProxy();
	FInt32Range animationRange = animation->GetFrameRange();

	for(int32 keyNum = startKey; keyNum <= endKey; keyNum++)
	{
		float x = (keyNum - offset + padding ) * frameSize;

		//Draw background
		const FColor backgroundColor = (keyNum & 1) ? backgroundColorOdd.ToFColor(true) : backgroundColorEven.ToFColor(true);
		const FVector2D pos(x, 0.f);
		const FVector2D size(frameSize, height);

		FSlateDrawElement::MakeBox(
			OutDrawElements,
			backgroundLayer,
        	AllottedGeometry.ToPaintGeometry( size, FSlateLayoutTransform( 1.0, TransformPoint( 1.0, pos ) ) ),
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
        		AllottedGeometry.ToPaintGeometry( textSize, FSlateLayoutTransform( 1.0, TransformPoint( 1.0, textPos ) ) ),
				frameString, 
				textFontInfo, 
				ESlateDrawEffect::None);
		}

		if (animationRange.Contains(keyNum))
		{
			bool isProxyDone = proxy->IsDone(keyNum);
			const FLinearColor& proxyDoneColor = FOdysseyStyle::GetColor("TimelineHeader.ProxyDoneColor");
			const FLinearColor& proxyPendingColor = FOdysseyStyle::GetColor("TimelineHeader.ProxyPendingColor");
			const FVector2D proxyPos(x, height - 2.f);
			const FVector2D proxySize(frameSize, 2.f);
			const FColor color = isProxyDone ? proxyDoneColor.ToFColor(true) : proxyPendingColor.ToFColor(true);
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				proxyLayer,
        		AllottedGeometry.ToPaintGeometry( proxySize, FSlateLayoutTransform( 1.0, TransformPoint( 1.0, proxyPos ) ) ),
				GenericBrush,
				ESlateDrawEffect::None,
				InWidgetStyle.GetColorAndOpacityTint() * color
			);
		}
	}

	LayerId = SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	return LayerId;
}

FReply 
SOdysseyAnimationTimelineHeader::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{	
	if (mPlayer && !mIsScrubbing && MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		mIsScrubbing = true;

		const float minScrub = 0.0f;
		float posX = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X;
		float frame = mTimelinePosition->MousePositionToFrame(posX);
		FTimespan time = FTimespan::FromSeconds(FMath::Max(0.f, frame) / mAnimation->GetFramesPerSecond());
		mPlayer->Stop();
		mPlayer->SetRenderType(IOdysseyImageRenderer::eRenderType::Render);
		mPlayer->SeekToTime(time);

		// This has prevent throttling on so that viewports continue to run whilst dragging the slider
		return FReply::Handled().CaptureMouse( SharedThis(this) ).PreventThrottling();
	}

	return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineHeader::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if(mPlayer && mIsScrubbing)
	{
		const float minScrub = 0.0f;
		float posX = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X;
		float frame = mTimelinePosition->MousePositionToFrame(posX);
		FTimespan time = FTimespan::FromSeconds(FMath::Max(0.f, frame) / mAnimation->GetFramesPerSecond());
		mPlayer->SeekToTime(time);
		return FReply::Handled();
	}
	
	return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineHeader::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (mPlayer && mIsScrubbing)
	{
		const float minScrub = 0.0f;
		float posX = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X;
		float frame = mTimelinePosition->MousePositionToFrame(posX);
		FOdysseyObjectEditorUtils::SetPropertyValue(mAnimation, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), FMath::Max(0, (int)frame));
		mPlayer->SetRenderType(IOdysseyImageRenderer::eRenderType::Editor);
		mIsScrubbing = false;
		return FReply::Handled().ReleaseMouseCapture();
	}
	
	return FReply::Unhandled();
}
