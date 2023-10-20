// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"

class ODYSSEYANIMATIONEDITOR_API SOdysseyAnimationCellImageStagger
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyAnimationCellImageStagger)
        {}
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& iArgs, TSharedPtr<FOdysseyAnimationCellImageStagger> iCell);
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const;

private:
    const FSlateBrush* GetBehaviourBrush() const;
    FText GetReachText() const;
    TSharedRef<SWidget> GetBehaviourMenuContent();

    void MapActions(TSharedPtr<FUICommandList> iCommandList);
    void BuildContextMenu(FMenuBuilder& iMenuBuilder);

    void SetBehaviour(FOdysseyAnimationCellImageStagger::eBehaviour iBehaviour);
    bool CanSetBehaviour(FOdysseyAnimationCellImageStagger::eBehaviour iBehaviour) const;

private:
    TSharedPtr<FOdysseyAnimationCellImageStagger> mCell;
};