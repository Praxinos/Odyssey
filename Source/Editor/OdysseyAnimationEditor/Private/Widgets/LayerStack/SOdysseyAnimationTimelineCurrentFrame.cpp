// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineCurrentFrame.h"

SOdysseyAnimationTimelineCurrentFrame::SOdysseyAnimationTimelineCurrentFrame()
	: mEditor(nullptr)
{
}

void
SOdysseyAnimationTimelineCurrentFrame::Construct(
    const FArguments& iArgs,
	FOdysseyAnimationEditor* iEditor
)
{
    mEditor = iEditor;
}

int32
SOdysseyAnimationTimelineCurrentFrame::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const FSlateBrush* GenericBrush = FCoreStyle::Get().GetBrush("GenericWhiteBox");

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

	++LayerId;
	return LayerId;
}

FVector2D
SOdysseyAnimationTimelineCurrentFrame::ComputeDesiredSize(float) const
{
	return FVector2D(); //no desired size
}

FReply
SOdysseyAnimationTimelineCurrentFrame::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return FReply::Unhandled();
}