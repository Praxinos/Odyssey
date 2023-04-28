// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineWidget.h"

SOdysseyAnimationTimelineWidget::SOdysseyAnimationTimelineWidget()
	: mEditor(nullptr)
	, mOffsetMousePosition(0)
	, mIsOffsetting(false)
    , mIsScrubbing(false)
{
}

void
SOdysseyAnimationTimelineWidget::Construct(
    const FArguments& iArgs,
	FOdysseyAnimationEditor* iEditor
)
{
    mEditor = iEditor;

	ChildSlot
	[
		SAssignNew(mPanel, SOdysseyAnimationTimelinePanel, iEditor)
		.Clipping(EWidgetClipping::ClipToBounds)
		.BaseOffset(iArgs._BaseOffset)
	];
}

FOdysseyAnimationEditor*
SOdysseyAnimationTimelineWidget::GetEditor() const
{
    return mEditor;
}

void
SOdysseyAnimationTimelineWidget::ClearChildren()
{
	mPanel->ClearChildren();
}

SOdysseyAnimationTimelineWidget::FScopedWidgetSlotArguments
SOdysseyAnimationTimelineWidget::AddChild()
{
	return mPanel->AddChild();
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
			mOffsetMousePosition.Y = GetEditor()->Timeline()->GetOffset();
    		return FReply::Handled().CaptureMouse(AsShared()).PreventThrottling();
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
		GetEditor()->Timeline()->SetOffset(FMath::Max(minOffset, mOffsetMousePosition.Y - (mouseOffset / GetEditor()->Timeline()->GetFrameWidth())));
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
    	return FReply::Handled().CaptureMouse(AsShared());
	}
    return FReply::Unhandled();
}

//=================
//=================
//=================

SOdysseyAnimationTimelinePanel::SOdysseyAnimationTimelinePanel()
	: mChildren(this)
{
}

void
SOdysseyAnimationTimelinePanel::Construct(
    const FArguments& iArgs,
	FOdysseyAnimationEditor* iEditor
)
{
    mEditor = iEditor;
	mBaseOffset = iArgs._BaseOffset;
}

FChildren*
SOdysseyAnimationTimelinePanel::GetChildren()
{
	return &mChildren;
}

void
SOdysseyAnimationTimelinePanel::ClearChildren()
{
	mChildren.Empty();
}

SOdysseyAnimationTimelinePanel::FScopedWidgetSlotArguments
SOdysseyAnimationTimelinePanel::AddChild()
{
	return FScopedWidgetSlotArguments{ MakeUnique<FSlot>(), mChildren, INDEX_NONE };
}

FVector2D
SOdysseyAnimationTimelinePanel::ComputeDesiredSize(float) const
{
	FVector2D desiredSize = FVector2D::ZeroVector;
	for (int32 SlotIndex = 0; SlotIndex < mChildren.Num(); ++SlotIndex)
	{
		const FSlot& slot = mChildren[SlotIndex];
		if (slot.GetWidget()->GetVisibility() != EVisibility::Collapsed)
		{
			const FVector2D childDesiredSize = slot.GetWidget()->GetDesiredSize();
			desiredSize.X += childDesiredSize.X + slot.GetPadding().GetTotalSpaceAlong<Orient_Horizontal>();
			desiredSize.Y = FMath::Max(childDesiredSize.Y, desiredSize.Y);
		}
	}
	return desiredSize;
}

void
SOdysseyAnimationTimelinePanel::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	float scrollPadding = AllottedGeometry.GetLocalSize().X;
	float currentChildOffset = -mEditor->Timeline()->GetOffset() * mEditor->Timeline()->GetFrameWidth() + mBaseOffset;

	for (int32 SlotIndex = 0; SlotIndex < mChildren.Num(); ++SlotIndex)
	{
		const FSlot& slot = mChildren[SlotIndex];
		const EVisibility childVisibility = slot.GetWidget()->GetVisibility();

		if (childVisibility == EVisibility::Collapsed)
			continue;
		
		currentChildOffset = ArrangeChildHorizontalAndReturnOffset(AllottedGeometry, ArrangedChildren, slot, currentChildOffset);
	}
}

float
SOdysseyAnimationTimelinePanel::ArrangeChildHorizontalAndReturnOffset(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren, const SScrollBox::FSlot& ThisSlot, float CurChildOffset) const
{
	const FMargin& ThisPadding = ThisSlot.GetPadding();
	const FVector2D& WidgetDesiredSize = ThisSlot.GetWidget()->GetDesiredSize();
	const float ThisSlotDesiredWidth = WidgetDesiredSize.X + ThisPadding.GetTotalSpaceAlong<Orient_Horizontal>();

	// Figure out the size and local position of the child within the slot.  There is no horizontal alignment, because
	// it doesn't make sense in a panel where items are stacked horizontally end-to-end.
	AlignmentArrangeResult YAlignmentResult = AlignChild<Orient_Vertical>(AllottedGeometry.GetLocalSize().Y, ThisSlot, ThisPadding);

	ArrangedChildren.AddWidget(AllottedGeometry.MakeChild(ThisSlot.GetWidget(), FVector2D(CurChildOffset + ThisPadding.Left, YAlignmentResult.Offset), FVector2D(WidgetDesiredSize.X, YAlignmentResult.Size)));
	return CurChildOffset + ThisSlotDesiredWidth;
}

int32
SOdysseyAnimationTimelinePanel::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	// Draw a current frame
	LayerId = SPanel::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	++LayerId;

	const FSlateBrush* GenericBrush = FCoreStyle::Get().GetBrush( "GenericWhiteBox" );

	const float height = AllottedGeometry.GetLocalSize().Y;  
	const float width = AllottedGeometry.GetLocalSize().X;
	float offset = mEditor->Timeline()->GetOffset();
	const float frameSize = mEditor->Timeline()->GetFrameWidth();

	FLinearColor lineColor = FLinearColor::Red;
	lineColor.A = 0.3f;

	int currentFrame = mEditor->Animation()->GetFrameIndexAtTime(mEditor->Player()->GetCurrentTime());
	float currentFramePos = (currentFrame - offset) * frameSize + mBaseOffset;

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
