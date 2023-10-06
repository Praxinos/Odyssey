// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/OdysseyAnimationCellClipboardData.h"

#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"

FOdysseyAnimationCellClipboardData::FOdysseyAnimationCellClipboardData()
    : IOdysseyClipboardData(StaticId())
{

}

FOdysseyAnimationCellClipboardData::FOdysseyAnimationCellClipboardData(UOdysseyAnimationLayer* iLayer, const FInt32Range& iSelectedFrames)
    : IOdysseyClipboardData(StaticId())
{
    Copy(iLayer->GetCellsContainer(), iSelectedFrames);
}

const FGuid&
FOdysseyAnimationCellClipboardData::StaticId()
{
    static FGuid id = FGuid::NewGuid();
    return id;
}

void
FOdysseyAnimationCellClipboardData::Copy(TSharedPtr<FOdysseyAnimationCellsContainer> iCellContainer, const FInt32Range& iSelectedFrames)
{
    mCellCopies.Empty();

    if (iCellContainer->GetCells().Num() == 0)
        return;

    const TArray<TSharedPtr<FOdysseyAnimationCell>>& cells = iCellContainer->GetCells();
    int startCellIndex = iCellContainer->GetCellIndexAtFrame(iSelectedFrames.GetLowerBoundValue());
    int startCellFrameIndex = iCellContainer->GetCellFrameAtFrame(iSelectedFrames.GetLowerBoundValue());
    int endCellIndex = iCellContainer->GetCellIndexAtFrame(iSelectedFrames.GetUpperBoundValue());
    int endCellFrameIndex = iCellContainer->GetCellFrameAtFrame(iSelectedFrames.GetUpperBoundValue());

    if (startCellIndex == INDEX_NONE)
        startCellIndex = 0;

    if (endCellIndex == INDEX_NONE)
        endCellIndex = iCellContainer->GetCells().Num() - 1;

    if (startCellIndex == endCellIndex)
    {
        mCellCopies.Add({cells[startCellIndex], endCellFrameIndex - startCellFrameIndex + 1});
        return;
    }

    mCellCopies.Add({cells[startCellIndex], cells[startCellIndex]->GetLength() - startCellFrameIndex});
    for (int i = startCellIndex + 1; i <= endCellIndex - 1; i++)
    {
        mCellCopies.Add({cells[i], cells[i]->GetLength()});
    }

    mCellCopies.Add({cells[endCellIndex], endCellFrameIndex + 1});
}

void
FOdysseyAnimationCellClipboardData::Paste(UOdysseyAnimationLayer* iLayer, int iFrame) const
{
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
    //TODO: check if iLayer cells types are compatible with the copied cells
    return true;
}
