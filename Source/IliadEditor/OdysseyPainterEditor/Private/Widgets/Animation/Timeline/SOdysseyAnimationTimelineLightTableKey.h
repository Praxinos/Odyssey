// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "OdysseyAnimationLightTable.h"

class UOdysseyAnimationLayer;
class UOdysseyAnimationCell;
class FOdysseyPainterEditorAnimationTimelinePosition;

class SOdysseyAnimationTimelineLightTableKey
    : public SCompoundWidget
{
public:
    DECLARE_DELEGATE_OneParam(FOnKeyChanged, FOdysseyAnimationLightTableKey)

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineLightTableKey)
    {}
        SLATE_ARGUMENT(TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>, TimelinePosition)
        SLATE_ATTRIBUTE(UOdysseyAnimationCell*, Cell)
        SLATE_ATTRIBUTE(FOdysseyAnimationLightTableKey, Key)
        SLATE_EVENT(FOnKeyChanged, OnChanged)
        SLATE_EVENT(FOnKeyChanged, OnCommited)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    TAttribute<UOdysseyAnimationCell*> mCell;
    TAttribute<FOdysseyAnimationLightTableKey> mKey;
};

class SOdysseyAnimationTimelineLightTableKeySlider
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
