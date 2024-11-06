// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Cells/OdysseyAnimationCellClipboardData.h"

class FOdysseyAnimationCellsDragDropOperation : public FDragDropOperation
{
public:
    static TSharedRef<FOdysseyAnimationCellsDragDropOperation> Create(UOdysseyAnimationLayer* iLayer, const TArray<UOdysseyAnimationCell*>& iCells);

public:
    FOdysseyAnimationCellsDragDropOperation(UOdysseyAnimationLayer* iLayer, const TArray<UOdysseyAnimationCell*>& iCells);

public:
    DRAG_DROP_OPERATOR_TYPE(FOdysseyAnimationCellsDragDropOperation, FDragDropOperation)
    using FDragDropOperation::Construct;

public:
    /**
     * Gets the widget that will serve as the decorator unless overridden.
     * If you do not override, you will have no decorator
     */
    virtual TSharedPtr<SWidget> GetDefaultDecorator() const override;

public:
    const FOdysseyAnimationCellClipboardData& GetData() const;
    UOdysseyAnimationLayer* GetLayer() const;

private:
    FText GetText() const;
    const FSlateBrush* GetIcon() const;

private:
    FOdysseyAnimationCellClipboardData mData;
    UOdysseyAnimationLayer* mLayer;
};