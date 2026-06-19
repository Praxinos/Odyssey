// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyAnimationCellClipboardData.h"
#include "Input/DragAndDrop.h"

struct FSlateBrush;

class FOdysseyAnimationCellsDragDropOperation : public FDragDropOperation
{
public:
    static TSharedRef<FOdysseyAnimationCellsDragDropOperation> Create(UOdysseyAnimationLayer* iLayer, const TArray<UOdysseyLayerCell*>& iCells);

public:
    FOdysseyAnimationCellsDragDropOperation(UOdysseyAnimationLayer* iLayer, const TArray<UOdysseyLayerCell*>& iCells);

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
