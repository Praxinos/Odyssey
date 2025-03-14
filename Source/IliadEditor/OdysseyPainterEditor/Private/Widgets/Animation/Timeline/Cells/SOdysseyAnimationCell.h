// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

struct FSlateBrush;
class UOdysseyAnimationLayer;
class UOdysseyAnimationCell;

class SOdysseyAnimationCell
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationCell)
        {}
        SLATE_DEFAULT_SLOT(FArguments, Content)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(
        const FArguments& iArgs,
        UOdysseyAnimationLayer* iAnimationLayer,
        UOdysseyAnimationCell* iCell
    );

public:
    // SWidget interface
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
    // End of SWidget interfacepublic:

private:
    bool IsSelected() const;
    bool IsSelectionCursor() const;

    const FSlateBrush* GetMarkBrush() const;
    FLinearColor GetMarkColor() const;
    float GetMarkOpacity() const;
    bool IsMarkSymbol() const;
    bool IsMarkFill() const;

private:
    UOdysseyAnimationLayer* mAnimationLayer;
    UOdysseyAnimationCell* mCell;
};
