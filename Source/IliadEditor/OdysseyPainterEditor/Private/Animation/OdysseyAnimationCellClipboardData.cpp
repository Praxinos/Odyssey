// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationCellClipboardData.h"

#include "OdysseyAnimationCell.h"
#include "OdysseyAnimationLayer.h"
#include "UObject/OdysseyObjectEditorUtils.h"

FOdysseyAnimationCellClipboardData::FOdysseyAnimationCellClipboardData()
    : IOdysseyClipboardData(StaticId())
{

}

FOdysseyAnimationCellClipboardData::FOdysseyAnimationCellClipboardData(const TArray<UOdysseyLayerCell*>& iCells)
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
FOdysseyAnimationCellClipboardData::Copy(const TArray<UOdysseyLayerCell*>& iCells)
{
    mCellCopies.Empty();
    for (UOdysseyLayerCell* cell : iCells)
    {
        mCellCopies.Add({cell, cell->GetExposure()});
    }
}

void
FOdysseyAnimationCellClipboardData::Paste(UOdysseyAnimationLayer* iLayer, int iFrame) const
{
    checkf(CanPaste(iLayer), TEXT("Can't Paste"));

    int cellIndex = INDEX_NONE;
    FInt32Range layerRange = iLayer->GetFrameRange();
    if (iFrame < layerRange.GetLowerBoundValue())
    {
        cellIndex = 0;
    }
    else if (iFrame > layerRange.GetUpperBoundValue())
    {
        UOdysseyLayerCell* lastCell = iLayer->GetCells().Last();
        lastCell->SetExposure(lastCell->GetExposure() + FMath::Max(0, (iFrame - layerRange.GetUpperBoundValue() - 1)));
        cellIndex = iLayer->GetCells().Num();
    }
    else
    {
        UOdysseyLayerCell* cell = iLayer->GetCellAtFrame(iFrame);
        cellIndex = cell->GetIndexInLayer();
        int cellFrame = iFrame - cell->GetFrameRange().GetLowerBoundValue();
        if (cellFrame != 0)
        {
            cell->Break(cellFrame, false);
            cellIndex++;
        }
    }

    int PreviousLayerOffset = iLayer->GetCellsOffset();
    for (int i = 0; i < mCellCopies.Num(); i++)
    {
        const FCellCopy& cellCopy = mCellCopies[i];
        UOdysseyLayerCell* cell = iLayer->CopyCell(cellCopy.mCell, cellIndex + i);
        cell->SetExposure(cellCopy.mExposure);

        if (i == mCellCopies.Num() - 1 && cellIndex == 0)
        {
            int addedExposures = iLayer->GetFrameRange().GetUpperBoundValue() - layerRange.GetUpperBoundValue();
            int offset = FMath::Max(0, FMath::Min(iFrame, iLayer->GetCellsOffset() - addedExposures));
            iLayer->SetCellsOffset( offset );
            cell->SetExposure(cellCopy.mExposure + FMath::Max(0, (PreviousLayerOffset - offset) - addedExposures));
        }
        else
        {
            cell->SetExposure(cellCopy.mExposure);
        }
    }
}

void
FOdysseyAnimationCellClipboardData::Move(UOdysseyAnimationLayer* iLayer, int iFrame) const
{
    if (!iLayer)
        return;

    struct FCellToExtend
    {
        UOdysseyLayerCell* Cell;
        int Value;
    };

    TMap<UOdysseyLayerCell*, int> CellsToExtend;

    int CellIndex = INDEX_NONE;
    FInt32Range layerRange = iLayer->GetFrameRange();
    if (iFrame < layerRange.GetLowerBoundValue())
    {
        CellIndex = 0;
    }
    else if (iFrame > layerRange.GetUpperBoundValue())
    {
        CellIndex = iLayer->GetCells().Num();
    }
    else
    {
        UOdysseyLayerCell* cell = iLayer->GetCellAtFrame(iFrame);
        CellIndex = cell->GetIndexInLayer();
    }

    for (int i = 0; i < mCellCopies.Num(); i++)
    {
        const FCellCopy& cellCopy = mCellCopies[i];
        UOdysseyLayer* Layer = cellCopy.mCell->GetLayer();
        if (!Layer || Layer != iLayer)
            continue;

        int IndexInLayer = cellCopy.mCell->GetIndexInLayer();
        if (IndexInLayer == INDEX_NONE)
            continue;

        int Exposure = cellCopy.mCell->GetExposure();

        //Remove Copied Cell from Layer
        Layer->RemoveCell(cellCopy.mCell);

        if (IndexInLayer < CellIndex)
        {
            CellIndex--;
        }

        int PreviousCellIndex = IndexInLayer - 1;
        if (PreviousCellIndex == INDEX_NONE)
        {
            Layer->SetCellsOffset(Layer->GetCellsOffset() + Exposure);
        }
        //Extend previous Cells or CellsOffset
        //Do not extend the cell after which we paste cells
        else// if (PreviousCellIndex != CellIndex - 1)
        {
            UOdysseyLayerCell* PreviousCell = Layer->GetCells()[PreviousCellIndex];
            int& Value = CellsToExtend.FindOrAdd(PreviousCell, 0);
            Value += Exposure;
        }
    }

    for (auto Element : CellsToExtend)
    {
        UOdysseyLayerCell* Cell = Element.Key;
        const int& Value = Element.Value;

        if (Cell->GetIndexInLayer() == CellIndex - 1)
        {
            if (iFrame > Cell->GetFrameRange().GetUpperBoundValue() + 1)
            {
                iFrame -= Value;
            }
            continue;
        }

        Cell->SetExposure(Cell->GetExposure() + Value);
    }

    Paste(iLayer, iFrame);
}

/* void
FOdysseyAnimationCellClipboardData::Move(UOdysseyAnimationLayer* iLayer, int iFrame) const
{
    Paste(iLayer, iFrame);

    for (int i = 0; i < mCellCopies.Num(); i++)
    {
        const FCellCopy& cellCopy = mCellCopies[i];
        UOdysseyLayer* Layer = cellCopy.mCell->GetLayer();
        if (!Layer)
            continue;

        int IndexInLayer = cellCopy.mCell->GetIndexInLayer();
        if (IndexInLayer == 0)
        {
            Layer->SetCellsOffset(Layer->GetCellsOffset() + cellCopy.mCell->GetExposure());
        }
        Layer->RemoveCell(cellCopy.mCell);
    }

    mCellCopies.Empty();
} */

bool
FOdysseyAnimationCellClipboardData::CanPaste(UOdysseyAnimationLayer* iLayer) const
{
    if (!iLayer->IsEditable())
        return false;

    for (const FCellCopy& cellCopy : mCellCopies)
    {
        if (!iLayer->GetSupportedCellTypes().Contains(cellCopy.mCell->GetClass()))
            return false;
    }

    return true;
}

int
FOdysseyAnimationCellClipboardData::GetCellCount() const
{
    return mCellCopies.Num();
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------- FGCObject implementation

void
FOdysseyAnimationCellClipboardData::AddReferencedObjects(FReferenceCollector& Collector)
{
    for (FCellCopy& cellCopy : mCellCopies)
    {
        Collector.AddReferencedObject(cellCopy.mCell);
    }
}

FString
FOdysseyAnimationCellClipboardData::GetReferencerName() const
{
    return "FOdysseyAnimationCellClipboardData";
}
