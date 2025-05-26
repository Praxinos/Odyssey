// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyLighttable.h"

class UOdysseyAnimationLayer;
class UOdysseyAnimationCell;
class FOdysseyPainterEditorAnimationTimelinePosition;

class SOdysseyAnimationTimelineLighttableKey
    : public SCompoundWidget
{
public:
    DECLARE_DELEGATE_OneParam(FOnKeyChanged, FOdysseyLighttableKey)

public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineLighttableKey)
    {}
        SLATE_ARGUMENT(TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>, TimelinePosition)
        SLATE_ATTRIBUTE(UOdysseyAnimationCell*, Cell)
        SLATE_ATTRIBUTE(FOdysseyLighttableKey, Key)
        SLATE_EVENT(FOnKeyChanged, OnChanged)
        SLATE_EVENT(FOnKeyChanged, OnCommited)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    TAttribute<UOdysseyAnimationCell*> mCell;
    TAttribute<FOdysseyLighttableKey> mKey;
};

class SOdysseyAnimationTimelineLighttableKeySlider
    : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationTimelineLighttableKeySlider)
    {}
        SLATE_ATTRIBUTE(FOdysseyLighttableKey, Key)
        SLATE_EVENT(SOdysseyAnimationTimelineLighttableKey::FOnKeyChanged, OnChanged)
        SLATE_EVENT(SOdysseyAnimationTimelineLighttableKey::FOnKeyChanged, OnCommited)
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
    TAttribute<FOdysseyLighttableKey> mKey;
    UOdysseyAnimationLayer* mLayer = nullptr;

    float mDraggingPosition = 0.f;
    float mOldOpacity = 0.f;
    bool mDragging = false;

    SOdysseyAnimationTimelineLighttableKey::FOnKeyChanged mOnChanged;
    SOdysseyAnimationTimelineLighttableKey::FOnKeyChanged mOnCommited;
};
