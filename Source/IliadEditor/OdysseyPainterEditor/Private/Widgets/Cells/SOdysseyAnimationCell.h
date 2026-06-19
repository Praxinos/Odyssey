// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/NumericTypeInterface.h"

class FOdysseyPainterEditorAnimationTimelinePosition;
class UOdysseyAnimationLayer;
class UOdysseyLayerCell;
struct FSlateBrush;

class SOdysseyAnimationCell
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationCell)
        {}
        SLATE_DEFAULT_SLOT(FArguments, Content)
        SLATE_ARGUMENT(TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition>, TimelinePosition)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(
        const FArguments& iArgs,
        UOdysseyAnimationLayer* iAnimationLayer,
        UOdysseyLayerCell* iCell
    );

public:
    // SWidget interface
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
    // End of SWidget interface

private:
    EVisibility GetExposureVisibility() const;
    FText GetExposureText() const;
    FText GetExposureTooltip() const;

private:
    bool IsSelected() const;
    bool IsSelectionCursor() const;

    const FSlateBrush* GetMarkBrush( int32 iFrameInCell ) const;
    FLinearColor GetMarkColor( int32 iFrameInCell ) const;
    float GetMarkOpacity( int32 iFrameInCell ) const;
    bool IsMarkSymbol( int32 iFrameInCell ) const;
    bool IsMarkFill( int32 iFrameInCell ) const;

private:
    UOdysseyAnimationLayer* mAnimationLayer;
    UOdysseyLayerCell* mCell;

    TSharedPtr<FOdysseyPainterEditorAnimationTimelinePosition> mTimelinePosition;

    TSharedPtr<TDefaultNumericTypeInterface<int32>> mExposureInterface;
};
