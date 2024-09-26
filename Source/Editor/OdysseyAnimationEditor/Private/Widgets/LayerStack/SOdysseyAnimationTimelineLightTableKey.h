// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"

class UOdysseyAnimationLayer;
class UOdysseyAnimationCell;
class FOdysseyAnimationEditorExtension;

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineLightTableKey
	: public SCompoundWidget
{
public:
	DECLARE_DELEGATE_OneParam(FOnKeyChanged, FOdysseyAnimationLightTableKey)
	
public:
	SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineLightTableKey)
	{}
		SLATE_ATTRIBUTE(UOdysseyAnimationCell*, Cell)
		SLATE_ATTRIBUTE(FOdysseyAnimationLightTableKey, Key)
		SLATE_EVENT(FOnKeyChanged, OnChanged)
		SLATE_EVENT(FOnKeyChanged, OnCommited)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, FOdysseyAnimationEditorExtension* iExtension);

private:
	bool IsOutOfPegsEnabled() const;
	const FSlateBrush* GetOutOfPegsButtonImage() const;
	void OnOutOfPegsCheckStateChanged(ECheckBoxState iValue);
	ECheckBoxState IsOutOfPegsChecked() const;

private:
	FOdysseyAnimationEditorExtension* mExtension = nullptr;
	TAttribute<UOdysseyAnimationCell*> mCell;
	TAttribute<FOdysseyAnimationLightTableKey> mKey;
};

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationTimelineLightTableKeySlider
	: public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineLightTableKeySlider)
	{}
		SLATE_ATTRIBUTE(FOdysseyAnimationLightTableKey, Key)
		SLATE_EVENT(SOdysseyAnimationTimelineLightTableKey::FOnKeyChanged, OnChanged)
		SLATE_EVENT(SOdysseyAnimationTimelineLightTableKey::FOnKeyChanged, OnCommited)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FReply OnMouseButtonDown(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) override;
	virtual FReply OnDragDetected(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent) override;
	virtual FVector2D ComputeDesiredSize(float iLayoutScaleMultiplier) const override;

private:
	TAttribute<FOdysseyAnimationLightTableKey> mKey;
	UOdysseyAnimationLayer* mLayer = nullptr;

	float mDraggingPosition = 0.f;
	float mOldOpacity = 0.f;
	bool mDragging = false;

	SOdysseyAnimationTimelineLightTableKey::FOnKeyChanged mOnChanged;
	SOdysseyAnimationTimelineLightTableKey::FOnKeyChanged mOnCommited;
};
