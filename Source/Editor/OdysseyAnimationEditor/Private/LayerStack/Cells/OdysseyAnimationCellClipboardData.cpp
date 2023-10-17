// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/OdysseyAnimationCellClipboardData.h"

#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"

FOdysseyAnimationCellClipboardData::FOdysseyAnimationCellClipboardData()
    : IOdysseyClipboardData(StaticId())
{

}

FOdysseyAnimationCellClipboardData::FOdysseyAnimationCellClipboardData(UOdysseyAnimationLayer* iLayer, const FInt32Range& iSelectedFrames)
    : IOdysseyClipboardData(StaticId())
    , mLayer(iLayer)
    , mSelectedFrames(iSelectedFrames)
{
    Copy();
}

const FGuid&
FOdysseyAnimationCellClipboardData::StaticId()
{
    static FGuid id = FGuid::NewGuid();
    return id;
}

void
FOdysseyAnimationCellClipboardData::Copy()
{
    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = mLayer->GetCellsContainer();
    mCellCopies.Empty();

    if (cellsContainer->GetCells().Num() == 0)
        return;

    const TArray<TSharedPtr<FOdysseyAnimationCell>>& cells = cellsContainer->GetCells();
    int startCellIndex = cellsContainer->GetCellIndexAtFrame(mSelectedFrames.GetLowerBoundValue());
    int startCellFrameIndex = cellsContainer->GetCellFrameAtFrame(mSelectedFrames.GetLowerBoundValue());
    int endCellIndex = cellsContainer->GetCellIndexAtFrame(mSelectedFrames.GetUpperBoundValue());
    int endCellFrameIndex = cellsContainer->GetCellFrameAtFrame(mSelectedFrames.GetUpperBoundValue());

    if (startCellIndex == INDEX_NONE)
        startCellIndex = 0;

    if (endCellIndex == INDEX_NONE)
        endCellIndex = cellsContainer->GetCells().Num() - 1;

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

UOdysseyAnimationLayer*
FOdysseyAnimationCellClipboardData::GetLayer() const
{
    return mLayer;
}

const FInt32Range&
FOdysseyAnimationCellClipboardData::GetSelectedFrames() const
{
    return mSelectedFrames;
}