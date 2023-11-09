// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationLightTableMutator;

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineLightTableKey
	: public SLeafWidget
{
public:
	static inline const float mDesiredHeight = 32.f;
	static inline const float mDesiredWidth = 16.f;

public:
	SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineLightTableKey)
		: _IsActivated(false)
	{}
		SLATE_ATTRIBUTE(bool, IsActivated)
	SLATE_END_ARGS()

	SOdysseyAnimationTimelineLightTableKey();

	void Construct(
		const FArguments& InArgs,
		UOdysseyAnimationLayer* iLayer,
		int iKeyIndex);

private:
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FReply OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) override;
	virtual FReply OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) override;
	virtual FVector2D ComputeDesiredSize(float iLayoutScaleMultiplier) const override;

private:
	UOdysseyAnimationLayer* mLayer;
	int mKeyIndex;
	TAttribute<bool> mIsActivated;
	TSharedPtr<FOdysseyAnimationLightTableMutator> mOpacityMutator;
	float mDraggingPosition;
	float mOldOpacity;
	bool mDragging;
};
