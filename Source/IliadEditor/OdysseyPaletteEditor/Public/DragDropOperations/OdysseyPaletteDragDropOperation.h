// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once
#include "Input/DragAndDrop.h"

class UOdysseyPaletteEntry;
class UOdysseyPalette;

class ODYSSEYPALETTEEDITOR_API FOdysseyPaletteDragDropOperation : public FDragDropOperation
{
public:
    FOdysseyPaletteDragDropOperation(UOdysseyPalette* iLayerStack, TArray<UOdysseyPaletteEntry*> iLayers);

public:
    DRAG_DROP_OPERATOR_TYPE(FOdysseyPaletteDragDropOperation, FDragDropOperation)

    using FDragDropOperation::Construct;

public:
    /**
     * @brief Get only the topmost layers, to avoid getting a parent layer and their children in the returned array
     *
     * @return TArray< UOdysseyPaletteEntry* >
     */
    TArray< UOdysseyPaletteEntry* > GetTopmostPaletteEntries() const;

    /**
     * @brief Get all layers without filtering
     *
     * @return TArray< UOdysseyPaletteEntry* >
     */
    TArray< UOdysseyPaletteEntry* > GetPaletteEntries() const;

    /**
     * @brief Get the Layer Stack object
     *
     * @return UOdysseyPalette*
     */
    UOdysseyPalette* GetPalette() const;

public:
    /**
     * Gets the widget that will serve as the decorator unless overridden.
     * If you do not override, you will have no decorator
     */
    virtual TSharedPtr<SWidget> GetDefaultDecorator() const override;

private:
    FText GetText() const;
    const FSlateBrush* GetIcon() const;

private:
    UOdysseyPalette* mPalette;
    TArray<UOdysseyPaletteEntry*> mPaletteEntries;
};
