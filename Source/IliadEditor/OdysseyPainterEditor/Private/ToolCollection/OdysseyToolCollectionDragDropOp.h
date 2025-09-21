// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

class UOdysseyPainterEditorTool;
class UOdysseyToolCollection;

class FOdysseyToolCollectionDragDropOp : public FDragDropOperation
{
public:
    DRAG_DROP_OPERATOR_TYPE(FOdysseyToolCollectionDragDropOp, FDragDropOperation)

public:
    static TSharedRef<FOdysseyToolCollectionDragDropOp> Create(UOdysseyPainterEditorTool* iTool, UOdysseyToolCollection* iSource);
    virtual TSharedPtr<SWidget> GetDefaultDecorator() const override;

public:
    UOdysseyPainterEditorTool* GetTool() const;

private:
    FText GetText() const;
    const FSlateBrush* GetIcon() const;

private:
    UOdysseyPainterEditorTool* mTool;
    TWeakObjectPtr<UOdysseyToolCollection> mSourceCollection;
};
