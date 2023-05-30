// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineControl.h"

SOdysseyAnimationTimelineControl::SOdysseyAnimationTimelineControl()
	: mEditor(nullptr)
	, mOffsetMousePosition(0)
	, mIsOffsetting(false)
{
}

void
SOdysseyAnimationTimelineControl::Construct(
    const FArguments& iArgs,
	FOdysseyAnimationEditor* iEditor
)
{
    mEditor = iEditor;
	ChildSlot
	[
		iArgs._Content.Widget
	];
}

int32
SOdysseyAnimationTimelineControl::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	// Draw a current frame
	LayerId = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	++LayerId;

	const FSlateBrush* GenericBrush = FCoreStyle::Get().GetBrush( "GenericWhiteBox" );

	const float height = AllottedGeometry.GetLocalSize().Y;  
	const float width = AllottedGeometry.GetLocalSize().X;
	float offset = mEditor->Timeline()->GetOffset();
	const float frameSize = mEditor->Timeline()->GetFrameWidth();

	FLinearColor lineColor = FLinearColor::Red;
	lineColor.A = 0.3f;

	int currentFrame = mEditor->Animation()->GetFrameIndexAtTime(mEditor->Player()->GetCurrentTime());
	float currentFramePos = (currentFrame - offset) * frameSize;

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(FVector2D(currentFramePos, 0.f), FVector2D(frameSize, height)),
		GenericBrush,
		ESlateDrawEffect::None,
		lineColor
	);

	return LayerId;
}

FReply 
SOdysseyAnimationTimelineControl::OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (MouseEvent.IsControlDown())
		{
			mIsOffsetting = true;
			mOffsetMousePosition = MouseEvent.GetScreenSpacePosition();
			mOffsetMousePosition.Y = mEditor->Timeline()->GetOffset();
    		return FReply::Handled().CaptureMouse(AsShared()).PreventThrottling();
		}
	}

	return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineControl::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (mIsOffsetting)
	{
		const float minOffset = 0.0f;
		float mouseOffset = MouseEvent.GetScreenSpacePosition().X - mOffsetMousePosition.X;
        //mOffsetMousePosition.Y contains the starting offset instead of the Y position
		mEditor->Timeline()->SetOffset(FMath::Max(minOffset, mOffsetMousePosition.Y - (mouseOffset / mEditor->Timeline()->GetFrameWidth())));
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineControl::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (mIsOffsetting)
	{
		mIsOffsetting = false;
    	return FReply::Handled().ReleaseMouseCapture();
	}
    return FReply::Unhandled();
}
