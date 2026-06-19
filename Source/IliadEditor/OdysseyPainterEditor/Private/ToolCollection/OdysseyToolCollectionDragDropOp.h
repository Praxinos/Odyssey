// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Input/DragAndDrop.h"

class UOdysseyPainterEditorToolConfiguration;
class UOdysseyToolCollection;
class FOdysseyPainterEditor;

class FOdysseyToolCollectionDragDropOp : public FDragDropOperation
{
public:
    DRAG_DROP_OPERATOR_TYPE(FOdysseyToolCollectionDragDropOp, FDragDropOperation)

public:
    static TSharedRef<FOdysseyToolCollectionDragDropOp> Create(UOdysseyPainterEditorToolConfiguration* iToolConfig, UOdysseyToolCollection* iSource, FOdysseyPainterEditor* iEditor);
    virtual TSharedPtr<SWidget> GetDefaultDecorator() const override;

public:
    UOdysseyPainterEditorToolConfiguration* GetToolConfig() const;
    TWeakObjectPtr<UOdysseyToolCollection> GetSourceCollection() const;
    FOdysseyPainterEditor* GetEditor() const;

private:
    FText GetText() const;
    const struct FSlateBrush* GetIcon() const;

private:
    UOdysseyPainterEditorToolConfiguration* mToolConfig;
    TWeakObjectPtr<UOdysseyToolCollection> mSourceCollection;
    FOdysseyPainterEditor* mEditor;
};
