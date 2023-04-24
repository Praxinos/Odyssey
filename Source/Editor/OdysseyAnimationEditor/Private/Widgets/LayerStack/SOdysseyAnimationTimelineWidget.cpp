// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineWidget.h"

void
SOdysseyAnimationTimelineWidget::Construct(
    const FArguments& iArgs,
	TSharedPtr<SOdysseyAnimationLayerStack> iLayerStackWidget
)
{
    mLayerStackWidget = iLayerStackWidget;
}

TSharedPtr<SOdysseyAnimationLayerStack>
SOdysseyAnimationTimelineWidget::GetLayerStackWidget() const
{
    return mLayerStackWidget.Pin();
}

FReply 
SOdysseyAnimationTimelineWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (MouseEvent.IsControlDown())
		{
			mIsOffsetting = true;
			mOffsetMousePosition = MouseEvent.GetScreenSpacePosition();
			mOffsetMousePosition.Y = GetLayerStackWidget()->GetTimelineOffset();
			return FReply::Handled();
		}
	}

    return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineWidget::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (mIsOffsetting)
	{
		const float minOffset = 0.0f;
		float mouseOffset = MouseEvent.GetScreenSpacePosition().X - mOffsetMousePosition.X;
        //mOffsetMousePosition.Y contains the starting offset instead of the Y position
		GetLayerStackWidget()->SetTimelineOffset(FMath::Max(minOffset, mOffsetMousePosition.Y - (mouseOffset / GetLayerStackWidget()->GetTimelineFrameWidth())));
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineWidget::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (mIsOffsetting)
	{
		mIsOffsetting = false;
		return FReply::Handled();
	}
    return FReply::Unhandled();
}
