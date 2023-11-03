// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationTimelineLightTableKey.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTableMutator.h"

#define LOCTEXT_NAMESPACE "SOdysseyAnimationTimelineLightTableKey"

SOdysseyAnimationTimelineLightTableKey::SOdysseyAnimationTimelineLightTableKey()
	: mLayer(nullptr)
	, mKeyIndex(INDEX_NONE)
	, mOpacityMutator(nullptr)
	, mDraggingPosition(0.f)
	, mOldOpacity(0.f)
	, mDragging(false)
{
}



int32
SOdysseyAnimationTimelineLightTableKey::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const FSlateBrush* GenericBrush = FCoreStyle::Get().GetBrush( "GenericWhiteBox" );

	const float height = AllottedGeometry.GetLocalSize().Y;
	const float width = AllottedGeometry.GetLocalSize().X;

	//Dragging Zone
	FLinearColor color = FOdysseyStyle::GetColor( "Animation.LightTableKey.Color" );
	float opacity = mLayer->GetLightTable()->GetKeyOpacity(mKeyIndex);


	//Dragging Bar
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(FVector2D(0.f, height * (1.0f - opacity)), FVector2D(width, height * opacity)),
		GenericBrush,
		ESlateDrawEffect::None,
		color
	);

	// Draw a current frame
	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

void
SOdysseyAnimationTimelineLightTableKey::Construct(const FArguments& InArgs, UOdysseyAnimationLayer* iLayer, int iKeyIndex)
{
	mLayer = iLayer;
	mKeyIndex = iKeyIndex;

	ChildSlot
	[
		SNew(SBorder)
		.Padding(FMargin(0))
	];
}

FReply
SOdysseyAnimationTimelineLightTableKey::OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
	if (iMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{	
		// This has prevent throttling on so that viewports continue to run whilst dragging the slider
		return FReply::Handled().DetectDrag(SharedThis(this), EKeys::LeftMouseButton);
	}

	return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineLightTableKey::OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
	mDraggingPosition = iMouseEvent.GetScreenSpacePosition().Y;
	mOldOpacity = mLayer->GetLightTable()->GetKeyOpacity(mKeyIndex);
	mDragging = true;
	mOpacityMutator = MakeShared<FOdysseyAnimationLightTableMutator>(mLayer->GetLightTable());
	return FReply::Handled().CaptureMouse( SharedThis(this) ).PreventThrottling();
}

FReply
SOdysseyAnimationTimelineLightTableKey::OnMouseMove(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
	if (mDragging)
	{	
		float position = iMouseEvent.GetScreenSpacePosition().Y;
		float delta = (position - mDraggingPosition) * -1;
		float sensitivity = 500.f;
		
		FOdysseyAnimationLightTableMutator mutator(mLayer->GetLightTable());
		mutator.SetKeyOpacity(mKeyIndex, mOldOpacity + delta / sensitivity);
		mutator.Commit();

		// This has prevent throttling on so that viewports continue to run whilst dragging the slider
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply
SOdysseyAnimationTimelineLightTableKey::OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent)
{
	if (mDragging)
	{
		//End Mutation
		mOpacityMutator->Commit();
		mOpacityMutator = nullptr;
		mDragging = false;
		return FReply::Handled().ReleaseMouseCapture();
	}

	if (iMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{	
		bool isActivated = mLayer->GetLightTable()->GetKeyIsActivated(mKeyIndex);

		//TODO: Toggle Key
		FOdysseyAnimationLightTableMutator mutator(mLayer->GetLightTable());
		mutator.SetKeyIsActivated(mKeyIndex, !isActivated);
		mutator.Commit();
		// This has prevent throttling on so that viewports continue to run whilst dragging the slider
		return FReply::Handled();
	}

	return FReply::Unhandled();
}


#undef LOCTEXT_NAMESPACE
