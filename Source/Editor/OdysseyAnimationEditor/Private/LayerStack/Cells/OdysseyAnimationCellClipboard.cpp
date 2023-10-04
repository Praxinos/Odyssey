// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/OdysseyAnimationCellClipboard.h"

#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"

FOdysseyAnimationCellClipboard*
FOdysseyAnimationCellClipboard::Get()
{
    static FOdysseyAnimationCellClipboard clipboard;
    return &clipboard;
}

FOdysseyAnimationCellClipboard::FOdysseyAnimationCellClipboard()
{
}

void
FOdysseyAnimationCellClipboard::Copy(TSharedRef<FOdysseyAnimationCellsContainer> iCellContainer, const FInt32Range& iSelectedFrames)
{
    mCellCopies.Empty();

    const TArray<TSharedPtr<FOdysseyAnimationCell>>& cells = iCellContainer->GetCells();
    int startCellIndex = iCellContainer->GetCellIndexAtFrame(iSelectedFrames.GetLowerBoundValue());
    int startCellFrameIndex = iCellContainer->GetCellFrameAtFrame(iSelectedFrames.GetLowerBoundValue());
    int endCellIndex = iCellContainer->GetCellIndexAtFrame(iSelectedFrames.GetUpperBoundValue());
    int endCellFrameIndex = iCellContainer->GetCellFrameAtFrame(iSelectedFrames.GetUpperBoundValue());

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
FOdysseyAnimationCellClipboard::Paste(UOdysseyAnimationLayer* iLayer, TSharedRef<FOdysseyAnimationCellsContainer> iCellContainer, int iFrame)
{
    FOdysseyAnimationCellsMutator mutator(iLayer, iCellContainer);

    TArray<TSharedPtr<FOdysseyAnimationCell>> cells;
    for (const FCellCopy& cellCopy : mCellCopies)
    {
        TSharedPtr<FOdysseyAnimationCell> cell = cellCopy.mCell->Clone(iLayer, cellCopy.mLength);
        cells.Add(cell);
    }
    mutator.AddAtFrame(cells, iFrame);
}

TArray<TSharedPtr<FOdysseyAnimationCell>>
FOdysseyAnimationCellClipboard::GetCells() const
{
    TArray<TSharedPtr<FOdysseyAnimationCell>> cells;
    for (const FCellCopy& cellCopy : mCellCopies)
    {
        cells.Add(cellCopy.mCell);
    }

    return cells;
}
