// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/OdysseyAnimationCellClipboardData.h"

#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "LayerStack/Cells/OdysseyAnimationCellsMutator.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"

FOdysseyAnimationCellClipboardData::FOdysseyAnimationCellClipboardData()
    : IOdysseyClipboardData(StaticId())
{

}

FOdysseyAnimationCellClipboardData::FOdysseyAnimationCellClipboardData(const TArray<TSharedPtr<FOdysseyAnimationCell>>& iCells)
    : IOdysseyClipboardData(StaticId())
{
    Copy(iCells);
}

const FGuid&
FOdysseyAnimationCellClipboardData::StaticId()
{
    static FGuid id = FGuid::NewGuid();
    return id;
}

void
FOdysseyAnimationCellClipboardData::Copy(const TArray<TSharedPtr<FOdysseyAnimationCell>>& iCells)
{
    mCellCopies.Empty();
    for (TSharedPtr<FOdysseyAnimationCell> cell : iCells)
    {
        mCellCopies.Add({cell, cell->GetLength()});
    }
}

void
FOdysseyAnimationCellClipboardData::Paste(UOdysseyAnimationLayer* iLayer, int iFrame) const
{
    checkf(CanPaste(iLayer), TEXT("Can't Paste"));

    FOdysseyAnimationCellsMutator mutator(iLayer, iLayer->GetCellsContainer());

    TArray<TSharedPtr<FOdysseyAnimationCell>> cells;
    for (const FCellCopy& cellCopy : mCellCopies)
    {
        TSharedPtr<FOdysseyAnimationCell> cell = cellCopy.mCell->Clone(iLayer, cellCopy.mLength);
        cells.Add(cell);
    }
    mutator.AddAtFrame(cells, iFrame);
}

bool
FOdysseyAnimationCellClipboardData::CanPaste(UOdysseyAnimationLayer* iLayer) const
{
    if (iLayer->GetIsLocked())
        return false;
    //TODO: check if iLayer cells types are compatible with the copied cells
    TSharedPtr<FOdysseyAnimationCellsContainer> cellContainer = iLayer->GetCellsContainer();
    if (!cellContainer)
        return false;
        
    for (const FCellCopy& cellCopy : mCellCopies)
    {
        if (!cellContainer->SupportsType(cellCopy.mCell->GetType()))
            return false;
    }

    return true;
}

int
FOdysseyAnimationCellClipboardData::GetCellCount() const
{
    return mCellCopies.Num();
}
