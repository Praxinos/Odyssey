// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "DragDropOperations/OdysseyAnimationCellsDragDropOperation.h"

#include "OdysseyLayer.h"
#include "OdysseyLayerStack.h"
#include "OdysseyStyle.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

TSharedRef<FOdysseyAnimationCellsDragDropOperation>
FOdysseyAnimationCellsDragDropOperation::Create(UOdysseyAnimationLayer* iLayer, const TArray<UOdysseyAnimationCell*>& iCells)
{
    TSharedRef<FOdysseyAnimationCellsDragDropOperation> operation =  MakeShared<FOdysseyAnimationCellsDragDropOperation>(iLayer, iCells);
    operation->Construct();
    return operation;
}

FOdysseyAnimationCellsDragDropOperation::FOdysseyAnimationCellsDragDropOperation(UOdysseyAnimationLayer* iLayer, const TArray<UOdysseyAnimationCell*>& iCells)
    : mData(iCells)
    , mLayer(iLayer)
{
}

const FOdysseyAnimationCellClipboardData&
FOdysseyAnimationCellsDragDropOperation::GetData() const
{
    return mData;
}

UOdysseyAnimationLayer*
FOdysseyAnimationCellsDragDropOperation::GetLayer() const
{
    return mLayer;
}

TSharedPtr<SWidget>
FOdysseyAnimationCellsDragDropOperation::GetDefaultDecorator() const
{
    return SNew(SBorder)
        .BorderImage(FAppStyle::GetBrush("Graph.ConnectorFeedback.Border")) //weird, but everyone does this
        .Content()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(0.f, 0.f, 2.f, 0.f)
            [
                SNew(SImage)
                .Image(this, &FOdysseyAnimationCellsDragDropOperation::GetIcon)
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(STextBlock)
                .Text(this, &FOdysseyAnimationCellsDragDropOperation::GetText)
            ]
        ];
}

FText
FOdysseyAnimationCellsDragDropOperation::GetText() const
{
    return FText::Format(LOCTEXT("cells-drag-drop-operation.cells-count", "{0} Cells"), FText::AsNumber(mData.GetCellCount()));
}

const FSlateBrush*
FOdysseyAnimationCellsDragDropOperation::GetIcon() const
{
    return FOdysseyStyle::GetBrush("PainterEditor.Layers16");
}

#undef LOCTEXT_NAMESPACE
