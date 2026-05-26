// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Input/DragAndDrop.h"

class UOdysseyLayer;
class UOdysseyLayerStack;

class ODYSSEYLAYERSTACKEDITOR_API FOdysseyLayerStackDragDropOperation : public FDragDropOperation
{
public:
    FOdysseyLayerStackDragDropOperation(UOdysseyLayerStack* iLayerStack, TArray<UOdysseyLayer*> iLayers);

public:
    DRAG_DROP_OPERATOR_TYPE(FOdysseyLayerStackDragDropOperation, FDragDropOperation)

    using FDragDropOperation::Construct;

public:
    /**
     * @brief Get only the topmost layers, to avoid getting a parent layer and their children in the returned array
     *
     * @return TArray< UOdysseyLayer* >
     */
    TArray< UOdysseyLayer* > GetTopmostLayers() const;

    /**
     * @brief Get all layers without filtering
     *
     * @return TArray< UOdysseyLayer* >
     */
    const TArray< UOdysseyLayer* >& GetLayers() const;

    /**
     * @brief Get the Layer Stack object
     *
     * @return UOdysseyLayerStack*
     */
    UOdysseyLayerStack* GetLayerStack() const;

public:
    /**
     * Gets the widget that will serve as the decorator unless overridden.
     * If you do not override, you will have no decorator
     */
    virtual TSharedPtr<SWidget> GetDefaultDecorator() const override;

private:
    FText GetText() const;
    const struct FSlateBrush* GetIcon() const;

private:
    UOdysseyLayerStack* mLayerStack;
    TArray<UOdysseyLayer*> mLayers;
};
